/*
 * ============================================================================
 * Arquivo : 03_vetor_vs_lista.c
 * Módulo  : 07 — Listas encadeadas           (Ementa: Unidade 1.3 / 0.4 / 0.7)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Comparar, com contadores, um vetor (memória contígua) e uma lista
 *   encadeada (nós espalhados ligados por ponteiros) nas duas operações em
 *   que eles são opostos:
 *     1. inserir no INÍCIO: o vetor desloca todos os elementos (O(n)); a lista
 *        só religa 2 ponteiros (O(1));
 *     2. acessar o i-ésimo elemento: o vetor calcula o endereço direto (O(1));
 *        a lista caminha i nós (O(n)).
 *   Primeiro com um exemplo pequeno desenhado passo a passo, depois com uma
 *   tabela de crescimento para n = 10, 100, 1000 e 10000.
 *
 * EXEMPLO REAL
 *   - Buffer de amostras de um sensor lido por índice (média móvel, FFT):
 *     acesso aleatório por índice → vetor.
 *   - Lista de alarmes ativos de um SCADA, em que alarmes novos e críticos
 *     entram na frente e saem de qualquer posição → lista encadeada.
 *   - Playlist ou fila de impressão editável: inserir/remover no meio sem
 *     deslocar o resto → lista.
 *
 * COMPLEXIDADE (n = número de elementos)
 *   Operação                    Vetor     Lista simples
 *   inserir no início           O(n)      O(1)
 *   inserir no fim              O(1)*     O(n)  (O(1) se guardar ponteiro p/ cauda)
 *   acessar o i-ésimo           O(1)      O(i) → O(n)
 *   percorrer tudo (ponteiro)   O(n)      O(n)
 *   for(i) acessa(i)            O(n)      O(n²)  ← armadilha!
 *   memória por elemento        sizeof(int)   sizeof(int)+ponteiro+overhead malloc
 *   (*) amortizado, quando há capacidade sobrando ou realloc com dobra.
 *   Por quê: o vetor sabe onde está v[i] (base + i*sizeof(int)), mas para
 *   abrir espaço na posição 0 precisa empurrar todo mundo uma casa. A lista
 *   não sabe onde está o i-ésimo nó (tem que seguir os ponteiros), mas para
 *   inserir na frente basta trocar quem é o primeiro.
 *
 * COMO EXECUTAR
 *   ./run 03_vetor_vs_lista                (ENTER avança cada passo)
 *   ./run 03_vetor_vs_lista < /dev/null    (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct No {
    int valor;
    struct No *prox;
} No;

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static unsigned curto(const void *p) {
    return (unsigned)((uintptr_t)p & 0xFFFFu);
}

/* ======================= VETOR (memória contígua) ======================== */

/* Desenha as 'cap' casas do vetor; casas com índice >= n aparecem vazias. */
static void desenha_vetor(const int *v, size_t n, size_t cap) {
    printf("    vetor: ");
    for (size_t k = 0; k < cap; k++) {
        if (k < n) printf("[%2d]", v[k]);
        else       printf("[  ]");
    }
    printf("   (n=%zu)\n", n);
}

/*
 * Insere 'valor' na posição 0 de um vetor com n elementos e capacidade >= n+1.
 * O(n): cada elemento é copiado uma casa para a direita, DE TRÁS PARA FRENTE
 * (se fosse da frente para trás, v[0] sobrescreveria v[1] antes de ele ser
 * copiado). Devolve o número de movimentos (cópias de elementos).
 * Se 'mostrar' != 0, desenha cada deslocamento.
 */
static unsigned long vetor_insere_inicio(int *v, size_t n, size_t cap, int valor, int mostrar) {
    unsigned long movimentos = 0;
    for (size_t k = n; k > 0; k--) {
        v[k] = v[k - 1];
        movimentos++;
        if (mostrar) {
            printf("  v[%zu] = v[%zu]  (movimento %lu)\n", k, k - 1, movimentos);
            desenha_vetor(v, n + 1, cap);
            pausar();
        }
    }
    v[0] = valor;
    return movimentos;
}

/* ======================= LISTA (nós espalhados) ========================== */

static void desenha_lista(const No *inicio, const No *marcado) {
    printf("    lista: inicio-->");
    for (const No *p = inicio; p != NULL; p = p->prox)
        printf(p == marcado ? "{%2d|*}-->" : "[%2d|*]-->", p->valor);
    printf("NULL\n");
    printf("                    ");
    for (const No *p = inicio; p != NULL; p = p->prox)
        printf(" @%04x   ", curto(p));
    printf("\n");
}

/* O(1): 2 escritas de ponteiro, não importa o tamanho da lista.
 * Devolve o número de escritas de ponteiro (sempre 2) ou 0 se faltou memória. */
static unsigned long lista_insere_inicio(No **inicio, int valor) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;
    novo->valor = valor;
    novo->prox = *inicio;
    *inicio = novo;
    return 2;
}

