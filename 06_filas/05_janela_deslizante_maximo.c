/*
 * ============================================================================
 * Arquivo : 05_janela_deslizante_maximo.c
 * Módulo  : 06 — Filas                        (Ementa: Unidade 3.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Calcular o MÁXIMO de cada janela de k leituras consecutivas de um sensor
 *   de pressão de duas formas e comparar o custo:
 *     - ingênua: para cada janela, percorre as k leituras -> O(n·k);
 *     - deque monotônico: um deque de ÍNDICES cujos valores ficam em ordem
 *       decrescente da frente para trás. A frente é sempre o máximo da janela.
 *       Cada índice entra uma vez e sai no máximo uma vez -> O(n) no total.
 *   Cada passo desenha as leituras com a janela marcada e o conteúdo do deque,
 *   narrando quem sai pela frente (expirou) e quem sai por trás (foi dominado).
 *
 * EXEMPLO REAL
 *   Supervisório que dispara alarme se o PICO de pressão do último segundo
 *   (k amostras) passar do limite; indicação "max-hold" em painéis e IHMs;
 *   proteção de sobrecorrente por janela; detecção de envelope de vibração.
 *   Trocando "<=" por ">=" o mesmo algoritmo dá o MÍNIMO da janela.
 *
 * COMPLEXIDADE (n = leituras, k = tamanho da janela)
 *   Método            Tempo     Espaço extra
 *   ingênuo           O(n·k)    O(1)
 *   deque monotônico  O(n)      O(k)   (o deque nunca passa de k índices)
 *   Por quê: no ingênuo, cada uma das n-k+1 janelas faz k-1 comparações. No
 *   deque, embora haja um `while` dentro do `for`, cada índice é removido no
 *   máximo uma vez ao longo de TODA a execução: no total são <= 2n operações
 *   de deque (análise amortizada).
 *
 * COMO EXECUTAR
 *   ./run 05_janela_deslizante_maximo               (ENTER avança cada passo)
 *   ./run 05_janela_deslizante_maximo < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int modo_interativo = 0; /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ------------------------------------------------------------------------- */
/* Deque circular de índices (mesma ideia de 04_deque_circular)               */
/* ------------------------------------------------------------------------- */

typedef struct {
    int *idx; /* guarda ÍNDICES das leituras, não os valores: assim sabemos
                 quando um elemento saiu da janela (índice <= i - k) */
    int cap, ini, tam;
} DequeIdx;

static int dq_criar(DequeIdx *d, int cap) {
    d->idx = malloc((size_t)cap * sizeof *d->idx);
    d->cap = cap;
    d->ini = d->tam = 0;
    return d->idx != NULL;
}
/* Todas O(1). Pré-condição (garantida pelo algoritmo): nunca insere cheio
   nem remove vazio. */
static int dq_frente(const DequeIdx *d) { return d->idx[d->ini]; }
static int dq_tras(const DequeIdx *d) { return d->idx[(d->ini + d->tam - 1) % d->cap]; }
static void dq_remover_frente(DequeIdx *d) { d->ini = (d->ini + 1) % d->cap; d->tam--; }
static void dq_remover_tras(DequeIdx *d) { d->tam--; }
static void dq_inserir_tras(DequeIdx *d, int i) {
    d->idx[(d->ini + d->tam) % d->cap] = i;
    d->tam++;
}

/* ------------------------------------------------------------------------- */
/* Os dois algoritmos                                                         */
/* ------------------------------------------------------------------------- */

/* O(n·k): para cada janela [i-k+1 .. i], procura o maior. */
static void maximo_ingenuo(const int *x, int n, int k, int *saida, long *comparacoes) {
    *comparacoes = 0;
    for (int i = k - 1; i < n; i++) {
        int m = x[i - k + 1];
        for (int j = i - k + 2; j <= i; j++) {
            (*comparacoes)++;
            if (x[j] > m) m = x[j];
        }
        saida[i - k + 1] = m;
    }
}

/* Desenha as leituras, a janela atual (colchetes) e o deque. O(n + k). */
static void desenhar(const int *x, int n, int k, int i, const DequeIdx *d) {
    printf("      i   : ");
    for (int j = 0; j < n; j++) printf("%4d", j);
    printf("\n      x[i]: ");
    for (int j = 0; j < n; j++) printf("%4d", x[j]);
    printf("\n      jan.: ");
    int ini_jan = i - k + 1 < 0 ? 0 : i - k + 1;
    for (int j = 0; j < n; j++) {
        if (j == ini_jan && j == i) printf("  []");
        else if (j == ini_jan) printf("  [-");
        else if (j == i) printf("---]");
        else if (j > ini_jan && j < i) printf("----");
        else printf("    ");
    }
    printf("\n      deque (frente -> trás): ");
    for (int t = 0; t < d->tam; t++) {
        int id = d->idx[(d->ini + t) % d->cap];
        printf("[i=%d:%d] ", id, x[id]);
    }
    if (d->tam == 0) printf("(vazio)");
    printf("\n");
}

/*
 * O(n) amortizado. Para cada nova leitura x[i]:
 *   1) remove da FRENTE o índice que saiu da janela (<= i - k);
 *   2) remove de TRÁS todos com valor <= x[i]: eles nunca mais serão máximo,
 *      porque x[i] é maior (ou igual) e vai durar mais na janela;
 *   3) insere i atrás;
 *   4) se a janela já tem k leituras, o máximo é x[frente].
 * `operacoes` conta inserções + remoções no deque (<= 2n).
 */
