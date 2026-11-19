/*
 * ============================================================================
 * Arquivo : 01_funcoes_passagem_valor.c
 * Módulo  : 02 — Revisão de funções e recursividade   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver, com "caixas de memória" desenhadas na tela, que em C os argumentos
 *   são passados POR VALOR: a função recebe uma cópia, e mudar a cópia não
 *   muda a variável de quem chamou. O resultado volta pelo 'return'.
 *   Também mostra funções de conversão reutilizáveis (ADC, 4–20 mA) testadas
 *   com uma tabela de casos, e a exceção dos vetores.
 *
 * EXEMPLO REAL
 *   Todo firmware de aquisição converte números crus em grandezas físicas:
 *   o ADC de 12 bits entrega 0..4095 e o programa precisa de volts; o
 *   transmissor de pressão entrega 4..20 mA e a IHM mostra bar. Escrever cada
 *   conversão como função evita copiar a fórmula em dez lugares (e errar em
 *   um deles).
 *
 * COMPLEXIDADE
 *   Operação                         Tempo    Espaço
 *   chamar uma função com k args     O(k)     O(k) (cópias na pilha)
 *   conversões ADC / 4–20 mA         O(1)     O(1)
 *   tabela de testes com t casos     O(t)     O(1)
 *   Por quê: cada argumento é copiado uma vez na chamada; as fórmulas têm
 *   número fixo de operações. Um vetor NÃO é copiado (ver Parte 3).
 *
 * COMO EXECUTAR
 *   ./run 01_funcoes_passagem_valor               (ENTER avança cada passo)
 *   ./run 01_funcoes_passagem_valor < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <math.h>
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

/* Desenha uma "caixa de memória" de uma variável de uma função. */
static void caixa(const char *funcao, const char *nome, int valor, const char *nota) {
    printf("    %-14s +-----------------------+\n", "");
    printf("    %-14s | %-10s = %8d | %s\n", funcao, nome, valor, nota);
    printf("    %-14s +-----------------------+\n", "");
}

/* ---------------------------------------------------------------------------
 * Parte 1: a cópia
 * ------------------------------------------------------------------------- */

/* Tenta zerar o setpoint. NÃO funciona para quem chamou: 'setpoint' aqui é
   uma variável local nova, inicializada com uma CÓPIA do argumento. */
static void tentar_zerar(int setpoint) {
    caixa("tentar_zerar", "setpoint", setpoint, "<- cópia recebida");
    setpoint = 0;
    caixa("tentar_zerar", "setpoint", setpoint, "<- só a cópia mudou");
    pausar();
}   /* aqui a cópia deixa de existir */

/* Jeito certo de "devolver" um valor: return.
   Limita o valor à faixa [minimo, maximo] (saturação). O(1). */
static int saturar(int valor, int minimo, int maximo) {
    if (valor < minimo) return minimo;
    if (valor > maximo) return maximo;
    return valor;
}

static void parte1_copia(void) {
    printf("PARTE 1 — Passagem por valor: a função recebe uma cópia\n");
    printf("================================================================\n");
    int setpoint = 75;
    printf("  main: int setpoint = 75;\n");
    caixa("main", "setpoint", setpoint, "");
    pausar();

    printf("  main chama tentar_zerar(setpoint):\n");
    tentar_zerar(setpoint);

    printf("  de volta à main:\n");
    caixa("main", "setpoint", setpoint, "<- continua 75!");
    printf("\n  Para mudar a variável de quem chama, devolva o novo valor com return:\n");
    printf("  main: setpoint = saturar(140, 0, 100);\n");
    setpoint = saturar(140, 0, 100);
    caixa("main", "setpoint", setpoint, "<- atribuído na main");
    printf("  (A outra forma, passar o endereço com ponteiro, está em\n");
    printf("   ../03_revisao_sistematica_c/03_funcoes/.)\n\n");
    pausar();
}

/* ---------------------------------------------------------------------------
 * Parte 2: funções puras e reutilizáveis + tabela de testes
 * ------------------------------------------------------------------------- */

#define ADC_MAX     4095      /* ADC de 12 bits: 2^12 - 1 */
#define VREF        3.3       /* tensão de referência do ADC, em volts */

