/*
 * ============================================================================
 * Arquivo : 03_vetor_dinamico.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.7 Alocação dinâmica)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Construir um vetor que CRESCE sozinho (como std::vector do C++, ArrayList
 *   do Java ou list do Python) e provar, contando cópias, por que a regra
 *   "quando encher, DOBRE a capacidade" dá custo amortizado O(1) por inserção,
 *   enquanto "cresça +1" custa O(n) por inserção (O(n²) no total).
 *   O crescimento é feito à mão (malloc novo + cópia + free antigo) para que
 *   cada cópia de elemento seja visível e contada.
 *
 * EXEMPLO REAL
 *   Um registrador de dados (data logger) não sabe quantas amostras chegarão
 *   durante um ensaio: o buffer começa pequeno e cresce. Com +1, o tempo de
 *   cada inserção piora conforme o ensaio avança e o sistema perde amostras;
 *   dobrando, o custo médio fica constante. (Exemplo completo: 06_log_sensores.c.)
 *
 * COMPLEXIDADE (n = elementos inseridos)
 *   Estratégia     Inserção (pior)   Inserção (amortizada)   Cópias totais   Espaço
 *   dobrar (x2)    O(n)              O(1)                    < 2n            ≤ 2n
 *   somar +1       O(n)              O(n)                    n(n-1)/2        n
 *   acesso v[i]    O(1)              —                       —               —
 *   Por quê (x2): as cópias acontecem em 1, 2, 4, 8, ..., n/2 -> soma < n.
 *   Uma inserção cara "paga" pelas muitas baratas que vieram antes dela.
 *
 * COMO EXECUTAR
 *   ./run 03_vetor_dinamico               (ENTER avança cada passo)
 *   ./run 03_vetor_dinamico < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
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

typedef enum { DOBRAR, MAIS_UM } Estrategia;

typedef struct {
    int *dados;                 /* bloco no heap */
    size_t tamanho;             /* elementos em uso */
    size_t capacidade;          /* elementos que cabem no bloco atual */
    Estrategia estrategia;
    unsigned long long copias;  /* elementos copiados em todos os crescimentos */
    unsigned realocacoes;
} VetorDin;

/* O(1): vetor vazio, capacidade inicial 1. */
static int vd_inicia(VetorDin *v, Estrategia e) {
    v->dados = malloc(sizeof *v->dados);
    v->tamanho = 0;
    v->capacidade = v->dados ? 1 : 0;
    v->estrategia = e;
    v->copias = 0;
    v->realocacoes = 0;
    return v->dados != NULL;
}

/* O(n): troca o bloco por um maior e copia os elementos um a um.
 * (realloc faria o mesmo por dentro quando não consegue crescer no lugar.) */
static int vd_cresce(VetorDin *v, int mostrar) {
    size_t nova = v->estrategia == DOBRAR ? v->capacidade * 2 : v->capacidade + 1;
    int *novo = malloc(nova * sizeof *novo);
    if (novo == NULL) return 0;              /* o vetor antigo continua intacto */
    for (size_t i = 0; i < v->tamanho; i++)  /* <- o custo O(n) mora aqui */
        novo[i] = v->dados[i];
    if (mostrar)
        printf("      CHEIO! aloca capacidade %zu, copia %zu elemento(s), libera o bloco antigo\n",
               nova, v->tamanho);
    v->copias += v->tamanho;
    v->realocacoes++;
    free(v->dados);
    v->dados = novo;
    v->capacidade = nova;
    return 1;
}

/* O(1) amortizado com DOBRAR; O(n) com MAIS_UM. */
static int vd_insere(VetorDin *v, int valor, int mostrar) {
    if (v->tamanho == v->capacidade && !vd_cresce(v, mostrar))
        return 0;
    v->dados[v->tamanho++] = valor;
    return 1;
}

static void vd_libera(VetorDin *v) {
    free(v->dados);
    v->dados = NULL;
    v->tamanho = v->capacidade = 0;
}

