/*
 * ============================================================================
 * Arquivo : 07_bst_degenerada_vs_balanceada.c
 * Módulo  : 09 — Árvores                      (Ementa: Unidade 5.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar, com números, por que a ORDEM de inserção decide o desempenho de
 *   uma BST. As mesmas chaves são inseridas de três jeitos:
 *     degenerada  — em ordem crescente (cada nó só tem filho à direita);
 *     aleatória   — embaralhadas (sem nenhum cuidado de balanceamento);
 *     balanceada  — construída pelo elemento do meio (altura mínima possível).
 *   Primeiro um exemplo pequeno desenhado; depois uma tabela de altura e de
 *   comparações para n = 10, 100, 1000, 10000 e 100000: O(n) contra O(log n).
 *
 * EXEMPLO REAL
 *   Historiador de processo (data logger) que indexa amostras pelo timestamp.
 *   As amostras chegam SEMPRE em ordem crescente de tempo: uma BST ingênua vira
 *   uma lista encadeada, e buscar "o valor às 10:32" fica tão lento quanto
 *   varrer tudo. É exatamente o problema que a AVL resolve (08_avl_rotacoes.c).
 *
 * COMPLEXIDADE (n = número de amostras, h = altura)
 *   Árvore        Altura h          Busca (pior)   Inserir n chaves (total)
 *   degenerada    n - 1             O(n)           O(n²)   = n(n-1)/2 comparações
 *   aleatória     ≈ 2,99·log2 n (*) O(log n) médio O(n log n) médio
 *   balanceada    ⌊log2 n⌋          O(log n)       O(n) construindo pelo meio
 *   (*) a altura esperada de uma BST aleatória é ≈ 4,31·ln n ≈ 2,99·log2 n;
 *       a profundidade MÉDIA de um nó é ≈ 1,39·log2 n.
 *   Por quê: o custo de buscar é o número de níveis descidos. Na degenerada
 *   cada nível tem 1 nó (n níveis); na balanceada cada nível dobra de tamanho
 *   (log2 n níveis).
 *
 * COMO EXECUTAR
 *   ./run 07_bst_degenerada_vs_balanceada               (ENTER avança)
 *   ./run 07_bst_degenerada_vs_balanceada < /dev/null   (direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct No {
    int chave;            /* timestamp da amostra (segundos) */
    struct No *esq;
    struct No *dir;
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

