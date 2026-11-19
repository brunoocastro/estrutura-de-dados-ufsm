/*
 * ============================================================================
 * Arquivo : 02_malloc_calloc_realloc_free.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.7 Alocação dinâmica)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   As quatro funções de <stdlib.h>, uma por passo, com o bloco desenhado:
 *     malloc(bytes)          -> bloco com conteúdo INDETERMINADO ("lixo");
 *     calloc(n, tamanho)     -> bloco ZERADO, e confere o estouro de n*tamanho;
 *     realloc(p, novo)       -> aumenta/diminui; pode MUDAR o endereço (e então
 *                               copia os dados antigos para o bloco novo);
 *     free(p)                -> devolve o bloco; free(NULL) não faz nada.
 *   Mostra o padrão seguro do realloc (ponteiro temporário) e a checagem de
 *   overflow em n * sizeof.
 *
 * EXEMPLO REAL
 *   Um gateway lê do arquivo de configuração quantos dispositivos Modbus vai
 *   consultar (n) e aloca o vetor de leituras com calloc(n, ...). Se um
 *   dispositivo novo é adicionado em operação, realloc aumenta o vetor.
 *
 * COMPLEXIDADE (n = elementos)
 *   Função            Custo típico
 *   malloc            O(1) amortizado (não toca nos bytes)
 *   calloc            O(n)  (precisa zerar os n elementos)
 *   realloc maior     O(1) se crescer no lugar; O(n) se mudar de endereço (cópia)
 *   realloc menor     O(1) em geral
 *   free              O(1)
 *
 * COMO EXECUTAR
 *   ./run 02_malloc_calloc_realloc_free               (ENTER avança cada passo)
 *   ./run 02_malloc_calloc_realloc_free < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
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

/* O(n): desenha o bloco. Só os "validos" primeiros elementos foram escritos;
 * os demais aparecem como ?? — NÃO os lemos, porque ler memória não
 * inicializada dá valores imprevisíveis (e ferramentas acusam erro). */
static void desenha(const char *nome, const int *p, size_t n, size_t validos) {
    printf("    %-6s = %p -> ", nome, (const void *)p);
    for (size_t i = 0; i < n; i++) {
        if (i < validos) printf("[%3d]", p[i]);
        else printf("[ ??]");
    }
    printf("  (%zu x %zu = %zu bytes)\n", n, sizeof *p, n * sizeof *p);
}

/* O(1): aloca n ints, recusando se n * sizeof(int) estourar size_t.
 * Sem essa checagem, um n enorme "dá a volta" e malloc reserva um bloco
 * PEQUENO, que depois seria estourado (falha de segurança clássica). */
static int *aloca_ints(size_t n) {
    if (n != 0 && n > SIZE_MAX / sizeof(int))
        return NULL;
    return malloc(n * sizeof(int));
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    printf("=== 1) malloc: reserva bytes, conteudo indeterminado ===\n");
    printf("    int *v = malloc(5 * sizeof *v);\n");
    int *v = malloc(5 * sizeof *v);
    if (v == NULL) { printf("sem memoria\n"); return 0; }  /* SEMPRE verifique */
    desenha("v", v, 5, 0);
    for (size_t i = 0; i < 5; i++) v[i] = (int)(i + 1) * 10;
    printf("    depois de preencher:\n");
    desenha("v", v, 5, 5);
    pausar();

    printf("\n=== 2) calloc: reserva E zera (custo O(n) para zerar) ===\n");
    printf("    int *z = calloc(5, sizeof *z);\n");
    int *z = calloc(5, sizeof *z);
    if (z == NULL) { free(v); printf("sem memoria\n"); return 0; }
    desenha("z", z, 5, 5);
    pausar();

    printf("\n=== 3) realloc para 8 elementos (padrao SEGURO com temporario) ===\n");
    printf("    int *tmp = realloc(v, 8 * sizeof *v);\n");
    printf("    if (tmp == NULL) { /* v continua valido! trate o erro */ } else v = tmp;\n");
    uintptr_t antes = (uintptr_t)v; /* guardado como número: após realloc, o
                                       ponteiro antigo não pode mais ser usado */
    int *tmp = realloc(v, 8 * sizeof *v);
    if (tmp == NULL) {
        printf("    realloc falhou; v (5 elementos) continua valido\n");
    } else {
        v = tmp;
        if ((uintptr_t)v == antes)
            printf("    cresceu NO LUGAR: havia espaco livre logo depois -> O(1)\n");
        else
            printf("    MUDOU de endereco (0x%llx -> %p): copiou os 5 valores -> O(n)\n",
                   (unsigned long long)antes, (void *)v);
        printf("    os 5 primeiros foram preservados; os 3 novos estao indeterminados:\n");
        desenha("v", v, 8, 5);
        for (size_t i = 5; i < 8; i++) v[i] = (int)(i + 1) * 10;
        desenha("v", v, 8, 8);
    }
    printf("    ERRADO: v = realloc(v, ...); se falhar, v vira NULL e o bloco antigo\n");
    printf("    fica sem dono (vazamento de memoria).\n");
    pausar();

    printf("\n=== 4) realloc para diminuir (3 elementos) ===\n");
    tmp = realloc(v, 3 * sizeof *v);
    if (tmp != NULL) v = tmp; /* se falhar, o bloco maior continua valido */
    desenha("v", v, 3, 3);
    printf("    os elementos 3..7 deixaram de pertencer ao programa.\n");
    pausar();

    printf("\n=== 5) Casos especiais ===\n");
    int *r = realloc(NULL, 2 * sizeof *r);   /* realloc(NULL, n) == malloc(n) */
    printf("    realloc(NULL, 8 bytes) -> %s (equivale a malloc)\n", r ? "bloco novo" : "NULL");
    free(r);
    free(NULL);                               /* permitido: não faz nada */
    printf("    free(NULL) -> nada acontece (por isso 'p = NULL' apos free e seguro)\n");
    size_t absurdo = SIZE_MAX / 2;
    int *o = aloca_ints(absurdo);
    printf("    aloca_ints(SIZE_MAX/2) -> %s\n",
           o ? "alocou?!" : "NULL: n * sizeof(int) estouraria, pedido recusado antes do malloc");
    free(o);
    printf("    (calloc ja faz essa checagem internamente; com malloc, faca voce.)\n");
    pausar();

    printf("\n=== 6) free: devolve os blocos ===\n");
    free(v);
    v = NULL;
    free(z);
    z = NULL;
    printf("    free(v); v = NULL;  free(z); z = NULL;\n");
    printf("    Regra: cada malloc/calloc/realloc bem-sucedido tem exatamente UM free.\n");
    return 0;
}
