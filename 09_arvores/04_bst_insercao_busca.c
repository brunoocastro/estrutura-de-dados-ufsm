/*
 * ============================================================================
 * Arquivo : 04_bst_insercao_busca.c
 * Módulo  : 09 — Árvores                      (Ementa: Unidade 5.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a árvore binária de busca (BST) sendo construída nó a nó. A cada
 *   inserção o programa narra as comparações ("350 < 500 -> esquerda"), desenha
 *   a árvore por níveis com o CAMINHO percorrido destacado e conta quantas
 *   comparações foram feitas. Depois busca códigos (existentes e ausentes) e
 *   compara com a busca sequencial em um vetor desordenado.
 *
 *   Legenda do desenho:  [v] nó comparado no caminho   <v> nó inserido/achado
 *
 * EXEMPLO REAL
 *   Almoxarifado de manutenção: os códigos das peças de reposição chegam em
 *   ordem qualquer. A BST mantém o catálogo sempre ordenado e permite achar
 *   uma peça olhando poucos nós, sem reordenar nada a cada cadastro.
 *
 * COMPLEXIDADE (n = número de peças, h = altura da árvore)
 *   Operação         Melhor    Médio       Pior       Espaço extra
 *   inserir          O(1)      O(log n)    O(n)       O(1) (versão iterativa)
 *   buscar           O(1)      O(log n)    O(n)       O(1)
 *   mínimo/máximo    O(1)      O(log n)    O(n)       O(1)
 *   busca sequencial O(1)      O(n)        O(n)       O(1)
 *   Por quê: cada comparação desce UM nível e descarta a outra subárvore
 *   inteira; o custo é o número de níveis percorridos, no máximo h + 1. Com
 *   dados em ordem aleatória h fica perto de log2 n; com dados já ordenados a
 *   árvore degenera (ver 07_bst_degenerada_vs_balanceada.c).
 *
 * COMO EXECUTAR
 *   ./run 04_bst_insercao_busca               (ENTER avança; no fim há um menu)
 *   ./run 04_bst_insercao_busca < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct No {
    int chave;            /* código da peça */
    struct No *esq;       /* subárvore com códigos MENORES */
    struct No *dir;       /* subárvore com códigos MAIORES */
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
/* Destaques usados pelo desenho                                              */
/* ------------------------------------------------------------------------- */
#define MAX_CAMINHO 64
static const No *caminho[MAX_CAMINHO];  /* nós comparados na última operação */
static int tam_caminho = 0;
static const No *destaque = NULL;       /* nó inserido ou encontrado         */

static void limpa_destaques(void) {
    tam_caminho = 0;
    destaque = NULL;
}

static void marca_caminho(const No *n) {
    if (tam_caminho < MAX_CAMINHO) caminho[tam_caminho++] = n;
}

static int esta_no_caminho(const No *n) {
    for (int i = 0; i < tam_caminho; i++)
        if (caminho[i] == n) return 1;
    return 0;
}

/* Texto que aparece no desenho para o nó `n`. */
static void rotulo(const No *n, char *buf, size_t tam) {
    if (n == destaque)          snprintf(buf, tam, "<%d>", n->chave);
    else if (esta_no_caminho(n)) snprintf(buf, tam, "[%d]", n->chave);
    else                        snprintf(buf, tam, "%d", n->chave);
}

/* ------------------------------------------------------------------------- */
/* Desenho da árvore por níveis                                               */
/* ------------------------------------------------------------------------- */
/*
 * Ideia: a coluna de cada nó é a sua posição no percurso EM-ORDEM (esquerda,
 * nó, direita) vezes ESP. Como a em-ordem de uma BST é crescente, cada nó fica
 * à direita de toda a sua subárvore esquerda e à esquerda da direita: ninguém
 * se sobrepõe. A linha é 2 × nível (a linha ímpar guarda as ligações).
 */
#define ESP 6          /* colunas reservadas para cada nó */
#define LARG 100       /* largura máxima do desenho       */
#define NIVEIS 8       /* níveis máximos desenhados       */

static char tela[2 * NIVEIS][LARG + 1];

static void escreve_centrado(int lin, int col, const char *txt) {
    int w = (int)strlen(txt);
    for (int k = 0; k < w; k++) {
        int c = col - w / 2 + k;
        if (c >= 0 && c < LARG) tela[lin][c] = txt[k];
    }
}

/* Posiciona o nó e seus descendentes na tela; devolve a coluna do nó. O(n). */
static int posiciona(const No *n, int nivel, int *ordem) {
    if (n == NULL) return -1;
    int ce = posiciona(n->esq, nivel + 1, ordem);
    int c = (*ordem)++ * ESP + ESP / 2;
    int cd = posiciona(n->dir, nivel + 1, ordem);

    char txt[16];
    rotulo(n, txt, sizeof txt);
    escreve_centrado(2 * nivel, c, txt);

    char *lig = tela[2 * nivel + 1];    /* linha das ligações abaixo do nó */
    if (ce >= 0) { lig[ce] = '/';  for (int k = ce + 1; k < c; k++) lig[k] = '-'; }
    if (cd >= 0) { lig[cd] = '\\'; for (int k = c + 1; k < cd; k++) lig[k] = '-'; }
    if (ce >= 0 || cd >= 0) lig[c] = '+';
    return c;
}

