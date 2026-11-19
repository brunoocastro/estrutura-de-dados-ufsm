/*
 * ============================================================================
 * Arquivo : 06_benchmark_ordenacao.c
 * Módulo  : 08 — Ordenação e Busca            (Ementa: Unidade 4.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Colocar todos os algoritmos do módulo lado a lado — bolha, seleção,
 *   inserção, merge sort, quicksort (pivô ingênuo e mediana-de-três) e o
 *   qsort da biblioteca padrão — e medir comparações, movimentações e tempo
 *   para n = 100, 1000 e 10000 em entradas ALEATÓRIA (semente fixa),
 *   ORDENADA e INVERTIDA. As contagens são determinísticas (iguais em
 *   qualquer máquina); o tempo via clock() é só ilustrativo.
 *
 * EXEMPLO REAL
 *   Escolher o algoritmo é decisão de engenharia: um supervisório que ordena
 *   10 mil leituras por segundo não pode usar um método O(n²) (≈ 50 milhões
 *   de comparações), mas para 8 leituras de um filtro a inserção é a mais
 *   rápida. Dados que chegam quase ordenados (timestamps) favorecem inserção.
 *
 * COMPLEXIDADE (n = número de elementos)
 *   Algoritmo           Melhor      Médio       Pior        Espaço     Estável
 *   bolha (c/ parada)   O(n)        O(n²)       O(n²)       O(1)       sim
 *   seleção             O(n²)       O(n²)       O(n²)       O(1)       não
 *   inserção            O(n)        O(n²)       O(n²)       O(1)       sim
 *   merge sort          O(n log n)  O(n log n)  O(n log n)  O(n)       sim
 *   quick (último)      O(n log n)  O(n log n)  O(n²)       O(log n)*  não
 *   quick (mediana-3)   O(n log n)  O(n log n)  O(n²)**     O(log n)   não
 *   qsort (stdlib)      depende da implementação; não é garantidamente estável
 *   Por quê: veja os arquivos 03, 04 e 05. *O(n) de pilha no pior caso.
 *   **O pior caso ainda existe, mas exige entradas construídas de propósito.
 *
 * COMO EXECUTAR
 *   ./run 06_benchmark_ordenacao               (ENTER avança cada tabela)
 *   ./run 06_benchmark_ordenacao < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próxima tabela...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Contadores globais. "Movimentações" = escritas de elementos no vetor:
 * uma troca conta 3 (t = a; a = b; b = t), um deslocamento ou cópia conta 1.
 * Assim os algoritmos ficam comparáveis entre si. */
static long comp, mov;

static void troca(int v[], int a, int b) {
    int t = v[a]; v[a] = v[b]; v[b] = t;
    mov += 3;
}

/* ---- O(n²) ---------------------------------------------------------------- */

static void bolha(int v[], int n) {
    for (int fim = n - 1; fim > 0; fim--) {
        int trocou = 0;
        for (int j = 0; j < fim; j++) {
            comp++;
            if (v[j] > v[j + 1]) { troca(v, j, j + 1); trocou = 1; }
        }
        if (!trocou) break;                   /* parada antecipada */
    }
}

static void selecao(int v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int m = i;
        for (int j = i + 1; j < n; j++) { comp++; if (v[j] < v[m]) m = j; }
        if (m != i) troca(v, i, m);
    }
}

static void insercao(int v[], int n) {
    for (int i = 1; i < n; i++) {
        int x = v[i], j = i - 1;
        while (j >= 0 && (comp++, v[j] > x)) { v[j + 1] = v[j]; mov++; j--; }
        v[j + 1] = x;
        mov++;
    }
}

/* ---- O(n log n) ----------------------------------------------------------- */

static void merge_rec(int v[], int aux[], int lo, int hi) {
    if (lo >= hi) return;
    int meio = lo + (hi - lo) / 2;
    merge_rec(v, aux, lo, meio);
    merge_rec(v, aux, meio + 1, hi);
    int i = lo, j = meio + 1, k = lo;
    while (i <= meio && j <= hi) { comp++; aux[k++] = v[i] <= v[j] ? v[i++] : v[j++]; mov++; }
    while (i <= meio) { aux[k++] = v[i++]; mov++; }
    while (j <= hi)   { aux[k++] = v[j++]; mov++; }
    for (k = lo; k <= hi; k++) { v[k] = aux[k]; mov++; }
}