static int maximo_deque(const int *x, int n, int k, int *saida, long *operacoes, int narrar) {
    DequeIdx d;
    if (!dq_criar(&d, k)) return 0;
    *operacoes = 0;
    for (int i = 0; i < n; i++) {
        if (narrar) printf("\n  passo i=%d: chega a leitura x[%d] = %d\n", i, i, x[i]);
        if (d.tam > 0 && dq_frente(&d) <= i - k) {
            if (narrar) printf("    frente i=%d saiu da janela (%d <= %d - %d) -> remover_frente\n",
                               dq_frente(&d), dq_frente(&d), i, k);
            dq_remover_frente(&d);
            (*operacoes)++;
        }
        while (d.tam > 0 && x[dq_tras(&d)] <= x[i]) {
            if (narrar) printf("    trás i=%d (valor %d) <= %d: nunca mais será máximo -> remover_tras\n",
                               dq_tras(&d), x[dq_tras(&d)], x[i]);
            dq_remover_tras(&d);
            (*operacoes)++;
        }
        dq_inserir_tras(&d, i);
        (*operacoes)++;
        if (narrar) printf("    inserir_tras(i=%d)\n", i);
        if (i >= k - 1) {
            saida[i - k + 1] = x[dq_frente(&d)];
            if (narrar) printf("    janela [%d..%d] completa: máximo = x[frente] = %d   (O(1))\n",
                               i - k + 1, i, saida[i - k + 1]);
        }
        if (narrar) {
            desenhar(x, n, k, i, &d);
            pausar();
        }
    }
    free(d.idx);
    return 1;
}

/* Gerador pseudoaleatório determinístico (LCG): mesma saída em qualquer máquina. */
static unsigned semente = 12345u;
static int leitura_sensor(void) {
    semente = semente * 1103515245u + 12345u;
    return 40 + (int)((semente >> 16) % 61u); /* pressão entre 40 e 100 kPa */
}

static void tabela_crescimento(void) {
    printf("\n=== TABELA: comparações (ingênuo) x operações de deque ===\n\n");
    /* Cabeçalho literal: printf conta bytes, e "ê"/"·" ocupam 2 bytes em UTF-8. */
    printf("         n     k |   ingênuo O(n·k) |     deque O(n) | resultados\n");
    printf("  ---------------+------------------+----------------+-----------\n");
    const int ns[] = {100, 1000, 10000, 100000};
    const int ks[] = {10, 100};
    for (int a = 0; a < 4; a++) {
        for (int b = 0; b < 2; b++) {
            int n = ns[a], k = ks[b];
            if (k > n) continue;
            int *x = malloc((size_t)n * sizeof *x);
            int *s1 = malloc((size_t)(n - k + 1) * sizeof *s1);
            int *s2 = malloc((size_t)(n - k + 1) * sizeof *s2);
            if (x == NULL || s1 == NULL || s2 == NULL) {
                free(x); free(s1); free(s2);
                printf("  sem memória\n");
                return;
            }
            semente = 777u;
            for (int i = 0; i < n; i++) x[i] = leitura_sensor();
            long c1 = 0, c2 = 0;
            maximo_ingenuo(x, n, k, s1, &c1);
            int ok = maximo_deque(x, n, k, s2, &c2, 0);
            for (int i = 0; ok && i <= n - k; i++) ok = s1[i] == s2[i];
            printf("  %8d %5d | %16ld | %14ld | %s\n", n, k, c1, c2, ok ? "iguais" : "DIFERENTES!");
            free(x); free(s1); free(s2);
        }
    }
    printf("\n  O ingênuo cresce com n·k; o deque fica perto de 2n, independentemente de k.\n"
           "  (com n = k há uma única janela e o ingênuo ganha: a vantagem do deque aparece\n"
           "  quando há muitas janelas, que é o caso de um sensor lido continuamente.)\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    enum { N = 14, K = 4 };
    int x[N];
    for (int i = 0; i < N; i++) x[i] = leitura_sensor();

    printf("MÁXIMO EM JANELA DESLIZANTE: pico de pressão (kPa) nas últimas %d leituras\n", K);
    printf("Invariante do deque: índices crescentes e VALORES DECRESCENTES da frente para trás.\n");
    pausar();

    int s_deque[N - K + 1], s_ingenuo[N - K + 1];
    long ops = 0, comps = 0;
    if (!maximo_deque(x, N, K, s_deque, &ops, 1)) { printf("sem memória\n"); return 0; }
    maximo_ingenuo(x, N, K, s_ingenuo, &comps);

    printf("\n  máximos por janela (deque)  :");
    for (int i = 0; i <= N - K; i++) printf(" %d", s_deque[i]);
    printf("\n  máximos por janela (ingênuo):");
    for (int i = 0; i <= N - K; i++) printf(" %d", s_ingenuo[i]);
    printf("\n  custo: deque = %ld operações (<= 2n = %d); ingênuo = %ld comparações"
           " ((n-k+1)(k-1) = %d)\n", ops, 2 * N, comps, (N - K + 1) * (K - 1));
    pausar();

    tabela_crescimento();
    return 0;
}