/* O(i): segue i ponteiros a partir do início. Pré-condição: i < tamanho.
 * Soma em *saltos quantos ponteiros foram seguidos. */
static const No *lista_acessa(const No *inicio, size_t i, unsigned long *saltos) {
    const No *p = inicio;
    for (size_t k = 0; k < i; k++) {
        p = p->prox;
        (*saltos)++;
    }
    return p;
}

static void lista_libera(No **inicio) {
    while (*inicio != NULL) {
        No *seguinte = (*inicio)->prox;
        free(*inicio);
        *inicio = seguinte;
    }
}

/* ========================= PARTE 1 e 2: visual =========================== */

static int demo_visual(void) {
    enum { N = 5, CAP = 6 };
    int v[CAP] = {10, 20, 30, 40, 50, 0};
    No *lista = NULL;
    for (int k = N; k >= 1; k--)
        if (lista_insere_inicio(&lista, k * 10) == 0) { lista_libera(&lista); return 0; }

    printf("============================================================================\n");
    printf("PARTE 1 — inserir o 5 no INÍCIO de uma sequência com %d elementos\n", N);
    printf("============================================================================\n");
    printf("Estado inicial:\n");
    desenha_vetor(v, N, CAP);
    desenha_lista(lista, NULL);
    pausar();

    printf("\n-- VETOR: é preciso abrir a casa 0 empurrando todos para a direita --\n");
    unsigned long mov = vetor_insere_inicio(v, N, CAP, 5, 1);
    printf("  v[0] = 5\n");
    desenha_vetor(v, N + 1, CAP);
    printf("  Total: %lu movimentos para n = %d  → cresce com n: O(n)\n", mov, N);
    pausar();

    printf("\n-- LISTA: o novo nó aponta para o antigo primeiro, e inicio aponta para ele --\n");
    unsigned long esc = lista_insere_inicio(&lista, 5);
    if (esc == 0) { lista_libera(&lista); return 0; }
    printf("  novo->prox = inicio;  inicio = novo;\n");
    desenha_lista(lista, lista);
    printf("  Total: %lu escritas de ponteiro, nenhum nó foi movido  → O(1)\n", esc);
    pausar();

    printf("\n============================================================================\n");
    printf("PARTE 2 — acessar o elemento de índice 4\n");
    printf("============================================================================\n");
    printf("-- VETOR: o endereço é calculado, sem olhar os outros elementos --\n");
    printf("  &v[4] = &v[0] + 4*sizeof(int) = @%04x + %zu = @%04x  → v[4] = %d\n",
           curto(&v[0]), 4 * sizeof(int), curto(&v[4]), v[4]);
    printf("  Total: 1 acesso → O(1)\n");
    pausar();

    printf("\n-- LISTA: os nós não são vizinhos na memória; só dá para seguir os ponteiros --\n");
    const No *p = lista;
    printf("  p = inicio                → p está em [%d] @%04x\n", p->valor, curto(p));
    for (int k = 1; k <= 4; k++) {
        p = p->prox;
        printf("  p = p->prox  (salto %d)    → p está em [%d] @%04x\n", k, p->valor, curto(p));
        pausar();
    }
    desenha_lista(lista, p);
    printf("  Total: 4 saltos para o índice 4 → O(i), no pior caso O(n)\n");
    printf("  Note que os endereços dos nós NÃO são consecutivos de 4 em 4 bytes como no vetor.\n");
    pausar();

    lista_libera(&lista);
    return 1;
}

/* ========================= PARTE 3: tabelas ============================== */

typedef struct {
    unsigned long vet_ins1, lis_ins1;     /* 1 inserção no início com n elementos */
    unsigned long vet_build, lis_build;   /* construir com n inserções no início */
    unsigned long vet_get, lis_get;       /* acessar o último (índice n-1) */
    unsigned long vet_all, lis_all;       /* for (i = 0..n-1) acessa(i) */
} Medidas;

/* Mede todas as operações para um n. Devolve 0 se faltar memória. */
static int medir(size_t n, Medidas *m) {
    int *v = malloc((n + 1) * sizeof *v);
    No *lista = NULL;
    if (v == NULL) return 0;
    *m = (Medidas){0};

    /* Construção: n inserções no início em cada estrutura (contagens reais). */
    for (size_t k = 0; k < n; k++) {
        m->vet_build += vetor_insere_inicio(v, k, n + 1, (int)k, 0);
        unsigned long e = lista_insere_inicio(&lista, (int)k);
        if (e == 0) { free(v); lista_libera(&lista); return 0; }
        m->lis_build += e;
    }

    /* Uma inserção a mais no início, com a estrutura já com n elementos. */
    m->vet_ins1 = vetor_insere_inicio(v, n, n + 1, -1, 0);
    m->lis_ins1 = lista_insere_inicio(&lista, -1);
    if (m->lis_ins1 == 0) { free(v); lista_libera(&lista); return 0; }

    /* Acesso ao índice n-1: no vetor, 1 acesso; na lista, n-1 saltos. */
    volatile int soma = 0;   /* volatile: impede o compilador de apagar os acessos */
    soma += v[n - 1];
    m->vet_get = 1;
    soma += lista_acessa(lista, n - 1, &m->lis_get)->valor;

    /* Percorrer "por índice": no vetor n acessos; na lista 0+1+...+(n-1) saltos. */
    for (size_t i = 0; i < n; i++) {
        soma += v[i];
        m->vet_all++;
        soma += lista_acessa(lista, i, &m->lis_all)->valor;
    }
    (void)soma;

    free(v);
    lista_libera(&lista);
    return 1;
}

