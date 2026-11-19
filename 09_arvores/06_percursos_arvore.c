/*
 * ============================================================================
 * Arquivo : 06_percursos_arvore.c
 * Módulo  : 09 — Árvores                      (Ementa: Unidade 5.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver os quatro percursos de árvore binária executando passo a passo:
 *     pré-ordem (nó, esq, dir), em-ordem (esq, nó, dir), pós-ordem (esq, dir,
 *     nó) — com a PILHA DE CHAMADAS da recursão desenhada a cada visita — e
 *     por nível (largura), com a FILA desenhada a cada passo.
 *   Ao fim de cada percurso a árvore é redesenhada com a ordem de visita
 *   (rótulo "valor:ordem"). Em seguida, as aplicações de cada um:
 *     em-ordem   -> gera a lista ORDENADA (relatório sem chamar sort);
 *     pré-ordem  -> serializa e recria a árvore com a MESMA forma (backup);
 *     pós-ordem  -> libera a memória com segurança (filhos antes do pai);
 *     por nível  -> mostra a hierarquia camada por camada.
 *
 * EXEMPLO REAL
 *   Índice de tags de um CLP numa BST: o relatório de tags sai em ordem pela
 *   em-ordem; o backup da configuração é gravado em pré-ordem para recarregar
 *   igual; ao desligar o serviço, a pós-ordem devolve toda a memória.
 *
 * COMPLEXIDADE (n = número de nós, h = altura, L = maior largura de um nível)
 *   Percurso           Tempo    Espaço extra
 *   pré/em/pós-ordem   O(n)     O(h) pilha de recursão (O(n) se degenerada)
 *   por nível          O(n)     O(L) fila (até ~n/2 numa árvore cheia)
 *   recriar por pré    O(n·h)   reinserção nó a nó: O(n log n) se balanceada
 *   Por quê: todo percurso visita cada nó exatamente uma vez; o que muda é a
 *   ORDEM, decidida por onde fica o "visita" em relação às chamadas.
 *
 * COMO EXECUTAR
 *   ./run 06_percursos_arvore               (ENTER avança cada passo)
 *   ./run 06_percursos_arvore < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct No {
    int chave;
    int ordem_visita;     /* só para o desenho: em que passo o nó foi visitado */
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
/* Desenho por níveis (coluna = posição em-ordem × ESP)                       */
/* ------------------------------------------------------------------------- */
#define ESP 6
#define LARG 100
#define NIVEIS 9

static int mostrar_ordem = 0;   /* 1: rótulo "valor:ordem"; 0: só o valor */
static char tela[2 * NIVEIS][LARG + 1];

