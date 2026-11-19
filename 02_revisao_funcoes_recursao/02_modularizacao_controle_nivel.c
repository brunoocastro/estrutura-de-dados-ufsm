/*
 * ============================================================================
 * Arquivo : 02_modularizacao_controle_nivel.c
 * Módulo  : 02 — Revisão de funções e recursividade   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver uma rotina de controle dividida em funções pequenas, cada uma com
 *   uma responsabilidade: ler o sensor, filtrar, calcular o erro, calcular
 *   a ação de controle, saturar e atualizar o processo. A cada ciclo a tela
 *   mostra o valor que entra e sai de cada função e uma barra com o nível.
 *   Depois troca SÓ a função do controlador (P -> liga/desliga) e compara
 *   os dois resultados: o benefício da modularização.
 *
 * EXEMPLO REAL
 *   Controle de nível de um tanque de água com bomba de velocidade variável:
 *   é a estrutura de qualquer bloco de controle em CLP (bloco de função
 *   FB/FC na IEC 61131-3) ou de um laço de controle em microcontrolador.
 *
 * COMPLEXIDADE (K = número de ciclos simulados)
 *   Operação                         Tempo     Espaço
 *   cada função do ciclo             O(1)      O(1)
 *   um ciclo de controle             O(1)      O(1)
 *   simulação completa               O(K)      O(1)
 *   Por quê: cada função faz um número fixo de contas; o ciclo chama um
 *   número fixo de funções; a simulação repete o ciclo K vezes. Dividir em
 *   funções não muda o Big O, mas deixa cada parte testável e trocável.
 *
 * COMO EXECUTAR
 *   ./run 02_modularizacao_controle_nivel               (ENTER avança cada passo)
 *   ./run 02_modularizacao_controle_nivel < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <unistd.h>

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

#define SETPOINT        60.0   /* nível desejado, % */
#define NIVEL_INICIAL   20.0
#define CICLOS          40
#define KP              5.0    /* ganho proporcional: % de bomba por % de erro */
#define VAZAO_BOMBA     4.0    /* % de nível por ciclo com bomba a 100% */
#define VAZAO_SAIDA     1.5    /* consumo constante, % de nível por ciclo */
#define ALFA_FILTRO     0.5    /* filtro exponencial: 0 = só passado, 1 = sem filtro */
#define BANDA_ONOFF     5.0    /* histerese do liga/desliga: SP ± 5% */

/* ---------------------------------------------------------------------------
 * Funções do ciclo de controle: cada uma faz UMA coisa
 * ------------------------------------------------------------------------- */

/* Simula o transmissor de nível: valor real + ruído determinístico
   (sempre o mesmo, para a saída ser reprodutível). O(1). */
static double ler_sensor(double nivel_real, int ciclo) {
    const double ruido[8] = {0.6, -0.4, 0.2, -0.7, 0.5, -0.1, -0.5, 0.4};
    return nivel_real + ruido[ciclo % 8];
}

/* Filtro passa-baixas exponencial: y = alfa·x + (1 - alfa)·y_anterior.
   Não precisa de vetor (diferente da média móvel): O(1) tempo e espaço. */
static double filtrar(double medida, double filtrado_anterior) {
    return ALFA_FILTRO * medida + (1.0 - ALFA_FILTRO) * filtrado_anterior;
}

/* Erro = desejado - medido. Positivo: falta água. O(1). */
static double calcular_erro(double setpoint, double medido) {
    return setpoint - medido;
}

/* Controlador proporcional: ação proporcional ao erro. O(1). */
static double controlador_p(double erro, double estado_anterior) {
    (void)estado_anterior;   /* o P não tem memória; o parâmetro existe para
                                que os dois controladores tenham a MESMA assinatura */
    return KP * erro;
}

/* Controlador liga/desliga com histerese: 100% abaixo de SP-banda, 0% acima
   de SP+banda, mantém o estado anterior entre os dois. O(1). */
static double controlador_onoff(double erro, double estado_anterior) {
    if (erro > BANDA_ONOFF)  return 100.0;
    if (erro < -BANDA_ONOFF) return 0.0;
    return estado_anterior;
}

/* Limita a ação à faixa física do atuador (0..100% da bomba). O(1). */
static double saturar(double u, double minimo, double maximo) {
    if (u < minimo) return minimo;
    if (u > maximo) return maximo;
    return u;
}

/* Modelo do tanque: entra água proporcional à bomba, sai um consumo fixo.
   O nível fica entre 0 e 100%. O(1). */
static double atualizar_planta(double nivel, double u) {
    return saturar(nivel + VAZAO_BOMBA * u / 100.0 - VAZAO_SAIDA, 0.0, 100.0);
}