static int tabelas(void) {
    static const size_t tamanhos[] = {10, 100, 1000, 10000};
    enum { QTD = sizeof tamanhos / sizeof tamanhos[0] };
    Medidas m[QTD];

    for (int t = 0; t < QTD; t++)
        if (!medir(tamanhos[t], &m[t])) return 0;

    printf("\n============================================================================\n");
    printf("PARTE 3 — tabela de crescimento (contagens exatas, medidas executando)\n");
    printf("============================================================================\n");

    printf("\n(a) Inserir UM elemento no início de uma estrutura que já tem n elementos\n");
    printf("    %8s | %18s | %22s\n", "n", "vetor: movimentos", "lista: escritas de ptr");
    printf("    ---------+--------------------+-----------------------\n");
    for (int t = 0; t < QTD; t++)
        printf("    %8zu | %18lu | %22lu\n", tamanhos[t], m[t].vet_ins1, m[t].lis_ins1);
    printf("    → vetor O(n) (cresce 10x quando n cresce 10x); lista O(1) (constante).\n");
    pausar();

    printf("\n(b) Construir a estrutura com n inserções no início (custo total)\n");
    printf("    %8s | %18s | %22s\n", "n", "vetor: movimentos", "lista: escritas de ptr");
    printf("    ---------+--------------------+-----------------------\n");
    for (int t = 0; t < QTD; t++)
        printf("    %8zu | %18lu | %22lu\n", tamanhos[t], m[t].vet_build, m[t].lis_build);
    printf("    → vetor n(n-1)/2 = O(n²) (cresce 100x); lista 2n = O(n) (cresce 10x).\n");
    pausar();

    printf("\n(c) Acessar o elemento de índice n-1 (o último)\n");
    printf("    %8s | %18s | %22s\n", "n", "vetor: acessos", "lista: saltos");
    printf("    ---------+--------------------+-----------------------\n");
    for (int t = 0; t < QTD; t++)
        printf("    %8zu | %18lu | %22lu\n", tamanhos[t], m[t].vet_get, m[t].lis_get);
    printf("    → vetor O(1); lista O(n). Aqui a vantagem é do vetor.\n");
    pausar();

    printf("\n(d) ARMADILHA: percorrer com for (i = 0; i < n; i++) acessa(i)\n");
    printf("    %8s | %18s | %22s\n", "n", "vetor: acessos", "lista: saltos");
    printf("    ---------+--------------------+-----------------------\n");
    for (int t = 0; t < QTD; t++)
        printf("    %8zu | %18lu | %22lu\n", tamanhos[t], m[t].vet_all, m[t].lis_all);
    printf("    → na lista, cada acessa(i) recomeça do início: n(n-1)/2 = O(n²).\n"
           "      Para percorrer uma lista use um ponteiro: for (p = inicio; p; p = p->prox),\n"
           "      que custa só n-1 saltos (O(n)).\n");
    pausar();
    return 1;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    if (!demo_visual() || !tabelas()) {
        fprintf(stderr, "Sem memória para a demonstração.\n");
        return EXIT_FAILURE;
    }

    printf("\n============================================================================\n");
    printf("PARTE 4 — custo de memória e conclusão\n");
    printf("============================================================================\n");
    printf("  sizeof(int) = %zu bytes por elemento no vetor.\n", sizeof(int));
    printf("  sizeof(No)  = %zu bytes por nó na lista (valor + ponteiro + alinhamento),\n"
           "                fora o cabeçalho que o malloc guarda para cada bloco.\n", sizeof(No));
    printf("  Além disso, nós espalhados aproveitam mal a memória cache; elementos\n"
           "  contíguos de um vetor são lidos em blocos. Na prática, o vetor costuma\n"
           "  vencer quando não há inserções/remoções no início ou no meio.\n\n");
    printf("  Use VETOR quando: acesso por índice, tamanho conhecido, percurso rápido.\n");
    printf("  Use LISTA quando: muitas inserções/remoções no início ou no meio (com o nó já\n"
           "  localizado), tamanho imprevisível, elementos que não podem mudar de endereço.\n");
    return EXIT_SUCCESS;
}
