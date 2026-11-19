/*
 * ============================================================================
 * Arquivo : 05_bst_remocao.c
 * Módulo  : 09 — Árvores                      (Ementa: Unidade 5.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a remoção de UM nó de uma BST nos três casos, sempre com o desenho
 *   ANTES e DEPOIS e a conferência de que a árvore continua ordenada:
 *     caso 1 — folha (0 filhos): o ponteiro do pai passa a NULL;
 *     caso 2 — um filho: o filho "sobe" e ocupa o lugar do nó removido;
 *     caso 3 — dois filhos: copia o SUCESSOR em-ordem (menor da subárvore
 *              direita) para o nó e remove o nó original do sucessor, que cai
 *              no caso 1 ou 2 porque nunca tem filho esquerdo.
 *
 *   Legenda: [v] caminho   <v> nó a remover   {v} sucessor escolhido
 *
 * EXEMPLO REAL
 *   Índice de ordens de produção (OP) abertas no MES da fábrica: quando uma OP
 *   é concluída, ela sai do índice, e as demais precisam continuar ordenadas e
 *   rápidas de encontrar.
 *
 * COMPLEXIDADE (n = número de OPs, h = altura)
 *   Operação              Melhor    Médio       Pior       Espaço extra
 *   localizar o nó        O(1)      O(log n)    O(n)       O(1)
 *   achar o sucessor      O(1)      O(log n)    O(n)       O(1)
 *   religar ponteiros     O(1)      O(1)        O(1)       O(1)
 *   remover (total)       O(1)      O(log n)    O(n)       O(1) (iterativa)
 *   Por quê: a busca do nó e a descida até o sucessor juntas percorrem no
 *   máximo um caminho raiz->folha, ou seja, h + 1 nós. Religar é constante.
 *
 * COMO EXECUTAR
 *   ./run 05_bst_remocao               (ENTER avança cada passo)
 *   ./run 05_bst_remocao < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct No {
    int chave;            /* número da ordem de produção */
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

/* ------------------------------------------------------------------------- */
/* Destaques e desenho por níveis (mesma técnica de 04_bst_insercao_busca.c)  */
/* ------------------------------------------------------------------------- */
#define MAX_CAMINHO 64
static const No *caminho[MAX_CAMINHO];
static int tam_caminho = 0;
static const No *alvo_desenho = NULL;      /* <v> */
static const No *sucessor_desenho = NULL;  /* {v} */

static void limpa_destaques(void) {
    tam_caminho = 0;
    alvo_desenho = sucessor_desenho = NULL;
}

static void marca_caminho(const No *n) {
    if (tam_caminho < MAX_CAMINHO) caminho[tam_caminho++] = n;
}

static int esta_no_caminho(const No *n) {
    for (int i = 0; i < tam_caminho; i++)
        if (caminho[i] == n) return 1;
    return 0;
}

static void rotulo(const No *n, char *buf, size_t tam) {
    if (n == alvo_desenho)          snprintf(buf, tam, "<%d>", n->chave);
    else if (n == sucessor_desenho) snprintf(buf, tam, "{%d}", n->chave);
    else if (esta_no_caminho(n))    snprintf(buf, tam, "[%d]", n->chave);
    else                            snprintf(buf, tam, "%d", n->chave);
}

#define ESP 6          /* colunas por nó: coluna = posição em-ordem × ESP */
#define LARG 100
#define NIVEIS 8

static char tela[2 * NIVEIS][LARG + 1];

static void escreve_centrado(int lin, int col, const char *txt) {
    int w = (int)strlen(txt);
    for (int k = 0; k < w; k++) {
        int c = col - w / 2 + k;
        if (c >= 0 && c < LARG) tela[lin][c] = txt[k];
    }
}