static int conta_nos(const No *n) {
    return n == NULL ? 0 : 1 + conta_nos(n->esq) + conta_nos(n->dir);
}

/* Altura: arestas do caminho mais longo até uma folha (vazia = -1). O(n). */
static int altura(const No *n) {
    if (n == NULL) return -1;
    int he = altura(n->esq), hd = altura(n->dir);
    return 1 + (he > hd ? he : hd);
}

/* Desenho "deitado" (raiz à esquerda, subárvore direita em cima): usado
 * quando a árvore é grande demais para o desenho por níveis. */
static void desenha_deitada(const No *n, int nivel) {
    if (n == NULL) return;
    desenha_deitada(n->dir, nivel + 1);
    char txt[16];
    rotulo(n, txt, sizeof txt);
    printf("    %*s%s\n", nivel * 6, "", txt);
    desenha_deitada(n->esq, nivel + 1);
}

static void desenha(const No *raiz) {
    if (raiz == NULL) { printf("    (árvore vazia)\n"); return; }
    int h = altura(raiz);
    if (conta_nos(raiz) * ESP > LARG || h >= NIVEIS) {
        printf("    (árvore grande: desenho deitado, raiz à esquerda e direita em cima)\n");
        desenha_deitada(raiz, 0);
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
        while (fim > 0 && tela[l][fim - 1] == ' ') fim--;   /* corta espaços à direita */
        printf("    %.*s\n", fim, tela[l]);
    }
}

/* ------------------------------------------------------------------------- */
/* Operações da BST                                                           */
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

/*
 * Insere `chave` e devolve o número de comparações de chave (= nós visitados).
 * O(h). Versão iterativa com ponteiro duplo: `p` aponta para o CAMPO (a raiz,
 * um `esq` ou um `dir`) onde o novo nó será pendurado. Assim não é preciso
 * guardar o pai nem tratar a árvore vazia como caso especial.
 * Armadilha: com `No *p` (ponteiro simples) a gente acharia o NULL, mas não
 * teria como alterar o campo do pai que o contém.
 */
static int insere(No **raiz, int chave, int narrar) {
    int comps = 0;
    limpa_destaques();
    No **p = raiz;
    while (*p != NULL) {
        No *atual = *p;
        comps++;
        marca_caminho(atual);
        if (chave < atual->chave) {
            if (narrar) printf("    compara %d < %d  -> desce à ESQUERDA\n", chave, atual->chave);
            p = &atual->esq;
        } else if (chave > atual->chave) {
            if (narrar) printf("    compara %d > %d  -> desce à DIREITA\n", chave, atual->chave);
            p = &atual->dir;
        } else {
            if (narrar) printf("    %d == %d  -> código já cadastrado, nada a fazer\n",
                               chave, atual->chave);
            destaque = atual;
            return comps;
        }
    }
    *p = novo_no(chave);
    destaque = *p;
    if (narrar) printf("    encontrou NULL -> pendura <%d> nesse lugar\n", chave);
    return comps;
}

/* Busca narrada. Devolve o nó (ou NULL) e escreve em *comps as comparações. O(h). */
static const No *busca(const No *raiz, int chave, int *comps, int narrar) {
    *comps = 0;
    limpa_destaques();
    while (raiz != NULL) {
        (*comps)++;
        if (chave == raiz->chave) {
            if (narrar) printf("    compara %d == %d -> ACHOU\n", chave, raiz->chave);
            destaque = raiz;
            return raiz;
        }
        marca_caminho(raiz);
        if (chave < raiz->chave) {
            if (narrar) printf("    compara %d < %d  -> esquerda (descarta %d e toda a direita)\n",
                               chave, raiz->chave, raiz->chave);
            raiz = raiz->esq;
        } else {
            if (narrar) printf("    compara %d > %d  -> direita (descarta %d e toda a esquerda)\n",
                               chave, raiz->chave, raiz->chave);
            raiz = raiz->dir;
        }
    }
    if (narrar) printf("    chegou em NULL -> o código NÃO existe (não é preciso olhar o resto)\n");
    return NULL;
}

/* Busca sequencial em vetor (alternativa ingênua). O(n). */
static int busca_sequencial(const int *v, int n, int chave, int *comps) {
    *comps = 0;
    for (int i = 0; i < n; i++) {
        (*comps)++;
        if (v[i] == chave) return i;
    }
    return -1;
}

/* Em-ordem: imprime os códigos em ordem crescente. O(n). */
static void em_ordem(const No *n) {
    if (n == NULL) return;
    em_ordem(n->esq);
    printf("%d ", n->chave);
    em_ordem(n->dir);
}

