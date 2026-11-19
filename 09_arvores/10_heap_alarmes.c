/*
 * ============================================================================
 * Arquivo : 10_heap_alarmes.c
 * Módulo  : 09 — Árvores                      (Ementa: Unidade 5.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver uma HEAP BINÁRIA (max-heap) funcionando como fila de prioridade de
 *   alarmes. A heap é uma árvore binária COMPLETA guardada num VETOR, sem
 *   ponteiros: o nó do índice i tem filhos em 2i+1 e 2i+2 e pai em (i-1)/2.
 *   A cada passo o programa mostra o vetor e a árvore correspondente:
 *     inserir  -> o alarme entra no fim do vetor e SOBE trocando com o pai;
 *     atender  -> a raiz (mais urgente) sai, o último vai para a raiz e DESCE
 *                 trocando com o filho mais urgente.
 *   Por fim compara heap, vetor desordenado e vetor ordenado até n = 100000.
 *
 * EXEMPLO REAL
 *   Gerenciamento de alarmes (ISA-18.2 / IEC 62682) num SCADA: numa avalanche
 *   de alarmes, o operador deve ver primeiro o mais crítico. Também é a fila
 *   de prontos de um RTOS (tarefa de maior prioridade primeiro), a fila do
 *   Dijkstra (10_grafos) e a base do algoritmo de ordenação heapsort.
 *
 * COMPLEXIDADE (n = alarmes ativos)
 *   Operação              Heap        Vetor desordenado   Vetor ordenado
 *   inserir               O(log n)    O(1)                O(n) (desloca)
 *   ver o mais urgente    O(1)        O(n)                O(1)
 *   atender (remover)     O(log n)    O(n)                O(1)
 *   espaço                O(n), sem ponteiros (só o vetor)
 *   Por quê: a heap é completa, então tem altura ⌊log2 n⌋; subir ou descer
 *   percorre no máximo um caminho raiz-folha. Ela NÃO é ordenada como uma
 *   BST: só garante pai >= filhos, o suficiente para achar o máximo na raiz.
 *
 * COMO EXECUTAR
 *   ./run 10_heap_alarmes               (ENTER avança cada passo)
 *   ./run 10_heap_alarmes < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int prioridade;       /* 1..999: maior = mais urgente                     */
    long seq;             /* ordem de chegada: desempata (o mais antigo antes) */
    const char *tag;      /* texto do alarme (literal; não liberar)           */
} Alarme;