static int posiciona(const No *n, int nivel, int *ordem) {
    if (n == NULL) return -1;
    int ce = posiciona(n->esq, nivel + 1, ordem);
    int c = (*ordem)++ * ESP + ESP / 2;
    int cd = posiciona(n->dir, nivel + 1, ordem);
    char txt[16];
    rotulo(n, txt, sizeof txt);
    escreve_centrado(2 * nivel, c, txt);
    char *lig = tela[2 * nivel + 1];
    if (ce >= 0) { lig[ce] = '/';  for (int k = ce + 1; k < c; k++) lig[k] = '-'; }
    if (cd >= 0) { lig[cd] = '\\'; for (int k = c + 1; k < cd; k++) lig[k] = '-'; }
    if (ce >= 0 || cd >= 0) lig[c] = '+';
    return c;
}

static int conta_nos(const No *n) {
    return n == NULL ? 0 : 1 + conta_nos(n->esq) + conta_nos(n->dir);
}

static int altura(const No *n) {
    if (n == NULL) return -1;
    int he = altura(n->esq), hd = altura(n->dir);
    return 1 + (he > hd ? he : hd);
}

static void desenha(const No *raiz) {
    if (raiz == NULL) { printf("    (árvore vazia)\n"); return; }
    int h = altura(raiz);
    if (conta_nos(raiz) * ESP > LARG || h >= NIVEIS) {
        printf("    (árvore grande demais para o desenho: %d nós)\n", conta_nos(raiz));
        return;
    }
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
/* BST                                                                        */
/* ------------------------------------------------------------------------- */
static No *novo_no(int chave) {
    No *n = malloc(sizeof *n);
    if (n == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    n->chave = chave;
    n->esq = n->dir = NULL;
    return n;
}

/* Inserção iterativa sem narração (montagem inicial). O(h). */
static void insere(No **raiz, int chave) {
    No **p = raiz;
    while (*p != NULL) {
        if (chave == (*p)->chave) return;
        p = chave < (*p)->chave ? &(*p)->esq : &(*p)->dir;
    }
    *p = novo_no(chave);
}

/* Confere a propriedade da BST: todo nó estritamente entre `min` e `max`.
 * O(n). Armadilha: comparar só com os filhos diretos NÃO basta — um neto
 * pode violar a regra em relação ao avô. */
static int eh_bst(const No *n, long min, long max) {
    if (n == NULL) return 1;
    if (n->chave <= min || n->chave >= max) return 0;
    return eh_bst(n->esq, min, n->chave) && eh_bst(n->dir, n->chave, max);
}

static void em_ordem(const No *n) {
    if (n == NULL) return;
    em_ordem(n->esq);
    printf("%d ", n->chave);
    em_ordem(n->dir);
}

static void confere(const No *raiz) {
    printf("    em-ordem: ");
    em_ordem(raiz);
    printf("\n    propriedade da BST mantida? %s | nós: %d | altura: %d\n",
           eh_bst(raiz, LONG_MIN, LONG_MAX) ? "SIM" : "NÃO (bug!)",
           conta_nos(raiz), altura(raiz));
}

/*
 * Remove `chave` narrando cada passo. O(h).
 *
 * Versão iterativa com ponteiro duplo: `p` aponta para o CAMPO que guarda o
 * nó (a raiz, ou o `esq`/`dir` do pai). Trocar `*p` religa o pai ao novo
 * ocupante do lugar sem precisar de uma variável "pai" nem de casos especiais
 * para a raiz.
 */
static void remove_narrado(No **raiz, int chave) {
    int comps = 0;
    printf("\n>>> Concluir (remover) a OP %d\n", chave);

    /* Passo 1: localizar, como numa busca comum. */
    limpa_destaques();
    No **p = raiz;
    while (*p != NULL && (*p)->chave != chave) {
        comps++;
        marca_caminho(*p);
        printf("    compara %d %c %d -> %s\n", chave, chave < (*p)->chave ? '<' : '>',
               (*p)->chave, chave < (*p)->chave ? "esquerda" : "direita");
        p = chave < (*p)->chave ? &(*p)->esq : &(*p)->dir;
    }
    if (*p == NULL) {
        printf("    chegou em NULL: a OP %d não existe; nada a remover (%d comparações)\n",
               chave, comps);
        pausar();
        return;
    }
    comps++;
    No *alvo = *p;
    alvo_desenho = alvo;
    printf("    ANTES (achou <%d> com %d comparações):\n", chave, comps);
    desenha(*raiz);

    if (alvo->esq == NULL && alvo->dir == NULL) {
        printf("    CASO 1 — folha: basta o pai apontar para NULL e liberar o nó.\n");
        *p = NULL;
        free(alvo);
    } else if (alvo->esq == NULL || alvo->dir == NULL) {
        No *filho = alvo->esq != NULL ? alvo->esq : alvo->dir;
        printf("    CASO 2 — um filho (%d): o filho sobe e ocupa o lugar de %d.\n",
               filho->chave, chave);
        printf("    Por que funciona: toda a subárvore de %d já está do lado certo do pai.\n",
               filho->chave);
        *p = filho;
        free(alvo);
    } else {
        printf("    CASO 3 — dois filhos: não dá para pendurar os DOIS no pai.\n");
        printf("    Troca-se pelo SUCESSOR: o menor da subárvore direita (1 passo à\n");
        printf("    direita e depois tudo à esquerda). Ele é maior que toda a esquerda\n");
        printf("    e menor que o resto da direita, então a ordem continua valendo.\n");
        pausar();
        No **ps = &alvo->dir;                 /* campo que guarda o sucessor */
        marca_caminho(alvo);
        while ((*ps)->esq != NULL) {
            comps++;
            marca_caminho(*ps);
            printf("    %d tem filho esquerdo -> continua à esquerda\n", (*ps)->chave);
            ps = &(*ps)->esq;
        }
        comps++;
        No *suc = *ps;
        sucessor_desenho = suc;
        printf("    %d não tem filho esquerdo -> sucessor = {%d}\n", suc->chave, suc->chave);
        desenha(*raiz);
        pausar();
        printf("    copia %d para o nó de %d e remove o nó original de %d\n",
               suc->chave, chave, suc->chave);
        printf("    (ele cai no caso %d: %s)\n", suc->dir ? 2 : 1,
               suc->dir ? "tem só filho direito, que sobe" : "não tem filhos");
        alvo->chave = suc->chave;
        *ps = suc->dir;                       /* sucessor nunca tem filho esquerdo */
        free(suc);
    }

    limpa_destaques();
    printf("    DEPOIS (%d comparações no total):\n", comps);
    desenha(*raiz);
    confere(*raiz);
    pausar();
}

static void libera(No *n) {
    if (n == NULL) return;
    libera(n->esq);
    libera(n->dir);
    free(n);
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    printf("=== BST: REMOÇÃO NOS 3 CASOS (índice de ordens de produção) ===\n");
    printf("Legenda: [v] caminho   <v> nó a remover   {v} sucessor escolhido\n");

    const int ops[] = {500, 300, 700, 200, 400, 600, 800, 350, 450, 650};
    No *raiz = NULL;
    for (size_t i = 0; i < sizeof ops / sizeof ops[0]; i++) insere(&raiz, ops[i]);

    printf("\nOPs abertas:\n");
    desenha(raiz);
    confere(raiz);
    pausar();

    remove_narrado(&raiz, 450);   /* caso 1: folha                              */
    remove_narrado(&raiz, 600);   /* caso 2: só filho direito (650)             */
    remove_narrado(&raiz, 300);   /* caso 3: sucessor 350 (filho de 400)        */
    remove_narrado(&raiz, 500);   /* caso 3 na raiz: sucessor 650, dois níveis  */
    remove_narrado(&raiz, 999);   /* inexistente                                */

    printf("\nResumo: só UM nó sai por remoção; os descendentes permanecem na árvore.\n");
    printf("Alternativa equivalente ao sucessor: o PREDECESSOR (maior da esquerda).\n");

    libera(raiz);
    return 0;
}