static void *aloca(size_t bytes) {
    void *p = malloc(bytes);
    if (p == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

static No *novo_no(int chave) {
    No *n = aloca(sizeof *n);
    n->chave = chave;
    n->esq = n->dir = NULL;
    return n;
}

/* ------------------------------------------------------------------------- */
/* Operações SEM recursão: uma árvore degenerada com 100000 nós teria 100000  */
/* chamadas recursivas empilhadas e poderia estourar a pilha do programa.     */
/* ------------------------------------------------------------------------- */

/* Inserção iterativa; soma em *comps as comparações feitas. O(h). */
static void insere(No **raiz, int chave, long long *comps) {
    No **p = raiz;
    while (*p != NULL) {
        (*comps)++;
        if (chave == (*p)->chave) return;
        p = chave < (*p)->chave ? &(*p)->esq : &(*p)->dir;
    }
    *p = novo_no(chave);
}

/* Comparações para encontrar `chave` (= nós visitados). O(h). */
static int custo_busca(const No *n, int chave) {
    int c = 0;
    while (n != NULL) {
        c++;
        if (chave == n->chave) break;
        n = chave < n->chave ? n->esq : n->dir;
    }
    return c;
}

/* Altura por largura (BFS nível a nível), sem recursão. O(n) tempo e espaço. */
static int altura(const No *raiz, int n) {
    if (raiz == NULL) return -1;
    const No **fila = aloca((size_t)n * sizeof *fila);
    int frente = 0, fim = 0, h = -1;
    fila[fim++] = raiz;
    while (frente < fim) {
        int fim_nivel = fim;           /* tudo até aqui pertence ao mesmo nível */
        h++;
        while (frente < fim_nivel) {
            const No *a = fila[frente++];
            if (a->esq) fila[fim++] = a->esq;
            if (a->dir) fila[fim++] = a->dir;
        }
    }
    free(fila);
    return h;
}

/*
 * Libera sem recursão nem pilha auxiliar: enquanto a raiz tiver filho
 * esquerdo, faz uma rotação à direita (o filho sobe); quando não tiver, libera
 * a raiz e segue pela direita. Cada nó sofre no máximo uma rotação: O(n).
 */
static void libera(No *raiz) {
    while (raiz != NULL) {
        if (raiz->esq != NULL) {
            No *e = raiz->esq;
            raiz->esq = e->dir;
            e->dir = raiz;
            raiz = e;
        } else {
            No *d = raiz->dir;
            free(raiz);
            raiz = d;
        }
    }
}

/* Constrói a BST de altura mínima a partir do vetor ORDENADO v[ini..fim]:
 * o elemento do meio vira raiz, e cada metade vira uma subárvore. O(n);
 * recursão de profundidade só log2 n. */
static No *constroi_balanceada(const int *v, int ini, int fim) {
    if (ini > fim) return NULL;
    int meio = ini + (fim - ini) / 2;   /* evita overflow de (ini+fim)/2 */
    No *n = novo_no(v[meio]);
    n->esq = constroi_balanceada(v, ini, meio - 1);
    n->dir = constroi_balanceada(v, meio + 1, fim);
    return n;
}

/* Gerador pseudoaleatório xorshift32 com semente fixa: resultados repetíveis. */
static uint32_t estado = 2463534242u;
static uint32_t aleatorio(void) {
    estado ^= estado << 13;
    estado ^= estado >> 17;
    estado ^= estado << 5;
    return estado;
}

/* Embaralhamento de Fisher-Yates: O(n), todas as permutações equiprováveis. */
static void embaralha(int *v, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = (int)(aleatorio() % (uint32_t)(i + 1));
        int t = v[i];
        v[i] = v[j];
        v[j] = t;
    }
}

/* ------------------------------------------------------------------------- */
/* Desenho por níveis (coluna = posição em-ordem × ESP)                       */
/* ------------------------------------------------------------------------- */
#define ESP 6
#define LARG 100
#define NIVEIS 10

static char tela[2 * NIVEIS][LARG + 1];

static int posiciona(const No *n, int nivel, int *ordem) {
    if (n == NULL) return -1;
    int ce = posiciona(n->esq, nivel + 1, ordem);
    int c = (*ordem)++ * ESP + ESP / 2;
    int cd = posiciona(n->dir, nivel + 1, ordem);
    char txt[16];
    snprintf(txt, sizeof txt, "%d", n->chave);
    int w = (int)strlen(txt);
    for (int k = 0; k < w; k++) tela[2 * nivel][c - w / 2 + k] = txt[k];
    char *lig = tela[2 * nivel + 1];
    if (ce >= 0) { lig[ce] = '/';  for (int k = ce + 1; k < c; k++) lig[k] = '-'; }
    if (cd >= 0) { lig[cd] = '\\'; for (int k = c + 1; k < cd; k++) lig[k] = '-'; }
    if (ce >= 0 || cd >= 0) lig[c] = '+';
    return c;
}

/* Pré-condição: n * ESP <= LARG e altura < NIVEIS (exemplo pequeno). */
static void desenha(const No *raiz, int n) {
    int h = altura(raiz, n);
    for (int l = 0; l < 2 * NIVEIS; l++) {
        memset(tela[l], ' ', LARG);
        tela[l][LARG] = '\0';
    }
    int ordem = 0;
    posiciona(raiz, 0, &ordem);
    for (int l = 0; l <= 2 * h; l++) {
        int fim = LARG;
        while (fim > 0 && tela[l][fim - 1] == ' ') fim--;
        printf("    %.*s\n", fim, tela[l]);
    }
}

/* ------------------------------------------------------------------------- */
/* Parte 1: exemplo pequeno, desenhado                                        */
/* ------------------------------------------------------------------------- */
static void exemplo_pequeno(void) {
    enum { N = 9 };
    int ts[N];
    for (int i = 0; i < N; i++) ts[i] = 1000 + i;   /* t = 1000 s, 1001 s, ... */

    printf("=== PARTE 1: 9 AMOSTRAS DO HISTORIADOR, TIMESTAMPS 1000..1008 ===\n");

    No *deg = NULL;
    long long cd = 0;
    for (int i = 0; i < N; i++) insere(&deg, ts[i], &cd);
    printf("\n[A] Inseridas na ordem de chegada (crescente) -> DEGENERADA:\n");
    desenha(deg, N);
    printf("    altura %d | inserir tudo: %lld comparações | buscar 1008: %d comparações\n",
           altura(deg, N), cd, custo_busca(deg, 1008));
    printf("    Cada nova amostra é maior que todas: desce sempre à direita até o fim.\n");
    pausar();

    int emb[N];
    memcpy(emb, ts, sizeof emb);
    embaralha(emb, N);
    No *ale = NULL;
    long long ca = 0;
    for (int i = 0; i < N; i++) insere(&ale, emb[i], &ca);
    printf("\n[B] As mesmas amostras embaralhadas (");
    for (int i = 0; i < N; i++) printf("%s%d", i ? " " : "", emb[i]);
    printf(") -> ALEATÓRIA:\n");
    desenha(ale, N);
    printf("    altura %d | inserir tudo: %lld comparações | buscar 1008: %d comparações\n",
           altura(ale, N), ca, custo_busca(ale, 1008));
    pausar();

    No *bal = constroi_balanceada(ts, 0, N - 1);
    printf("\n[C] Construída pelo meio (o do meio vira raiz, recursivamente) -> BALANCEADA:\n");
    desenha(bal, N);
    printf("    altura %d (= ⌊log2 %d⌋) | buscar 1008: %d comparações\n",
           altura(bal, N), N, custo_busca(bal, 1008));
    printf("    Problema: construir pelo meio exige ter TODOS os dados antes. O historiador\n");
    printf("    recebe uma amostra por vez — por isso existem árvores autobalanceadas (AVL).\n");
    pausar();

    libera(deg);
    libera(ale);
    libera(bal);
}

/* ------------------------------------------------------------------------- */
/* Parte 2: tabela de crescimento                                             */
/* ------------------------------------------------------------------------- */
#define LIMITE_INSERCAO 10000   /* acima disso, inserir em ordem custaria bilhões de passos */
#define AMOSTRAS_BUSCA 1000     /* buscas usadas para medir o custo médio */

typedef struct {
    int altura;
    int busca_pior;          /* comparações para achar a chave n-1 (a mais recente) */
    double busca_media;      /* média em AMOSTRAS_BUSCA chaves espaçadas    */
    long long insercao;      /* comparações para inserir todas as chaves    */
} Medida;

static Medida mede(const No *raiz, int n, long long insercao) {
    Medida m;
    m.altura = altura(raiz, n);
    m.busca_pior = custo_busca(raiz, n - 1);
    int s = n < AMOSTRAS_BUSCA ? n : AMOSTRAS_BUSCA;
    long long soma = 0;
    for (int i = 0; i < s; i++) soma += custo_busca(raiz, (int)((long long)i * n / s));
    m.busca_media = (double)soma / s;
    m.insercao = insercao;
    return m;
}

static void tabela(void) {
    const int tamanhos[] = {10, 100, 1000, 10000, 100000};
    const int qt = (int)(sizeof tamanhos / sizeof tamanhos[0]);
    Medida md[5], ma[5], mb[5];
    int estimado[5];

    for (int t = 0; t < qt; t++) {
        int n = tamanhos[t];
        int *v = aloca((size_t)n * sizeof *v);
        for (int i = 0; i < n; i++) v[i] = i;            /* chaves 0..n-1 = timestamps */

        /* Degenerada: inserção em ordem. Para n grande, a árvore resultante é
         * montada direto como uma "lista" pela direita (é exatamente a mesma
         * forma) e o custo da inserção vem da fórmula n(n-1)/2. */
        No *deg = NULL;
        long long cd = 0;
        estimado[t] = n > LIMITE_INSERCAO;
        if (!estimado[t]) {
            for (int i = 0; i < n; i++) insere(&deg, v[i], &cd);
        } else {
            No **p = &deg;
            for (int i = 0; i < n; i++) { *p = novo_no(v[i]); p = &(*p)->dir; }
            cd = (long long)n * (n - 1) / 2;
        }
        md[t] = mede(deg, n, cd);
        libera(deg);

        No *bal = constroi_balanceada(v, 0, n - 1);
        mb[t] = mede(bal, n, 0);
        libera(bal);

        embaralha(v, n);
        No *ale = NULL;
        long long ca = 0;
        for (int i = 0; i < n; i++) insere(&ale, v[i], &ca);
        ma[t] = mede(ale, n, ca);
        libera(ale);
        free(v);
    }

    printf("\n=== PARTE 2: CRESCIMENTO (chaves 0..n-1) ===\n\n");
    printf("  ALTURA e COMPARAÇÕES PARA ACHAR A AMOSTRA MAIS RECENTE (chave n-1)\n");
    printf("  (na degenerada essa é a pior busca possível: a chave está no fim da \"lista\")\n");
    printf("  %7s | %-26s | %-26s |\n", "", "        altura h", "   busca da chave n-1");
    printf("  %7s | %8s %8s %8s | %8s %8s %8s | %6s\n", "n", "degen.", "aleat.", "balanc.",
           "degen.", "aleat.", "balanc.", "log2 n");
    for (int t = 0; t < qt; t++)
        printf("  %7d | %8d %8d %8d | %8d %8d %8d | %6.1f\n", tamanhos[t],
               md[t].altura, ma[t].altura, mb[t].altura, md[t].busca_pior,
               ma[t].busca_pior, mb[t].busca_pior, log2((double)tamanhos[t]));
    pausar();

    printf("\n  BUSCA MÉDIA E CUSTO TOTAL PARA INSERIR AS n AMOSTRAS (em comparações)\n");
    printf("          |      média por busca       | inserir todas as chaves\n");
    printf("  %7s | %8s %8s %8s | %16s %15s\n", "n", "degen.", "aleat.", "balanc.",
           "degen.", "aleat.");
    for (int t = 0; t < qt; t++)
        printf("  %7d | %8.1f %8.1f %8.1f | %15lld%s %15lld\n", tamanhos[t],
               md[t].busca_media, ma[t].busca_media, mb[t].busca_media,
               md[t].insercao, estimado[t] ? "*" : " ", ma[t].insercao);
    printf("  (balanceada construída pelo meio: O(n), sem comparações de chave)\n");
    printf("\n  * n(n-1)/2 calculado pela fórmula: inserir 100000 amostras em ordem custaria\n");
    printf("    ~5 bilhões de comparações (a árvore foi montada direto, com a mesma forma).\n");
    printf("\n  Leitura: com 10x mais amostras, a degenerada fica 10x mais lenta por busca\n");
    printf("  (O(n)); a balanceada ganha só ~3,3 comparações (O(log n)). A aleatória fica\n");
    printf("  perto da balanceada, mas dados de processo quase nunca chegam embaralhados.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    exemplo_pequeno();
    tabela();
    return 0;
}
