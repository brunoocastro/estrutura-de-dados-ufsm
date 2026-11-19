/*
 * ============================================================================
 * Arquivo : 04_matriz_dinamica.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.4 Matrizes, 0.7 Alocação)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Três jeitos de criar uma matriz cujo tamanho só é conhecido em execução,
 *   com os endereços impressos para comparar o arranjo na memória:
 *     A) ponteiro para ponteiro (int **): 1 vetor de ponteiros + 1 malloc por
 *        linha. Linhas espalhadas; permite linhas de tamanhos diferentes.
 *     B) bloco contíguo (int *): 1 malloc de L*C; acesso m[l*C + c].
 *     C) híbrido: vetor de ponteiros apontando para DENTRO de um bloco único:
 *        sintaxe m[l][c] com memória contígua (2 mallocs).
 *   No fim, uma matriz TRIANGULAR (linhas de tamanhos diferentes) mostra a
 *   vantagem do int ** para guardar só metade dos dados.
 *
 * EXEMPLO REAL
 *   O mapa de temperaturas de um forno com L x C termopares lido do arquivo
 *   de configuração; a matriz de distâncias entre n estações de um AGV
 *   (simétrica: basta guardar o triângulo inferior).
 *
 * COMPLEXIDADE (L = linhas, C = colunas)
 *   Arranjo        mallocs   free     acesso m[l][c]          memória extra
 *   A) int **      L + 1     L + 1    O(1), 2 leituras         L ponteiros
 *   B) contíguo    1         1        O(1), 1 leitura + conta  nenhuma
 *   C) híbrido     2         2        O(1), 2 leituras         L ponteiros
 *   Preencher/percorrer: O(L*C) em todos. Triangular: n(n+1)/2 em vez de n².
 *
 * COMO EXECUTAR
 *   ./run 04_matriz_dinamica               (pede L e C; ENTER avança)
 *   ./run 04_matriz_dinamica < /dev/null   (usa 3 x 4, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static unsigned mallocs = 0, frees = 0;

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Envoltórios que contam as chamadas para comparar os arranjos. */
static void *aloca(size_t bytes) { void *p = malloc(bytes); if (p) mallocs++; return p; }
static void libera(void *p) { if (p) frees++; free(p); }

/* ---------------- A) ponteiro para ponteiro ---------------- */

/* O(L) mallocs. Se uma linha falhar, libera o que já foi alocado (senão
 * vazaria memória) e devolve NULL. */
static int **cria_ponteiros(size_t L, size_t C) {
    int **m = aloca(L * sizeof *m);          /* vetor de L ponteiros */
    if (m == NULL) return NULL;
    for (size_t l = 0; l < L; l++) {
        m[l] = aloca(C * sizeof *m[l]);      /* cada linha é um bloco separado */
        if (m[l] == NULL) {
            while (l-- > 0) libera(m[l]);
            libera(m);
            return NULL;
        }
    }
    return m;
}

/* O(L): libera as linhas ANTES do vetor de ponteiros (depois de liberar o
 * vetor, não teríamos mais como achar as linhas). */
static void libera_ponteiros(int **m, size_t L) {
    if (m == NULL) return;
    for (size_t l = 0; l < L; l++) libera(m[l]);
    libera(m);
}

/* ---------------- C) híbrido ---------------- */

/* 2 mallocs: um bloco com L*C ints e um vetor de L ponteiros para o início
 * de cada linha dentro do bloco. Permite m[l][c] e mantém a contiguidade. */
static int **cria_hibrida(size_t L, size_t C) {
    int **m = aloca(L * sizeof *m);
    int *bloco = aloca(L * C * sizeof *bloco);
    if (m == NULL || bloco == NULL) { libera(m); libera(bloco); return NULL; }
    for (size_t l = 0; l < L; l++) m[l] = bloco + l * C;
    return m;
}

static void libera_hibrida(int **m) {
    if (m == NULL) return;
    libera(m[0]);   /* m[0] aponta para o início do bloco único */
    libera(m);
}

/* Imprime, para cada linha, o endereço inicial e a distância (bytes) até a
 * linha anterior: C * sizeof(int) significa "colada" na anterior. */