static void rotulo(const No *n, char *buf, size_t tam) {
    if (mostrar_ordem && n->ordem_visita > 0)
        snprintf(buf, tam, "%d:%d", n->chave, n->ordem_visita);
    else
        snprintf(buf, tam, "%d", n->chave);
}

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
        printf("    (árvore grande demais para o desenho)\n");
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
/* BST básica                                                                 */
/* ------------------------------------------------------------------------- */
static No *novo_no(int chave) {
    No *n = malloc(sizeof *n);
    if (n == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    n->chave = chave;
    n->ordem_visita = 0;
    n->esq = n->dir = NULL;
    return n;
}

static void insere(No **raiz, int chave) {
    No **p = raiz;
    while (*p != NULL) {
        if (chave == (*p)->chave) return;
        p = chave < (*p)->chave ? &(*p)->esq : &(*p)->dir;
    }
    *p = novo_no(chave);
}

static void zera_ordem(No *n) {
    if (n == NULL) return;
    n->ordem_visita = 0;
    zera_ordem(n->esq);
    zera_ordem(n->dir);
}

/* Duas árvores têm a mesma FORMA e as mesmas chaves? O(n). */
static int iguais(const No *a, const No *b) {
    if (a == NULL || b == NULL) return a == b;
    return a->chave == b->chave && iguais(a->esq, b->esq) && iguais(a->dir, b->dir);
}

/* ------------------------------------------------------------------------- */
/* Percursos em profundidade com a pilha de chamadas visível                  */
/* ------------------------------------------------------------------------- */
typedef enum { PRE_ORDEM, EM_ORDEM, POS_ORDEM } Tipo;

#define MAX_PILHA 64
static const No *pilha[MAX_PILHA];   /* espelho da pilha de chamadas da recursão */
static int topo = 0;

typedef struct {
    int *seq;      /* chaves na ordem de visita */
    int n;         /* quantas já foram visitadas */
} Saida;

/* Imprime a pilha de chamadas (base à esquerda, topo à direita) em largura fixa. */
static void mostra_pilha(void) {
    int usados = printf("pilha: ");
    for (int i = 0; i < topo; i++) usados += printf("%s%d", i ? " > " : "", pilha[i]->chave);
    printf("%*s", usados < 30 ? 30 - usados : 1, "");
}

/* "Visitar" = processar o nó. Aqui: anota a ordem e imprime o passo. */
static void visita(No *n, Saida *s) {
    s->seq[s->n++] = n->chave;
    n->ordem_visita = s->n;
    printf("    passo %2d | visita %3d | ", s->n, n->chave);
    mostra_pilha();
    printf("| saída:");
    for (int i = 0; i < s->n; i++) printf(" %d", s->seq[i]);
    printf("\n");
}

/*
 * Um único código para os três percursos: a ÚNICA diferença é a posição da
 * chamada visita() em relação às duas chamadas recursivas. O(n) tempo, O(h)
 * de pilha. Empilhar/desempilhar em `pilha[]` só serve para mostrar ao aluno
 * o que a própria recursão já guarda escondido na pilha do programa.
 */
static void percorre(No *n, Tipo t, Saida *s) {
    if (n == NULL) return;
    if (topo < MAX_PILHA) pilha[topo] = n;
    topo++;
    if (t == PRE_ORDEM) visita(n, s);   /* nó ANTES dos filhos  */
    percorre(n->esq, t, s);
    if (t == EM_ORDEM) visita(n, s);    /* nó ENTRE os filhos   */
    percorre(n->dir, t, s);
    if (t == POS_ORDEM) visita(n, s);   /* nó DEPOIS dos filhos */
    topo--;
}

static void demonstra_profundidade(No *raiz, Tipo t, int *seq) {
    static const char *nomes[] = {"PRÉ-ORDEM (nó, esq, dir)", "EM-ORDEM (esq, nó, dir)",
                                  "PÓS-ORDEM (esq, dir, nó)"};
    printf("\n=== %s ===\n", nomes[t]);
    zera_ordem(raiz);
    Saida s = {seq, 0};
    percorre(raiz, t, &s);
    printf("    árvore com a ordem de visita (valor:ordem):\n");
    mostrar_ordem = 1;
    desenha(raiz);
    mostrar_ordem = 0;
    pausar();
}

/* ------------------------------------------------------------------------- */
/* Percurso por nível com a fila visível                                      */
/* ------------------------------------------------------------------------- */
/*
 * BFS: tira o nó da FRENTE da fila, visita, põe os filhos no FIM. Como a fila
 * é FIFO, todo o nível k sai antes de qualquer nó do nível k+1. A fila é um
 * vetor de n posições (cada nó entra uma única vez). O(n) tempo e espaço.
 */
static void por_nivel(No *raiz) {
    printf("\n=== POR NÍVEL / LARGURA (fila FIFO) ===\n");
    if (raiz == NULL) return;
    int n = conta_nos(raiz);
    No **fila = malloc((size_t)n * sizeof *fila);
    int *nivel = malloc((size_t)n * sizeof *nivel);   /* nível de quem está na fila */
    if (fila == NULL || nivel == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    zera_ordem(raiz);
    int frente = 0, fim = 0, passo = 0, nivel_atual = -1;
    fila[fim] = raiz;
    nivel[fim++] = 0;
    printf("    início: enfileira a raiz %d\n", raiz->chave);
    while (frente < fim) {
        No *atual = fila[frente];
        int nv = nivel[frente++];
        atual->ordem_visita = ++passo;
        if (nv != nivel_atual) {
            nivel_atual = nv;
            printf("    -- nível %d --\n", nv);
        }
        printf("    passo %2d | desenfileira e visita %3d | enfileira:", passo, atual->chave);
        int enf = 0;
        if (atual->esq) { fila[fim] = atual->esq; nivel[fim++] = nv + 1; printf(" %d", atual->esq->chave); enf++; }
        if (atual->dir) { fila[fim] = atual->dir; nivel[fim++] = nv + 1; printf(" %d", atual->dir->chave); enf++; }
        printf("%*s| fila: frente->", enf == 2 ? 1 : enf == 1 ? 5 : 9, "");
        for (int i = frente; i < fim; i++) printf("[%d]", fila[i]->chave);
        printf("<-fim\n");
    }
    printf("    árvore com a ordem de visita (valor:ordem):\n");
    mostrar_ordem = 1;
    desenha(raiz);
    mostrar_ordem = 0;
    free(fila);
    free(nivel);
    pausar();
}

/* ------------------------------------------------------------------------- */
/* Aplicação: pós-ordem para liberar memória                                  */
/* ------------------------------------------------------------------------- */
/*
 * Por que PÓS-ordem? free(n) invalida n; depois dele, ler n->esq ou n->dir é
 * "uso após liberação" (comportamento indefinido — o sanitizer acusa). Na
 * pós-ordem os dois filhos já foram liberados quando chegamos ao pai, então
 * nunca precisamos ler um nó morto. Na PRÉ-ordem, liberaríamos o pai e em
 * seguida tentaríamos descer por ponteiros guardados dentro dele.
 */
static void libera_narrado(No *n, int *contador) {
    if (n == NULL) return;
    if (topo < MAX_PILHA) pilha[topo] = n;
    topo++;
    libera_narrado(n->esq, contador);
    libera_narrado(n->dir, contador);
    printf("    free(%3d) #%d | ", n->chave, ++*contador);
    mostra_pilha();
    printf("| os dois filhos já foram liberados\n");
    topo--;
    free(n);
}

static void libera(No *n) {
    if (n == NULL) return;
    libera(n->esq);
    libera(n->dir);
    free(n);
}

static void imprime_vetor(const char *titulo, const int *v, int n) {
    printf("    %s", titulo);
    for (int i = 0; i < n; i++) printf(" %d", v[i]);
    printf("\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    printf("=== PERCURSOS EM ÁRVORE BINÁRIA (índice de tags de um CLP) ===\n");
    const int tags[] = {50, 30, 70, 20, 40, 60, 80, 45};
    const int n = (int)(sizeof tags / sizeof tags[0]);
    No *raiz = NULL;
    for (int i = 0; i < n; i++) insere(&raiz, tags[i]);
    desenha(raiz);
    printf("    Regra de bolso: o nome diz quando o NÓ é visitado em relação aos filhos.\n");
    pausar();

    int *pre = malloc((size_t)n * sizeof *pre);
    int *em = malloc((size_t)n * sizeof *em);
    int *pos = malloc((size_t)n * sizeof *pos);
    if (pre == NULL || em == NULL || pos == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        return EXIT_FAILURE;
    }

    demonstra_profundidade(raiz, PRE_ORDEM, pre);
    demonstra_profundidade(raiz, EM_ORDEM, em);
    demonstra_profundidade(raiz, POS_ORDEM, pos);
    por_nivel(raiz);

    /* ---- Aplicação 1: em-ordem gera a lista ordenada ---- */
    printf("\n=== APLICAÇÃO 1: EM-ORDEM = RELATÓRIO ORDENADO ===\n");
    imprime_vetor("ordem de cadastro :", tags, n);
    imprime_vetor("em-ordem          :", em, n);
    int ordenado = 1;
    for (int i = 1; i < n; i++)
        if (em[i - 1] >= em[i]) ordenado = 0;
    printf("    crescente? %s — O(n), sem nenhum algoritmo de ordenação.\n",
           ordenado ? "SIM" : "NÃO");
    printf("    (inserir n chaves e ler em-ordem é o \"tree sort\": O(n log n) médio)\n");
    pausar();

    /* ---- Aplicação 2: pré-ordem preserva a forma ---- */
    printf("\n=== APLICAÇÃO 2: PRÉ-ORDEM = BACKUP QUE RECRIA A MESMA ÁRVORE ===\n");
    imprime_vetor("arquivo de backup (pré-ordem):", pre, n);
    No *copia = NULL;
    for (int i = 0; i < n; i++) insere(&copia, pre[i]);
    printf("    reinserindo nessa ordem (cada pai entra antes dos filhos):\n");
    desenha(copia);
    printf("    mesma forma da original? %s\n", iguais(raiz, copia) ? "SIM" : "NÃO");
    pausar();

    No *errada = NULL;
    for (int i = 0; i < n; i++) insere(&errada, em[i]);
    printf("    E se o backup fosse gravado em EM-ordem (já ordenado)?\n");
    desenha(errada);
    printf("    mesma forma? %s — virou uma \"lista\" de altura %d (ver 07_bst_degenerada).\n",
           iguais(raiz, errada) ? "SIM" : "NÃO", altura(errada));
    pausar();

    /* ---- Aplicação 3: pós-ordem libera a memória ---- */
    printf("\n=== APLICAÇÃO 3: PÓS-ORDEM = LIBERAR MEMÓRIA COM SEGURANÇA ===\n");
    int liberados = 0;
    libera_narrado(raiz, &liberados);
    raiz = NULL;   /* evita ponteiro pendurado: a árvore não existe mais */
    printf("    %d nós liberados; a raiz foi a ÚLTIMA (ninguém mais dependia dela).\n",
           liberados);

    libera(copia);
    libera(errada);
    free(pre);
    free(em);
    free(pos);
    return 0;
}