/* O(capacidade): desenha as casas usadas e as livres (·). */
static void vd_desenha(const VetorDin *v) {
    printf("      [");
    for (size_t i = 0; i < v->capacidade; i++) {
        if (i < v->tamanho) printf("%3d", v->dados[i]);
        else printf("  .");
    }
    printf(" ]  tamanho=%zu capacidade=%zu | copias acumuladas=%llu\n",
           v->tamanho, v->capacidade, v->copias);
}

/* Insere 1..n e devolve as cópias totais (ou -1 sem memória). */
static long long mede(Estrategia e, size_t n, unsigned *realocs, size_t *cap_final) {
    VetorDin v;
    if (!vd_inicia(&v, e)) return -1;
    for (size_t i = 0; i < n; i++) {
        if (!vd_insere(&v, (int)i, 0)) { vd_libera(&v); return -1; }
    }
    long long c = (long long)v.copias;
    *realocs = v.realocacoes;
    *cap_final = v.capacidade;
    vd_libera(&v);
    return c;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    VetorDin v;

    printf("=== 1) Estrategia DOBRAR: inserindo 1..9 ===\n");
    if (!vd_inicia(&v, DOBRAR)) { printf("sem memoria\n"); return 0; }
    for (int x = 1; x <= 9; x++) {
        printf("    insere %d\n", x);
        if (!vd_insere(&v, x, 1)) { printf("sem memoria\n"); break; }
        vd_desenha(&v);
        pausar();
    }
    printf("    9 insercoes, %llu copias -> %.2f copias por insercao (amortizado O(1))\n",
           v.copias, (double)v.copias / 9.0);
    vd_libera(&v);
    pausar();

    printf("\n=== 2) Estrategia +1: inserindo 1..9 ===\n");
    if (!vd_inicia(&v, MAIS_UM)) { printf("sem memoria\n"); return 0; }
    for (int x = 1; x <= 9; x++) {
        printf("    insere %d\n", x);
        if (!vd_insere(&v, x, 1)) { printf("sem memoria\n"); break; }
        vd_desenha(&v);
    }
    printf("    9 insercoes, %llu copias -> %.2f copias por insercao (cresce com n)\n",
           v.copias, (double)v.copias / 9.0);
    vd_libera(&v);
    pausar();

    printf("\n=== 3) Crescimento (executado de verdade, contando copias) ===\n");
    printf("%9s || %11s | %6s | %7s || %13s | %6s | %8s\n",
           "n", "x2: copias", "realoc", "cop/n", "+1: copias", "realoc", "cop/n");
    printf("----------++-------------+--------+---------++---------------+--------+---------\n");
    for (size_t n = 10; n <= 10000; n *= 10) {
        unsigned r1, r2;
        size_t cap1, cap2;
        long long c1 = mede(DOBRAR, n, &r1, &cap1);
        long long c2 = mede(MAIS_UM, n, &r2, &cap2);
        if (c1 < 0 || c2 < 0) { printf("sem memoria\n"); return 0; }
        printf("%9zu || %11lld | %6u | %7.2f || %13lld | %6u | %8.1f\n",
               n, c1, r1, (double)c1 / (double)n, c2, r2, (double)c2 / (double)n);
    }
    {
        unsigned r1 = 0;
        size_t cap1 = 0;
        size_t n = 1000000;
        long long c1 = mede(DOBRAR, n, &r1, &cap1);
        if (c1 >= 0)
            printf("%9zu || %11lld | %6u | %7.2f || %13s | %6s | %8s\n",
                   n, c1, r1, (double)c1 / (double)n, "~5e11", "999999", "nao exec");
        printf("\nCom x2, copias/n fica abaixo de 2 para qualquer n: O(1) amortizado.\n");
        printf("Com +1, copias/n cresce junto com n: O(n) por insercao, O(n^2) no total.\n");
        printf("Preco do x2: ate metade da capacidade pode ficar ociosa (n = %zu ocupa\n", n);
        printf("capacidade %zu). Fatores como 1,5 trocam um pouco de tempo por menos espaco.\n", cap1);
    }
    return 0;
}
