/*
 * ============================================================================
 * Arquivo : 03_recursao_partida_esteiras.c
 * Módulo  : 02 — Revisão de funções e recursividade   (Ementa: Unidade 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Primeiro contato com recursão: ver a pilha de chamadas crescer ("ida")
 *   e esvaziar ("volta"), com o trabalho feito ANTES da chamada recursiva
 *   acontecendo em uma ordem e o trabalho feito DEPOIS acontecendo na ordem
 *   inversa. Compara com a versão iterativa (dois laços).
 *
 * EXEMPLO REAL
 *   Partida de uma linha de esteiras transportadoras E1 -> E2 -> ... -> En
 *   (minério, grãos, bagagem). Regra de intertravamento:
 *     - verifica cada esteira na ordem do fluxo (E1, E2, ..., En);
 *     - liga de trás para frente (En primeiro, E1 por último), para que
 *       nenhuma esteira despeje material em outra que ainda está parada;
 *     - se alguma verificação falhar, NENHUMA esteira é ligada.
 *   A recursão faz isso naturalmente: verificar na ida, ligar na volta.
 *
 * COMPLEXIDADE (n = número de esteiras)
 *   Versão        Tempo   Espaço extra
 *   recursiva     O(n)    O(n)  (n quadros na pilha de chamadas)
 *   iterativa     O(n)    O(1)
 *   Por quê: cada esteira gera exatamente uma chamada (ou uma volta de laço);
 *   na recursiva, as chamadas ficam empilhadas esperando até o caso base.
 *
 * COMO EXECUTAR
 *   ./run 03_recursao_partida_esteiras               (ENTER avança cada passo)
 *   ./run 03_recursao_partida_esteiras < /dev/null   (executa direto, sem pausas)
 *   Mais recursão (fatorial, Fibonacci, busca binária, profundidade):
 *   ../03_revisao_sistematica_c/04_recursao/
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

#define N_ESTEIRAS 4

/* Estado da linha: 1 = esteira pronta para partir (sem falha), 0 = falha. */
static int pronta[N_ESTEIRAS + 1];   /* índice 1..N; a posição 0 não é usada */
static int ligada[N_ESTEIRAS + 1];
static int chamadas = 0;             /* quantas vezes a função foi chamada */

/* Desenha a pilha de chamadas: do topo (chamada mais recente) até a main. */
static void desenha_pilha(int topo) {
    printf("      pilha de chamadas:\n");
    for (int i = topo; i >= 1; i--)
        printf("      | partir(%d)%s |%s\n", i, i > N_ESTEIRAS ? " base" : "     ",
               i == topo ? "  <- topo" : "");
    printf("      | main           |\n");
    printf("      +----------------+\n");
}

/* Mostra quais esteiras estão ligadas, no sentido do fluxo. */
static void desenha_linha(void) {
    printf("      fluxo -> ");
    for (int i = 1; i <= N_ESTEIRAS; i++)
        printf("[E%d %s]%s", i, ligada[i] ? "ON " : "off", i < N_ESTEIRAS ? "==>" : "\n");
}

/* Parte a sequência da esteira i até a última.
   Devolve 1 se E_i..E_n foram ligadas, 0 se alguma falhou (e então nenhuma
   delas é ligada).
   - Caso base: i > n -> não há mais esteiras, tudo certo.
   - Passo: verifica E_i (IDA), resolve o resto da linha recursivamente e,
     só se o resto deu certo, liga E_i (VOLTA).
   Cada chamada fica parada na linha da chamada recursiva até ela voltar:
   é isso que inverte a ordem. O(n) tempo, O(n) pilha. */