/* Soma das profundidades (+1) de todos os nós = total de comparações para
 * buscar cada chave uma vez. Usada para a média. O(n). */
static long soma_custos(const No *n, int prof) {
    if (n == NULL) return 0;
    return prof + 1 + soma_custos(n->esq, prof + 1) + soma_custos(n->dir, prof + 1);
}

/* Libera em pós-ordem: os dois filhos antes do pai. O(n). */
static void libera(No *n) {
    if (n == NULL) return;
    libera(n->esq);
    libera(n->dir);
    free(n);
}

/* ------------------------------------------------------------------------- */
static void demonstra_busca(const No *raiz, const int *vetor, int n, int chave) {
    int cb, cs;
    printf("\n>>> Buscar a peça %d\n", chave);
    const No *achou = busca(raiz, chave, &cb, 1);
    busca_sequencial(vetor, n, chave, &cs);
    desenha(raiz);
    printf("    resultado: %s | BST: %d comparações | vetor sequencial: %d comparações\n",
           achou ? "encontrada" : "inexistente", cb, cs);
    pausar();
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    /* Códigos na ordem em que as peças foram cadastradas (ordem "aleatória"). */
    const int chegada[] = {500, 300, 700, 200, 400, 600, 800, 350, 650, 450, 750};
    const int n = (int)(sizeof chegada / sizeof chegada[0]);

    printf("=== BST: INSERÇÃO E BUSCA PASSO A PASSO (catálogo de peças) ===\n");
    printf("Regra: para todo nó, a esquerda guarda códigos MENORES e a direita MAIORES.\n");
    printf("Legenda: [v] nó comparado no caminho   <v> nó inserido/achado\n");
    pausar();

    No *raiz = NULL;
    for (int i = 0; i < n; i++) {
        printf("\n>>> Inserir a peça %d  (%d/%d)\n", chegada[i], i + 1, n);
        int c = insere(&raiz, chegada[i], 1);
        desenha(raiz);
        printf("    comparações: %d | nós: %d | altura: %d\n",
               c, conta_nos(raiz), altura(raiz));
        pausar();
    }

    printf("\nA em-ordem (esq, nó, dir) lista o catálogo ORDENADO sem nunca ordenar:\n    ");
    em_ordem(raiz);
    printf("\n");
    pausar();

    demonstra_busca(raiz, chegada, n, 650);   /* existe, no fundo da árvore */
    demonstra_busca(raiz, chegada, n, 420);   /* não existe                */
    demonstra_busca(raiz, chegada, n, 750);   /* último cadastrado          */

    /* Mínimo e máximo: descer sempre para um lado. */
    limpa_destaques();
    const No *m = raiz;
    while (m->esq != NULL) { marca_caminho(m); m = m->esq; }
    destaque = m;
    printf("\n>>> Menor código: desce sempre à esquerda\n");
    desenha(raiz);
    printf("    mínimo = %d\n", m->chave);
    limpa_destaques();
    m = raiz;
    while (m->dir != NULL) { marca_caminho(m); m = m->dir; }
    destaque = m;
    printf("\n>>> Maior código: desce sempre à direita\n");
    desenha(raiz);
    printf("    máximo = %d\n", m->chave);
    pausar();

    /* Custo médio: buscar cada peça uma vez. */
    int nos = conta_nos(raiz);
    printf("\n=== CUSTO MÉDIO DE UMA BUSCA BEM-SUCEDIDA (%d peças) ===\n", nos);
    printf("    BST (altura %d)       : %.2f comparações\n",
           altura(raiz), (double)soma_custos(raiz, 0) / nos);
    printf("    vetor desordenado    : %.2f comparações  ((n+1)/2)\n", (nos + 1) / 2.0);
    printf("    Com 11 peças a diferença é pequena; com 1 milhão seriam ~20 contra ~500000.\n");
    pausar();

    /* Menu livre: termina em EOF (Ctrl+D) ou com 'q'. */
    printf("\n=== EXPLORE: i <código> insere | b <código> busca | q sai ===\n");
    for (;;) {
        printf("> ");
        fflush(stdout);
        char cmd;
        int chave;
        if (scanf(" %c", &cmd) != 1 || cmd == 'q') break;
        if (scanf("%d", &chave) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) { }   /* descarta a linha inválida */
            if (c == EOF) break;
            printf("    uso: i 420  ou  b 420\n");
            continue;
        }
        if (cmd == 'i') {
            int c = insere(&raiz, chave, 1);
            desenha(raiz);
            printf("    comparações: %d | altura: %d\n", c, altura(raiz));
        } else if (cmd == 'b') {
            int c;
            const No *r = busca(raiz, chave, &c, 1);
            desenha(raiz);
            printf("    %s em %d comparações\n", r ? "encontrada" : "inexistente", c);
        } else {
            printf("    comando desconhecido\n");
        }
    }
    printf("\nfim.\n");

    libera(raiz);
    return 0;
}