/* Converte a contagem do ADC (0..4095) em volts. O(1).
   Pré-condição: 0 <= contagem <= ADC_MAX (saturamos para garantir). */
static double adc_para_volts(int contagem) {
    contagem = saturar(contagem, 0, ADC_MAX);   /* muda só a cópia local: seguro */
    return contagem * VREF / ADC_MAX;
}

/* Converte corrente de 4..20 mA na grandeza física da faixa [inf, sup].
   4 mA = inf, 20 mA = sup, reta entre os dois. O(1).
   Abaixo de 3,8 mA o laço está rompido: devolve NAN para sinalizar falha. */
static double ma_para_unidade(double ma, double inf, double sup) {
    if (ma < 3.8) return NAN;          /* fio partido / transmissor sem alimentação */
    return inf + (ma - 4.0) * (sup - inf) / 16.0;
}

static void parte2_reuso(void) {
    printf("PARTE 2 — Funções puras: mesma entrada, mesma saída\n");
    printf("================================================================\n");
    printf("  Uma função que só depende dos argumentos pode ser testada com uma\n");
    printf("  tabela de casos (é o que os programas 'testa_*' da lista_2 fazem).\n\n");

    const int casos_adc[] = {0, 1024, 2048, 4095, 5000, -10};
    printf("  adc_para_volts(contagem)          ADC 12 bits, Vref = %.1f V\n", VREF);
    printf("  %10s | %8s | %s\n", "contagem", "volts", "observação");
    for (int k = 0; k < 6; k++) {
        int c = casos_adc[k];
        printf("  %10d | %8.3f | %s\n", c, adc_para_volts(c),
               c > ADC_MAX || c < 0 ? "fora da faixa: saturado" : "");
    }
    pausar();

    const double casos_ma[] = {4.0, 8.0, 12.0, 20.0, 2.0};
    printf("\n  ma_para_unidade(mA, 0, 10)        transmissor de pressão 0..10 bar\n");
    printf("  %10s | %8s\n", "mA", "bar");
    for (int k = 0; k < 5; k++) {
        double p = ma_para_unidade(casos_ma[k], 0.0, 10.0);
        if (isnan(p)) printf("  %10.1f | %8s  <- laço rompido (< 3,8 mA)\n", casos_ma[k], "FALHA");
        else          printf("  %10.1f | %8.2f\n", casos_ma[k], p);
    }
    printf("\n  A mesma função serve para nível 0..5 m: ma_para_unidade(12, 0, 5) = %.2f m\n\n",
           ma_para_unidade(12.0, 0.0, 5.0));
    pausar();
}

/* ---------------------------------------------------------------------------
 * Parte 3: a exceção — vetores não são copiados
 * ------------------------------------------------------------------------- */

/* Zera as leituras inválidas (negativas) do vetor. Altera o vetor de QUEM
   CHAMOU, porque a função recebe a posição do vetor na memória, não uma
   cópia dos elementos. O(n). */
static void zera_negativos(int leituras[], int n) {
    for (int i = 0; i < n; i++)
        if (leituras[i] < 0) leituras[i] = 0;
}

static void parte3_vetores(void) {
    printf("PARTE 3 — Exceção: vetores NÃO são copiados\n");
    printf("================================================================\n");
    int leituras[6] = {12, -3, 15, -1, 14, 13};
    printf("  main, antes : ");
    for (int i = 0; i < 6; i++) printf("%4d", leituras[i]);
    zera_negativos(leituras, 6);
    printf("\n  main, depois: ");
    for (int i = 0; i < 6; i++) printf("%4d", leituras[i]);
    printf("\n\n  Copiar um vetor de 1 milhão de elementos a cada chamada custaria O(n);\n");
    printf("  por isso C passa só onde o vetor começa (O(1)). Consequência: a função\n");
    printf("  pode alterar o vetor original. Use 'const int v[]' quando ela só lê.\n");
    printf("  É por isso que inverte_vetor (lista_2) inverte o próprio vetor da main.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("Funções: passagem por valor e reaproveitamento\n\n");
    parte1_copia();
    parte2_reuso();
    parte3_vetores();
    return 0;
}