typedef struct {
    Alarme *v;            /* vetor da heap: v[0] é a raiz                      */
    int n;                /* alarmes ativos                                    */
    int cap;              /* capacidade alocada                                */
} Heap;

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static int narrar = 1;            /* 0 na tabela de desempenho     */
static long long comparacoes;     /* comparações entre alarmes      */
static long seq_global = 0;       /* contador de chegada            */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void *aloca(size_t bytes) {
    void *p = malloc(bytes);
    if (p == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/* `a` é mais urgente que `b`? Prioridade maior; empate: chegou antes. O(1). */
static int mais_urgente(const Alarme *a, const Alarme *b) {
    comparacoes++;
    if (a->prioridade != b->prioridade) return a->prioridade > b->prioridade;
    return a->seq < b->seq;
}

static void troca(Alarme *a, Alarme *b) {
    Alarme t = *a;
    *a = *b;
    *b = t;
}

/* ------------------------------------------------------------------------- */
/* Desenho: vetor e árvore (a mesma informação, duas leituras)                */
/* ------------------------------------------------------------------------- */
#define ESP 6
#define LARG 100
#define NIVEIS 6

static char tela[2 * NIVEIS][LARG + 1];
static int destaque = -1;   /* índice do elemento que está subindo/descendo */

static void mostra_vetor(const Heap *h) {
    printf("    índice :");
    for (int i = 0; i < h->n; i++) printf(" %5d", i);
    printf("\n    vetor  :");
    for (int i = 0; i < h->n; i++) {
        if (i == destaque) printf(" [%3d]", h->v[i].prioridade);
        else               printf(" %5d", h->v[i].prioridade);
    }
    printf("\n");
}

/* Mesmo layout das outras árvores (coluna = posição em-ordem), mas
 * navegando por ÍNDICES: filhos de i em 2i+1 e 2i+2. O(n). */
static int posiciona(const Heap *h, int i, int nivel, int *ordem) {
    if (i >= h->n) return -1;
    int ce = posiciona(h, 2 * i + 1, nivel + 1, ordem);
    int c = (*ordem)++ * ESP + ESP / 2;
    int cd = posiciona(h, 2 * i + 2, nivel + 1, ordem);
    char txt[16];
    if (i == destaque) snprintf(txt, sizeof txt, "[%d]", h->v[i].prioridade);
    else               snprintf(txt, sizeof txt, "%d", h->v[i].prioridade);
    int w = (int)strlen(txt);
    for (int k = 0; k < w; k++) {
        int col = c - w / 2 + k;
        if (col >= 0 && col < LARG) tela[2 * nivel][col] = txt[k];
    }
    char *lig = tela[2 * nivel + 1];
    if (ce >= 0) { lig[ce] = '/';  for (int k = ce + 1; k < c; k++) lig[k] = '-'; }
    if (cd >= 0) { lig[cd] = '\\'; for (int k = c + 1; k < cd; k++) lig[k] = '-'; }
    if (ce >= 0 || cd >= 0) lig[c] = '+';
    return c;
}

static void desenha(const Heap *h) {
    mostra_vetor(h);
    if (h->n == 0) { printf("    (nenhum alarme ativo)\n"); return; }
    int alt = 0;
    while ((2 << alt) - 1 < h->n) alt++;          /* altura = ⌊log2 n⌋ */
    if (h->n * ESP > LARG || alt >= NIVEIS) return;
    for (int l = 0; l < 2 * NIVEIS; l++) {
        memset(tela[l], ' ', LARG);
        tela[l][LARG] = '\0';
    }
    int ordem = 0;
    posiciona(h, 0, 0, &ordem);
    printf("    árvore :\n");
    for (int l = 0; l <= 2 * alt; l++) {
        int fim = LARG;
        while (fim > 0 && tela[l][fim - 1] == ' ') fim--;
        printf("    %.*s\n", fim, tela[l]);
    }
}

/* ------------------------------------------------------------------------- */
/* Operações da heap                                                          */
/* ------------------------------------------------------------------------- */
static void heap_inicia(Heap *h, int cap) {
    h->v = aloca((size_t)cap * sizeof *h->v);
    h->n = 0;
    h->cap = cap;
}

/*
 * Inserir: coloca no fim (a árvore continua completa) e SOBE enquanto for
 * mais urgente que o pai. O(log n): no máximo ⌊log2 n⌋ trocas.
 * Pré-condição: h->n < h->cap.
 */
static void heap_insere(Heap *h, int prioridade, const char *tag) {
    int i = h->n++;
    h->v[i] = (Alarme){prioridade, seq_global++, tag};
    if (narrar) {
        if (i == 0) printf("    heap vazia: entra na raiz, índice 0\n");
        else        printf("    entra no fim do vetor, índice %d (pai em (%d-1)/2 = %d)\n",
                           i, i, (i - 1) / 2);
    }
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (!mais_urgente(&h->v[i], &h->v[pai])) {
            if (narrar) printf("    %d <= pai %d -> PARA (pai é mais urgente)\n",
                               h->v[i].prioridade, h->v[pai].prioridade);
            break;
        }
        if (narrar) printf("    %d > pai %d (índice %d) -> troca, sobe\n",
                           h->v[i].prioridade, h->v[pai].prioridade, pai);
        troca(&h->v[i], &h->v[pai]);
        i = pai;
    }
    if (narrar && i == 0) printf("    chegou na raiz: é o alarme mais urgente agora\n");
    destaque = i;
}

/*
 * Atender: devolve a raiz, move o ÚLTIMO elemento para a raiz e o faz DESCER
 * trocando com o filho mais urgente, até não haver filho mais urgente.
 * O(log n). Pré-condição: h->n > 0.
 * Armadilha: trocar com o filho "errado" (o menos urgente) quebraria a regra
 * pai >= filhos no outro lado.
 */
static Alarme heap_remove(Heap *h) {
    Alarme topo = h->v[0];
    h->v[0] = h->v[--h->n];
    int i = 0;
    if (narrar && h->n > 0)
        printf("    último (%d) vai para a raiz; agora desce\n", h->v[0].prioridade);
    for (;;) {
        int e = 2 * i + 1, d = 2 * i + 2, m = i;
        if (e < h->n && mais_urgente(&h->v[e], &h->v[m])) m = e;
        if (d < h->n && mais_urgente(&h->v[d], &h->v[m])) m = d;
        if (m == i) break;
        if (narrar) printf("    filho mais urgente de %d é %d (índice %d) -> troca, desce\n",
                           h->v[i].prioridade, h->v[m].prioridade, m);
        troca(&h->v[i], &h->v[m]);
        i = m;
    }
    if (narrar && h->n > 0) printf("    nenhum filho mais urgente -> PARA no índice %d\n", i);
    destaque = h->n > 0 ? i : -1;
    return topo;
}

/* ------------------------------------------------------------------------- */
/* Alternativas ingênuas, para comparar                                       */
/* ------------------------------------------------------------------------- */
/* Vetor desordenado: inserir no fim O(1); atender procura o máximo O(n). */
static Alarme desord_remove(Alarme *v, int *n) {
    int m = 0;
    for (int i = 1; i < *n; i++)
        if (mais_urgente(&v[i], &v[m])) m = i;
    Alarme r = v[m];
    v[m] = v[--*n];          /* tapa o buraco com o último: O(1) */
    return r;
}

/* Vetor ordenado (mais urgente no FIM): inserir desloca os menos urgentes
 * para abrir espaço, O(n); atender é tirar do fim, O(1). */
static void ord_insere(Alarme *v, int *n, Alarme a) {
    int i = *n;
    while (i > 0 && mais_urgente(&v[i - 1], &a)) {
        v[i] = v[i - 1];
        i--;
    }
    v[i] = a;
    (*n)++;
}

/* Gerador pseudoaleatório xorshift32 com semente fixa: resultados repetíveis. */
static uint32_t estado = 2463534242u;
static uint32_t aleatorio(void) {
    estado ^= estado << 13;
    estado ^= estado >> 17;
    estado ^= estado << 5;
    return estado;
}

/* ------------------------------------------------------------------------- */
int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    printf("=== HEAP BINÁRIA COMO FILA DE PRIORIDADE DE ALARMES ===\n");
    printf("Prioridade 1..999 (maior = mais urgente). Regra da max-heap: pai >= filhos.\n");
    printf("[v] marca o alarme que acabou de subir/descer.\n");
    pausar();

    static const struct { int prio; const char *tag; } chegada[] = {
        {300, "LT-140 nível baixo (aviso)"},
        {500, "TT-130 temperatura alta"},
        {200, "FT-110 vazão baixa (aviso)"},
        {700, "PT-120 pressão alta na caldeira"},
        {400, "M-160 falha de partida da bomba"},
        {950, "PB-101 EMERGÊNCIA acionada"},
        {600, "XV-150 válvula não abriu"},
        {800, "TT-130 temperatura MUITO alta"},
        {500, "AI-105 pH fora da faixa"},
        {100, "ZS-103 comporta em manutenção"},
    };
    const int qt = (int)(sizeof chegada / sizeof chegada[0]);

    Heap h;
    heap_inicia(&h, qt);
    printf("\n--- AVALANCHE DE ALARMES: inserções ---\n");
    for (int i = 0; i < qt; i++) {
        printf("\n>>> chega alarme %d: %s\n", chegada[i].prio, chegada[i].tag);
        heap_insere(&h, chegada[i].prio, chegada[i].tag);
        desenha(&h);
        pausar();
    }
    destaque = -1;
    printf("\nRepare: o vetor NÃO está ordenado (ex.: v[1] < v[2] é permitido); só a raiz\n");
    printf("é garantidamente a maior. Isso basta e custa bem menos que ordenar.\n");
    pausar();

    printf("\n--- OPERADOR ATENDE OS 4 MAIS URGENTES ---\n");
    for (int k = 0; k < 4; k++) {
        printf("\n>>> atender\n");
        Alarme a = heap_remove(&h);
        printf("    ATENDIDO: %d  %s\n", a.prioridade, a.tag);
        desenha(&h);
        pausar();
    }
    destaque = -1;
    printf("\nRestantes, na ordem em que seriam atendidos:\n");
    while (h.n > 0) {
        narrar = 0;
        Alarme a = heap_remove(&h);
        printf("    %3d  %s\n", a.prioridade, a.tag);
    }
    printf("    (os dois de prioridade 500 saem por ordem de chegada: desempate por seq)\n");
    free(h.v);
    pausar();

    /* ---- Comparação de custo: insere n alarmes e atende todos ---- */
    printf("\n=== CUSTO DE INSERIR n ALARMES E ATENDER TODOS (comparações) ===\n");
    printf("  %7s | %14s | %17s | %16s\n", "n", "heap", "vetor desordenado",
           "vetor ordenado");
    narrar = 0;
    const int tamanhos[] = {10, 100, 1000, 10000, 100000};
    const int LIMITE_INGENUO = 10000;   /* acima disso as versões O(n²) levariam bilhões de passos */
    for (int t = 0; t < 5; t++) {
        int n = tamanhos[t];
        int *prio = aloca((size_t)n * sizeof *prio);
        for (int i = 0; i < n; i++) prio[i] = 1 + (int)(aleatorio() % 999u);

        Heap hp;
        heap_inicia(&hp, n);
        comparacoes = 0;
        for (int i = 0; i < n; i++) heap_insere(&hp, prio[i], "");
        while (hp.n > 0) heap_remove(&hp);
        long long c_heap = comparacoes;
        free(hp.v);

        long long c_des, c_ord;
        int estimado = n > LIMITE_INGENUO;
        if (!estimado) {
            Alarme *v = aloca((size_t)n * sizeof *v);
            int m = 0;
            comparacoes = 0;
            for (int i = 0; i < n; i++) v[m++] = (Alarme){prio[i], seq_global++, ""};
            while (m > 0) desord_remove(v, &m);
            c_des = comparacoes;

            comparacoes = 0;
            m = 0;
            for (int i = 0; i < n; i++) ord_insere(v, &m, (Alarme){prio[i], seq_global++, ""});
            while (m > 0) m--;                  /* atender = tirar do fim, sem comparar */
            c_ord = comparacoes;
            free(v);
        } else {
            /* desordenado: (n-1) + (n-2) + ... + 0 = n(n-1)/2 comparações exatas;
             * ordenado: em média metade dos elementos é deslocada, ~n²/4. */
            c_des = (long long)n * (n - 1) / 2;
            c_ord = (long long)n * n / 4;
        }
        printf("  %7d | %14lld | %16lld%s | %15lld%s\n", n, c_heap, c_des,
               estimado ? "*" : " ", c_ord, estimado ? "*" : " ");
        free(prio);
    }
    printf("\n  * estimado pela fórmula (n(n-1)/2 e ~n²/4); medir levaria bilhões de passos.\n");
    printf("  A heap cresce como n·log n; as alternativas com vetor, como n².\n");
    return 0;
}