static void merge_sort(int v[], int n) {
    int *aux = malloc((size_t)n * sizeof *aux);
    if (aux == NULL) { fprintf(stderr, "sem memória para o merge sort\n"); return; }
    merge_rec(v, aux, 0, n - 1);
    free(aux);
}

/* Partição de Lomuto com pivô em v[hi]. */
static int lomuto(int v[], int lo, int hi) {
    int p = v[hi], i = lo - 1;
    for (int j = lo; j < hi; j++) { comp++; if (v[j] <= p) { i++; if (i != j) troca(v, i, j); } }
    if (i + 1 != hi) troca(v, i + 1, hi);
    return i + 1;
}

static void quick_ingenuo_rec(int v[], int lo, int hi) {
    if (lo >= hi) return;
    int p = lomuto(v, lo, hi);
    quick_ingenuo_rec(v, lo, p - 1);
    quick_ingenuo_rec(v, p + 1, hi);
}
static void quick_ingenuo(int v[], int n) { quick_ingenuo_rec(v, 0, n - 1); }

/* Mediana-de-três + recursão no lado MENOR primeiro e laço no maior: isso
 * limita a pilha a O(log n) mesmo no pior caso (técnica de Sedgewick). */
static void quick_m3_rec(int v[], int lo, int hi) {
    while (lo < hi) {
        if (hi - lo >= 2) {
            int m = lo + (hi - lo) / 2;
            comp += 3;
            if (v[m] < v[lo]) troca(v, m, lo);
            if (v[hi] < v[lo]) troca(v, hi, lo);
            if (v[hi] < v[m]) troca(v, hi, m);
            troca(v, m, hi);                  /* mediana vira o pivô em v[hi] */
        }
        int p = lomuto(v, lo, hi);
        if (p - lo < hi - p) { quick_m3_rec(v, lo, p - 1); lo = p + 1; }
        else                 { quick_m3_rec(v, p + 1, hi); hi = p - 1; }
    }
}
static void quick_m3(int v[], int n) { quick_m3_rec(v, 0, n - 1); }

/* qsort só nos deixa contar comparações (pelo comparador); as movimentações
 * acontecem dentro da biblioteca e não são visíveis. */
static int cmp_int(const void *a, const void *b) {
    comp++;
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);                 /* sem x - y: evita overflow */
}
static void qsort_stdlib(int v[], int n) { qsort(v, (size_t)n, sizeof v[0], cmp_int); }

/* ------------------------------------------------------------------------- */

typedef struct {
    const char *nome;
    void (*ordena)(int[], int);
    int mov_visivel;
} Algoritmo;

static const Algoritmo algs[] = {
    {"bolha (c/ parada)", bolha, 1},     {"seleção", selecao, 1},
    {"inserção", insercao, 1},           {"merge sort", merge_sort, 1},
    {"quick (último)", quick_ingenuo, 1}, {"quick (mediana-3)", quick_m3, 1},
    {"qsort (stdlib)", qsort_stdlib, 0},
};
#define N_ALG ((int)(sizeof algs / sizeof algs[0]))
#define N_TAM 3
#define N_ENT 3

static const int tamanhos[N_TAM] = {100, 1000, 10000};
static const char *entradas[N_ENT] = {"ALEATÓRIA", "ORDENADA", "INVERTIDA"};

typedef struct { long comp, mov; double ms; int ok; } Resultado;
static Resultado res[N_ALG][N_ENT][N_TAM];

/* Gerador próprio (LCG) com semente fixa: a mesma entrada "aleatória" em
 * qualquer sistema, o que torna as contagens reprodutíveis. */
static unsigned semente;
static int aleatorio(void) {
    semente = semente * 1103515245u + 12345u;
    return (int)((semente >> 16) & 0x7FFF) * 32768 + (int)((semente >> 1) & 0x7FFF);
}

static void gera(int v[], int n, int tipo) {
    semente = 20261119u;                      /* mesma semente para todos os algoritmos */
    for (int i = 0; i < n; i++)
        v[i] = tipo == 0 ? aleatorio() % 1000000 : tipo == 1 ? i : n - i;
}

static int ordenado(const int v[], int n) {
    for (int i = 1; i < n; i++) if (v[i - 1] > v[i]) return 0;
    return 1;
}