/* Barra do nível com o setpoint marcado por '|'. Cada caractere = 4%. */
static void barra_nivel(double nivel) {
    int cheio = (int)(nivel / 4.0 + 0.5);
    int sp = (int)(SETPOINT / 4.0);
    putchar('[');
    for (int k = 0; k < 25; k++)
        putchar(k == sp ? '|' : (k < cheio ? '#' : '.'));
    putchar(']');
}

/* ---------------------------------------------------------------------------
 * Simulação: o laço só "liga" as funções
 * ------------------------------------------------------------------------- */

#define CTRL_P      1
#define CTRL_ONOFF  2

/* Escolhe o controlador. Os dois têm a MESMA assinatura (mesmos parâmetros e
   retorno), então trocar um pelo outro não muda nada no resto do ciclo.
   (Passar a própria função como argumento também é possível, com ponteiro
   para função: ../03_revisao_sistematica_c/03_funcoes/.) O(1). */
static double controlador(int tipo, double erro, double estado_anterior) {
    if (tipo == CTRL_ONOFF) return controlador_onoff(erro, estado_anterior);
    return controlador_p(erro, estado_anterior);
}

/* Roda CICLOS ciclos e devolve quantas vezes a bomba mudou bruscamente
   (variação > 50 pontos), um indicador de desgaste do atuador. O(K). */
static int simular(const char *nome, int tipo, int detalhar) {
    double nivel = NIVEL_INICIAL;
    double filtrado = NIVEL_INICIAL;
    double u = 0.0;
    int trancos = 0;

    printf("\n  Controlador: %s   (SP = %.0f%%, nível inicial = %.0f%%)\n",
           nome, SETPOINT, NIVEL_INICIAL);
    if (detalhar)
        printf("  ciclo | sensor | filtrado |  erro  | ctrl   | saturado | nível\n");
    else
        printf("  ciclo | bomba | nível\n");

    for (int k = 0; k < CICLOS; k++) {
        double medida = ler_sensor(nivel, k);
        filtrado      = filtrar(medida, filtrado);
        double erro   = calcular_erro(SETPOINT, filtrado);
        double bruto  = controlador(tipo, erro, u);
        double novo_u = saturar(bruto, 0.0, 100.0);
        if (novo_u - u > 50.0 || u - novo_u > 50.0) trancos++;
        u = novo_u;

        if (detalhar)
            printf("  %5d | %6.1f | %8.1f | %6.1f | %6.1f | %7.1f%% | ",
                   k, medida, filtrado, erro, bruto, u);
        else
            printf("  %5d | %4.0f%% | ", k, u);
        barra_nivel(nivel);
        printf(" %5.1f%%\n", nivel);

        nivel = atualizar_planta(nivel, u);
        if (detalhar && k < 3) pausar();   /* os primeiros ciclos com calma */
    }
    printf("  Nível final: %.1f%% | mudanças bruscas da bomba: %d\n", nivel, trancos);
    pausar();
    return trancos;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("Modularização: rotina de controle de nível em funções\n");
    printf("================================================================\n");
    printf("  Um ciclo de controle = uma linha de chamadas:\n");
    printf("    medida   = ler_sensor(nivel, k);\n");
    printf("    filtrado = filtrar(medida, filtrado);\n");
    printf("    erro     = calcular_erro(SETPOINT, filtrado);\n");
    printf("    u        = saturar(controlador(tipo, erro, u), 0, 100);\n");
    printf("    nivel    = atualizar_planta(nivel, u);\n");
    pausar();

    int trancos_p = simular("proporcional (P)", CTRL_P, 1);
    printf("\n  Repare: o P estabiliza ABAIXO do setpoint (~52,5%%). É o erro em regime\n");
    printf("  do controlador P: para manter a bomba em 37,5%% (vazão = consumo) ele\n");
    printf("  precisa de erro = 37,5 / Kp = 7,5%%. Corrigir isso pede a ação integral (PI).\n");
    pausar();

    printf("\n  Agora trocamos SÓ o controlador. Sensor, filtro, saturação e planta\n");
    printf("  são as mesmas funções, sem nenhuma linha alterada:\n");
    int trancos_onoff = simular("liga/desliga com histerese", CTRL_ONOFF, 0);

    printf("\n  Comparação: P = %d mudanças bruscas, liga/desliga = %d.\n",
           trancos_p, trancos_onoff);
    printf("  O liga/desliga oscila em torno do SP e dá trancos na bomba; o P é suave\n");
    printf("  mas tem erro em regime. Testar essa troca levou uma linha de código\n");
    printf("  porque cada responsabilidade está em uma função separada.\n");
    return 0;
}