static void mostra_linhas(int *const linhas[], size_t L, size_t C) {
    for (size_t l = 0; l < L; l++) {
        printf("    linha %zu em %p", l, (void *)linhas[l]);
        if (l > 0) {
            uintptr_t a = (uintptr_t)linhas[l - 1], b = (uintptr_t)linhas[l];
            long long d = (long long)b - (long long)a;
            printf("  (distancia da anterior: %lld bytes%s)", d,
                   d == (long long)(C * sizeof(int)) ? ", colada" : ", NAO contigua");
        }
        printf(" :");
        for (size_t c = 0; c < C; c++) printf(" %3d", linhas[l][c]);
        printf("\n");
    }
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    size_t L = 3, C = 4;
    printf("Linhas e colunas (ex.: 3 4): ");
    if (scanf("%zu %zu", &L, &C) != 2 || L < 1 || C < 1 || L > 20 || C > 12) {
        L = 3;
        C = 4;
        printf("(entrada vazia ou fora de 1..20 x 1..12: usando %zu x %zu)\n", L, C);
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) { }
    }

    printf("\n=== A) int **: um vetor de ponteiros + um malloc por linha ===\n");
    printf("    m --> [ * ] --> [linha 0 ...]\n");
    printf("          [ * ] --> [linha 1 ...]     (cada linha em um lugar do heap)\n");
    printf("          [ * ] --> [linha 2 ...]\n");
    mallocs = frees = 0;
    int **a = cria_ponteiros(L, C);
    if (a == NULL) { printf("sem memoria\n"); return 0; }
    for (size_t l = 0; l < L; l++)
        for (size_t c = 0; c < C; c++) a[l][c] = (int)(l * 10 + c);
    mostra_linhas(a, L, C);
    printf("    acesso a[l][c]: le a[l] (um ponteiro) e depois o int -> O(1), 2 leituras\n");
    libera_ponteiros(a, L);
    printf("    mallocs = %u, frees = %u\n", mallocs, frees);
    pausar();

    printf("\n=== B) bloco contiguo: um malloc de L*C, indice l*C + c ===\n");
    printf("    m --> [l0c0 l0c1 ... | l1c0 l1c1 ... | l2c0 ...]\n");
    mallocs = frees = 0;
    int *b = aloca(L * C * sizeof *b);
    if (b == NULL) { printf("sem memoria\n"); return 0; }
    int *linhas_b[20];
    for (size_t l = 0; l < L; l++) {
        for (size_t c = 0; c < C; c++) b[l * C + c] = (int)(l * 10 + c);
        linhas_b[l] = &b[l * C];   /* só para imprimir com a mesma função */
    }
    mostra_linhas(linhas_b, L, C);
    printf("    b[%zu*%zu + %zu] = %d  (o mesmo calculo que o compilador faz em int m[L][C])\n",
           L - 1, C, C - 1, b[(L - 1) * C + (C - 1)]);
    libera(b);
    printf("    mallocs = %u, frees = %u  (melhor para cache e para liberar)\n", mallocs, frees);
    pausar();

    printf("\n=== C) hibrido: ponteiros para dentro de um bloco unico ===\n");
    mallocs = frees = 0;
    int **h = cria_hibrida(L, C);
    if (h == NULL) { printf("sem memoria\n"); return 0; }
    for (size_t l = 0; l < L; l++)
        for (size_t c = 0; c < C; c++) h[l][c] = (int)(l * 10 + c);
    mostra_linhas(h, L, C);
    libera_hibrida(h);
    printf("    mallocs = %u, frees = %u  (sintaxe h[l][c] e memoria contigua)\n", mallocs, frees);
    pausar();

    printf("\n=== D) Matriz triangular (so o int ** permite linhas de tamanhos diferentes) ===\n");
    /* Distâncias entre 5 estações: d[i][j] == d[j][i] e d[i][i] == 0, então
     * guardamos só j <= i: a linha i tem i + 1 elementos. */
    const size_t n = 5;
    const int pos[5] = {0, 4, 9, 15, 22}; /* posição das estações num trilho (m) */
    mallocs = frees = 0;
    int **tri = aloca(n * sizeof *tri);
    if (tri == NULL) { printf("sem memoria\n"); return 0; }
    for (size_t i = 0; i < n; i++) {
        tri[i] = aloca((i + 1) * sizeof *tri[i]);
        if (tri[i] == NULL) {
            while (i-- > 0) libera(tri[i]);
            libera(tri);
            printf("sem memoria\n");
            return 0;
        }
        for (size_t j = 0; j <= i; j++) tri[i][j] = abs(pos[i] - pos[j]);
    }
    for (size_t i = 0; i < n; i++) {
        printf("    estacao %zu:", i);
        for (size_t j = 0; j <= i; j++) printf(" %3d", tri[i][j]);
        printf("\n");
    }
    printf("    distancia(1, 3) = tri[3][1] = %d m (para i < j, use tri[j][i])\n", tri[3][1]);
    for (size_t i = 0; i < n; i++) libera(tri[i]);
    libera(tri);
    printf("    mallocs = %u, frees = %u\n", mallocs, frees);

    printf("\n    Memoria (ints) para n estacoes:\n");
    printf("    %8s | %14s | %14s\n", "n", "quadrada n^2", "triangular");
    for (unsigned long long k = 10; k <= 10000; k *= 10)
        printf("    %8llu | %14llu | %14llu\n", k, k * k, k * (k + 1) / 2);
    return 0;
}
