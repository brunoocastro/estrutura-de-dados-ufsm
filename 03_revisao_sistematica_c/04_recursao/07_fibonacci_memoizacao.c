/*
 * ============================================================================
 * Arquivo : 07_fibonacci_memoizacao.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Comparar três formas de calcular Fibonacci e MEDIR a diferença:
 *     1. recursão ingênua      O(2^n)  -> recalcula os mesmos F(k) sem parar;
 *     2. recursão + memória    O(n)    -> guarda cada F(k) numa tabela (memo);
 *     3. laço iterativo        O(n)    -> só duas variáveis, sem pilha extra.
 *   Primeiro o aluno vê os rastros (com a pilha indentada) para n pequeno;
 *   depois, uma tabela com o número de chamadas para n = 10, 20, 30, 40.
 *
 * EXEMPLO REAL
 *   Memoização é usada em planejamento de trajetórias de robôs e AGVs
 *   (programação dinâmica), em cálculo de rotas e em qualquer recursão cujos
 *   subproblemas se repetem. Guardar resultados troca memória por tempo.
 *
 * COMPLEXIDADE (n = índice)
 *   Versão        Tempo     Memória extra        Chamadas/iterações para n = 40
 *   ingênua       O(2^n)    O(n) pilha           331 160 281 chamadas
 *   memoização    O(n)      O(n) tabela + pilha  79 chamadas
 *   iterativa     O(n)      O(1)                 39 iterações
 *   Por quê: a ingênua resolve F(n-2) duas vezes, F(n-3) três vezes, F(n-4)
 *   cinco vezes... (os próprios números de Fibonacci!). Chamadas = 2·F(n+1) − 1.
 *
 * COMO EXECUTAR
 *   ./run 07_fibonacci_memoizacao               (ENTER avança cada passo)
 *   ./run 07_fibonacci_memoizacao < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define N_MAX 90                  /* F(90) ainda cabe em unsigned long long */

typedef unsigned long long u64;

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static u64 chamadas = 0;          /* contador de chamadas da versão em teste */
static int mostrar = 0;           /* 1 = imprime o rastro */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* 1) O(2^n) tempo, O(n) pilha. */
static u64 fib_ingenuo(unsigned n, int nivel) {
    chamadas++;
    if (mostrar) printf("%*sF(%u)\n", nivel * 3, "", n);
    if (n <= 1) return n;
    return fib_ingenuo(n - 1, nivel + 1) + fib_ingenuo(n - 2, nivel + 1);
}

/* 2) O(n) tempo e memória. memo[k] == 0 significa "ainda não calculado"
 * (para k >= 2 nenhum F(k) vale 0, então o 0 serve de marcador).
 * Cada F(k) é calculado UMA vez; as outras chamadas só consultam a tabela. */
static u64 memo[N_MAX + 1];

static u64 fib_memo(unsigned n, int nivel) {
    chamadas++;
    if (n <= 1) {
        if (mostrar) printf("%*sF(%u) = %u  caso base\n", nivel * 3, "", n, n);
        return n;
    }
    if (memo[n] != 0) {
        if (mostrar)
            printf("%*sF(%u) = %llu  ja esta na tabela: retorna SEM descer\n",
                   nivel * 3, "", n, memo[n]);
        return memo[n];
    }
    if (mostrar) printf("%*sF(%u) nao esta na tabela: calcula\n", nivel * 3, "", n);
    memo[n] = fib_memo(n - 1, nivel + 1) + fib_memo(n - 2, nivel + 1);
    if (mostrar) printf("%*sF(%u) = %llu  guardado em memo[%u]\n", nivel * 3, "", n, memo[n], n);
    return memo[n];
}

static void limpa_memo(void) {
    for (unsigned i = 0; i <= N_MAX; i++) memo[i] = 0;
}

/* 3) O(n) tempo, O(1) memória: desliza a "janela" (anterior, atual). */
static u64 fib_iterativo(unsigned n, u64 *iteracoes) {
    u64 anterior = 0, atual = 1;
    *iteracoes = 0;
    if (n == 0) return 0;
    for (unsigned i = 2; i <= n; i++) {
        u64 proximo = anterior + atual;
        if (mostrar)
            printf("   i = %u: F(%u) = F(%u) + F(%u) = %llu + %llu = %llu   (janela: %llu, %llu)\n",
                   i, i, i - 2, i - 1, anterior, atual, proximo, atual, proximo);
        anterior = atual;
        atual = proximo;
        (*iteracoes)++;
    }
    return atual;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    const unsigned n_demo = 6;

    printf("=== 1) Recursao ingenua: arvore de chamadas de F(%u) ===\n", n_demo);
    printf("(indentacao = profundidade na pilha; note F(2), F(1) e F(0) se repetindo)\n");
    chamadas = 0;
    mostrar = 1;
    u64 r = fib_ingenuo(n_demo, 0);
    printf("F(%u) = %llu com %llu chamadas\n", n_demo, r, chamadas);
    pausar();

    printf("\n=== 2) Recursao com memoizacao: F(%u) ===\n", n_demo);
    limpa_memo();
    chamadas = 0;
    r = fib_memo(n_demo, 0);
    printf("F(%u) = %llu com %llu chamadas (cada F(k) calculado uma unica vez)\n",
           n_demo, r, chamadas);
    pausar();

    printf("\n=== 3) Laco iterativo: F(%u) ===\n", n_demo);
    u64 iteracoes;
    r = fib_iterativo(n_demo, &iteracoes);
    printf("F(%u) = %llu com %llu iteracoes e so 2 variaveis\n", n_demo, r, iteracoes);
    mostrar = 0;
    pausar();

    printf("\n=== 4) Crescimento: chamadas/iteracoes executadas de verdade ===\n");
    printf("%4s | %12s | %15s | %10s | %10s | %9s\n",
           "n", "F(n)", "ingenua", "memo", "iterativa", "tempo ing.");
    printf("-----+--------------+-----------------+------------+------------+----------\n");
    unsigned ns[] = {10, 20, 30, 40};
    for (size_t k = 0; k < sizeof ns / sizeof ns[0]; k++) {
        unsigned n = ns[k];
        chamadas = 0;
        clock_t t0 = clock();
        u64 f1 = fib_ingenuo(n, 0);
        double seg = (double)(clock() - t0) / CLOCKS_PER_SEC;
        u64 c_ing = chamadas;

        limpa_memo();
        chamadas = 0;
        u64 f2 = fib_memo(n, 0);
        u64 c_memo = chamadas;

        u64 f3 = fib_iterativo(n, &iteracoes);
        printf("%4u | %12llu | %15llu | %10llu | %10llu | %8.3fs%s\n",
               n, f1, c_ing, c_memo, iteracoes, seg,
               (f1 == f2 && f2 == f3) ? "" : "  (DIVERGIU!)");
    }
    printf("\nCada +10 no n multiplica as chamadas ingenuas por ~123 (= 1,618^10);\n");
    printf("memo e laco crescem so +20 e +10. Em n = 50, a ingenua faria ~40 bilhoes\n");
    printf("de chamadas (minutos); a memoizada, 99.\n");

    printf("\nMemo com n = %d: ", N_MAX);
    limpa_memo();
    chamadas = 0;
    printf("F(%d) = %llu em %llu chamadas.\n", N_MAX, fib_memo(N_MAX, 0), chamadas);
    return 0;
}