/* Roda tudo e guarda em res. Custo dominado pelos O(n²) com n = 10000. */
static void executa(void) {
    int *base = malloc(10000 * sizeof *base), *v = malloc(10000 * sizeof *v);
    if (base == NULL || v == NULL) { free(base); free(v); fprintf(stderr, "sem memória\n"); exit(1); }
    for (int t = 0; t < N_TAM; t++)
        for (int e = 0; e < N_ENT; e++) {
            int n = tamanhos[t];
            gera(base, n, e);
            for (int a = 0; a < N_ALG; a++) {
                memcpy(v, base, (size_t)n * sizeof *v);
                comp = mov = 0;
                clock_t c0 = clock();
                algs[a].ordena(v, n);
                clock_t c1 = clock();
                res[a][e][t] = (Resultado){comp, mov, 1000.0 * (double)(c1 - c0) / CLOCKS_PER_SEC,
                                           ordenado(v, n)};
            }
        }
    free(base);
    free(v);
}

/* printf("%-18s") conta BYTES, e em UTF-8 "ç"/"ã" ocupam 2 bytes: a coluna
 * desalinharia. Aqui contamos só os bytes que iniciam um caractere. */
static void coluna(const char *s, int largura) {
    int visiveis = 0;
    for (const char *p = s; *p; p++)
        if (((unsigned char)*p & 0xC0) != 0x80) visiveis++;
    printf("   %s%*s |", s, largura > visiveis ? largura - visiveis : 0, "");
}

static void tabela_contagens(int e) {
    printf("=== Entrada %s ===\n", entradas[e]);
    printf("                      |           comparações            |         movimentações\n");
    printf("   %-18s | %10s %10s %10s | %10s %10s %10s\n", "algoritmo", "n=100", "n=1000", "n=10000", "n=100",
           "n=1000", "n=10000");
    printf("   -------------------+----------------------------------+---------------------------------\n");
    for (int a = 0; a < N_ALG; a++) {
        coluna(algs[a].nome, 18);
        for (int t = 0; t < N_TAM; t++) printf(" %10ld", res[a][e][t].comp);
        printf(" |");
        for (int t = 0; t < N_TAM; t++) {
            if (algs[a].mov_visivel) printf(" %10ld", res[a][e][t].mov);
            else                     printf(" %10s", "n/d");
        }
        for (int t = 0; t < N_TAM; t++) if (!res[a][e][t].ok) printf("  ERRO: não ordenou n=%d", tamanhos[t]);
        printf("\n");
    }
    coluna("ref. n²/2", 18);
    for (int t = 0; t < N_TAM; t++) printf(" %10.0f", tamanhos[t] * (double)tamanhos[t] / 2);
    printf(" |\n   %-18s |", "ref. n log2 n");
    for (int t = 0; t < N_TAM; t++) printf(" %10.0f", tamanhos[t] * log2(tamanhos[t]));
    printf(" |\n\n");
}

static void tabela_tempo(void) {
    printf("=== Tempo de CPU com n = 10000 (ms, via clock(); varia com máquina e flags) ===\n");
    printf("   %-18s |  aleatória   ordenada  invertida\n", "algoritmo");
    printf("   -------------------+----------------------------------\n");
    for (int a = 0; a < N_ALG; a++) {
        coluna(algs[a].nome, 18);
        for (int e = 0; e < N_ENT; e++) printf(" %10.2f", res[a][e][N_TAM - 1].ms);
        printf("\n");
    }
    printf("\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("BENCHMARK DE ORDENAÇÃO (contagens determinísticas + tempo ilustrativo)\n");
    printf("Executando 7 algoritmos x 3 entradas x 3 tamanhos...\n\n");
    fflush(stdout);
    executa();

    for (int e = 0; e < N_ENT; e++) { tabela_contagens(e); pausar(); }
    tabela_tempo();

    printf("COMO LER\n");
    printf(" - ALEATÓRIA: os O(n²) multiplicam por ~100 quando n cresce 10x; os O(n log n), por ~13.\n");
    printf("   Com n = 10000, bolha/seleção fazem ~50 milhões de comparações; merge/quick, ~150 mil.\n");
    printf(" - ORDENADA: bolha com parada e inserção fazem só n-1 comparações (melhor caso);\n");
    printf("   o quick com pivô no último elemento cai no PIOR caso (~n²/2) e recursão de n níveis.\n");
    printf(" - INVERTIDA: pior caso de bolha e inserção; a mediana-de-três continua ~n log n.\n");
    printf(" - Seleção faz sempre ~n²/2 comparações, mas é a que MENOS movimenta (≤ 3(n-1)).\n");
    printf(" - Merge sort é o mais previsível (mesma ordem em qualquer entrada), ao custo de O(n) de\n");
    printf("   memória extra. Na prática, use qsort (ou um algoritmo estável se precisar de estabilidade).\n");
    return 0;
}