static int partir(int i, int mostrar) {
    chamadas++;
    if (mostrar) {
        desenha_pilha(i);
    }
    if (i > N_ESTEIRAS) {                         /* caso base: sem esse if, */
        if (mostrar) {                            /* a recursão nunca para   */
            printf("      caso base: não há E%d -> devolve 1 (começa a VOLTA)\n\n", i);
            pausar();
        }
        return 1;
    }

    /* IDA: trabalho feito antes da chamada recursiva. */
    if (mostrar) {
        printf("      IDA   E%d: verifica intertravamento -> %s\n\n", i,
               pronta[i] ? "ok, desce para a próxima" : "FALHA, devolve 0");
        pausar();
    }
    if (!pronta[i]) return 0;                     /* não desce mais */

    int resto_ok = partir(i + 1, mostrar);        /* fica esperando aqui */

    /* VOLTA: trabalho feito depois que a chamada recursiva retornou. */
    if (resto_ok) ligada[i] = 1;
    if (mostrar) {
        desenha_pilha(i);
        if (resto_ok)
            printf("      VOLTA E%d: resto da linha ok -> liga E%d\n", i, i);
        else
            printf("      VOLTA E%d: resto da linha falhou -> E%d fica desligada\n", i, i);
        desenha_linha();
        printf("\n");
        pausar();
    }
    return resto_ok;
}

/* Mesma lógica sem recursão: um laço de verificação e um laço de partida em
   ordem inversa. O(n) tempo, O(1) espaço: não empilha nada. */
static int partir_iterativo(void) {
    for (int i = 1; i <= N_ESTEIRAS; i++)
        if (!pronta[i]) return 0;
    for (int i = N_ESTEIRAS; i >= 1; i--)
        ligada[i] = 1;
    return 1;
}

static void prepara(int esteira_com_falha) {
    for (int i = 1; i <= N_ESTEIRAS; i++) {
        pronta[i] = (i != esteira_com_falha);
        ligada[i] = 0;
    }
    chamadas = 0;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("Recursão: partida em sequência de esteiras transportadoras\n");
    printf("================================================================\n\n");

    printf("CENÁRIO 1 — todas as %d esteiras prontas\n\n", N_ESTEIRAS);
    prepara(0);
    desenha_linha();
    printf("\n");
    int ok = partir(1, 1);
    printf("  Resultado: %s | chamadas = %d (n + 1, contando o caso base)\n",
           ok ? "linha em operação" : "partida abortada", chamadas);
    printf("  Ordem: verificou E1..E%d na IDA e ligou E%d..E1 na VOLTA.\n\n",
           N_ESTEIRAS, N_ESTEIRAS);
    pausar();

    printf("CENÁRIO 2 — falha na esteira E3 (sensor de desalinhamento atuado)\n\n");
    prepara(3);
    ok = partir(1, 1);
    printf("  Resultado: %s | chamadas = %d\n", ok ? "linha em operação" : "partida abortada",
           chamadas);
    printf("  E1 e E2 foram verificadas, mas na VOLTA receberam 0 e não ligaram:\n");
    desenha_linha();
    printf("\n");
    pausar();

    printf("CENÁRIO 3 — versão iterativa (dois laços), mesma lógica\n\n");
    prepara(0);
    ok = partir_iterativo();
    desenha_linha();
    printf("  Resultado: %s\n\n", ok ? "linha em operação" : "partida abortada");

    printf("Comparação (n = número de esteiras):\n");
    printf("  %-10s | %-6s | %s\n", "versão", "tempo", "memória extra");
    printf("  %-10s | %-6s | %s\n", "recursiva", "O(n)", "O(n): um quadro na pilha por esteira");
    printf("  %-10s | %-6s | %s\n", "iterativa", "O(n)", "O(1)");
    printf("\nQuando usar recursão: quando o problema se define em termos de uma\n");
    printf("versão menor dele mesmo (árvores, divisão e conquista). Para linhas\n");
    printf("simples como esta, o laço é igualmente claro e não gasta pilha.\n");
    printf("Armadilha: esquecer o caso base -> chamadas infinitas -> estouro de pilha.\n");
    return 0;
}
