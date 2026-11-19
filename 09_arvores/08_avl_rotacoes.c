/*
 * ============================================================================
 * Arquivo : 08_avl_rotacoes.c
 * Módulo  : 09 — Árvores                      (Ementa: Unidade 5.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a árvore AVL se rebalanceando. Cada nó é desenhado como "id(fb)", onde
 *   fb = altura(esq) - altura(dir) é o FATOR DE BALANCEAMENTO. A AVL exige
 *   fb ∈ {-1, 0, +1} em todo nó; quando uma inserção deixa algum nó com +2 ou
 *   -2 (marcado com "!"), uma ROTAÇÃO conserta a altura em O(1):
 *     LL -> rotação simples à direita        RR -> rotação simples à esquerda
 *     LR -> esquerda no filho + direita      RL -> direita no filho + esquerda
 *   Cada caso é desenhado ANTES e DEPOIS. Depois, um índice de tags SCADA
 *   cadastradas com IDs crescentes, e uma tabela BST x AVL até n = 100000.
 *
 * EXEMPLO REAL
 *   Índice de tags de um SCADA por ID numérico: os IDs são atribuídos em ordem
 *   crescente à medida que os instrumentos são comissionados. Uma BST comum
 *   degeneraria (07_bst_degenerada_vs_balanceada.c); a AVL mantém a busca do
 *   tag em ~log2 n comparações mesmo assim. Bancos de dados, std::map (C++) e
 *   o escalonador do Linux usam árvores balanceadas pelo mesmo motivo.
 *
 * COMPLEXIDADE (n = número de tags)
 *   Operação         Melhor    Médio       Pior        Espaço extra
 *   buscar           O(1)      O(log n)    O(log n)    O(1)
 *   inserir          O(log n)  O(log n)    O(log n)    O(log n) recursão
 *   uma rotação      O(1)      O(1)        O(1)        O(1)
 *   Por quê: a regra |fb| <= 1 garante altura h < 1,44·log2(n + 2); inserir
 *   desce h níveis, sobe atualizando alturas e faz no máximo UMA rotação
 *   (simples ou dupla), que mexe só em 3 ponteiros e 2 alturas.
 *
 * COMO EXECUTAR
 *   ./run 08_avl_rotacoes               (ENTER avança cada passo)
 *   ./run 08_avl_rotacoes < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct No {
    int id;               /* chave: ID numérico do tag                    */
    const char *tag;      /* descrição (aponta para literal; não liberar) */
    int altura;           /* altura da subárvore deste nó (folha = 0)     */
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
/* Estado da demonstração                                                     */
/* ------------------------------------------------------------------------- */
static int narrar = 1;                 /* 0 na tabela de desempenho              */
static No **ref_raiz = NULL;           /* endereço da variável raiz, p/ desenhar */
static const No *desbalanceado = NULL; /* nó marcado com "!" no desenho          */
static long rot_ll, rot_rr, rot_lr, rot_rl;   /* contadores de rotações          */
static long comparacoes;               /* comparações de chave nas inserções    */

/* ------------------------------------------------------------------------- */
/* Alturas e fator de balanceamento                                           */
/* ------------------------------------------------------------------------- */
/* Altura guardada no nó: O(1). Convenção: vazia = -1, folha = 0. */
static int h(const No *n) { return n != NULL ? n->altura : -1; }

static int maior(int a, int b) { return a > b ? a : b; }

/* Recalcula a altura a partir dos filhos (que precisam estar corretos). O(1). */
static void atualiza(No *n) { n->altura = 1 + maior(h(n->esq), h(n->dir)); }

/* Fator de balanceamento: positivo = pende à esquerda. O(1). */
static int fb(const No *n) { return h(n->esq) - h(n->dir); }

/* Altura recalculada do zero, O(n). Usada SÓ no desenho: no meio de uma
 * inserção os ancestrais ainda guardam a altura antiga, e o desenho deve
 * mostrar o fator verdadeiro. */
static int altura_real(const No *n) {
    return n == NULL ? -1 : 1 + maior(altura_real(n->esq), altura_real(n->dir));
}

/* ------------------------------------------------------------------------- */
/* Desenho por níveis (coluna = posição em-ordem × ESP)                       */
/* ------------------------------------------------------------------------- */
#define ESP 8          /* rótulos de até 8 colunas: nós do mesmo nível nunca são vizinhos em-ordem */
#define LARG 104
#define NIVEIS 8

static char tela[2 * NIVEIS][LARG + 1];

static void rotulo(const No *n, char *buf, size_t tam) {
    int b = altura_real(n->esq) - altura_real(n->dir);
    if (b == 0) snprintf(buf, tam, "%d(0)%s", n->id, n == desbalanceado ? "!" : "");
    else        snprintf(buf, tam, "%d(%+d)%s", n->id, b, n == desbalanceado ? "!" : "");
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
    char txt[24];
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

static void desenha(const No *raiz) {
    if (raiz == NULL) { printf("    (árvore vazia)\n"); return; }
    int alt = altura_real(raiz);
    if (conta_nos(raiz) * ESP > LARG || alt >= NIVEIS) {
        printf("    (árvore grande demais para o desenho)\n");
        return;
    }
    for (int l = 0; l < 2 * NIVEIS; l++) {
        memset(tela[l], ' ', LARG);
        tela[l][LARG] = '\0';
    }
    int ordem = 0;
    posiciona(raiz, 0, &ordem);
    for (int l = 0; l <= 2 * alt; l++) {
        int fim = LARG;
        while (fim > 0 && tela[l][fim - 1] == ' ') fim--;
        printf("    %.*s\n", fim, tela[l]);
    }
}

/* ------------------------------------------------------------------------- */
/* Rotações                                                                   */
/* ------------------------------------------------------------------------- */
/*
 * Rotação à DIREITA em z (corrige excesso à esquerda):
 *
 *          z                y
 *         / \             /   \
 *        y   T4   ==>    x     z
 *       / \                   / \
 *      x   T3               T3   T4
 *
 * y sobe, z desce para a direita de y e a subárvore T3 (que estava entre y e
 * z na ordem) troca de pai: deixa de ser direita de y e vira esquerda de z.
 * A em-ordem (x, y, T3, z, T4) não muda: a árvore continua uma BST.
 * O(1): três ponteiros e duas alturas (primeiro z, que ficou embaixo).
 */
static No *rotaciona_direita(No *z) {
    No *y = z->esq;
    No *t3 = y->dir;
    if (narrar) {
        char t[24];
        if (t3) snprintf(t, sizeof t, "(raiz %d)", t3->id); else snprintf(t, sizeof t, "(vazia)");
        printf("    rotação à DIREITA em %d: %d sobe e %d vira filho direito de %d;\n",
               z->id, y->id, z->id, y->id);
        printf("    T3 %s deixa de ser direita de %d e vira esquerda de %d\n", t, y->id, z->id);
    }
    y->dir = z;
    z->esq = t3;
    atualiza(z);
    atualiza(y);
    return y;
}

/* Espelho da anterior: rotação à ESQUERDA em z (corrige excesso à direita). */
static No *rotaciona_esquerda(No *z) {
    No *y = z->dir;
    No *t2 = y->esq;
    if (narrar) {
        char t[24];
        if (t2) snprintf(t, sizeof t, "(raiz %d)", t2->id); else snprintf(t, sizeof t, "(vazia)");
        printf("    rotação à ESQUERDA em %d: %d sobe e %d vira filho esquerdo de %d;\n",
               z->id, y->id, z->id, y->id);
        printf("    T2 %s deixa de ser esquerda de %d e vira direita de %d\n", t, y->id, z->id);
    }
    y->esq = z;
    z->dir = t2;
    atualiza(z);
    atualiza(y);
    return y;
}

/*
 * Verifica o nó `*ref` e, se |fb| = 2, aplica a rotação certa. Recebe o
 * ENDEREÇO do campo que aponta para o nó (`No **`): assim a nova raiz da
 * subárvore é gravada direto no pai, e a árvore inteira continua consistente
 * a todo momento (é o que permite desenhá-la no meio do rebalanceamento).
 */
static void rebalanceia(No **ref) {
    No *z = *ref;
    int b = fb(z);
    if (b >= -1 && b <= 1) return;

    if (narrar) {
        printf("    !! %d ficou com fb = %+d -> fora da regra da AVL\n", z->id, b);
        desbalanceado = z;
        printf("    ANTES:\n");
        desenha(*ref_raiz);
        desbalanceado = NULL;
        pausar();
    }

    if (b > 1) {                                   /* pende à ESQUERDA */
        if (fb(z->esq) >= 0) {
            if (narrar) printf("    caso LL: o filho esquerdo %d também pende à esquerda\n",
                               z->esq->id);
            *ref = rotaciona_direita(z);
            rot_ll++;
        } else {
            if (narrar) {
                printf("    caso LR: o filho esquerdo %d pende à DIREITA (zigue-zague).\n",
                       z->esq->id);
                printf("    Uma rotação só não resolve: primeiro alinha (vira LL)...\n");
            }
            z->esq = rotaciona_esquerda(z->esq);
            if (narrar) { printf("    intermediário (agora é LL):\n"); desenha(*ref_raiz); }
            *ref = rotaciona_direita(z);
            rot_lr++;
        }
    } else {                                       /* pende à DIREITA */
        if (fb(z->dir) <= 0) {
            if (narrar) printf("    caso RR: o filho direito %d também pende à direita\n",
                               z->dir->id);
            *ref = rotaciona_esquerda(z);
            rot_rr++;
        } else {
            if (narrar) {
                printf("    caso RL: o filho direito %d pende à ESQUERDA (zigue-zague).\n",
                       z->dir->id);
                printf("    Primeiro alinha (vira RR)...\n");
            }
            z->dir = rotaciona_direita(z->dir);
            if (narrar) { printf("    intermediário (agora é RR):\n"); desenha(*ref_raiz); }
            *ref = rotaciona_esquerda(z);
            rot_rl++;
        }
    }
    if (narrar) {
        printf("    DEPOIS (todos os fb voltaram a -1, 0 ou +1):\n");
        desenha(*ref_raiz);
    }
}

/* ------------------------------------------------------------------------- */
/* Inserção, busca e liberação                                                */
/* ------------------------------------------------------------------------- */
static No *novo_no(int id, const char *tag) {
    No *n = malloc(sizeof *n);
    if (n == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    n->id = id;
    n->tag = tag;
    n->altura = 0;
    n->esq = n->dir = NULL;
    return n;
}

/*
 * Insere como numa BST comum e, NA VOLTA da recursão (de baixo para cima),
 * atualiza a altura de cada ancestral e rebalanceia se preciso. O(log n).
 * Armadilha: esquecer de atualizar a altura antes de calcular o fb faz a AVL
 * "não perceber" o desequilíbrio.
 */
static void insere(No **ref, int id, const char *tag) {
    No *n = *ref;
    if (n == NULL) {
        *ref = novo_no(id, tag);
        if (narrar) printf("    NULL encontrado -> pendura %d\n", id);
        return;
    }
    comparacoes++;
    if (id < n->id) {
        if (narrar) printf("    %d < %d -> esquerda\n", id, n->id);
        insere(&n->esq, id, tag);
    } else if (id > n->id) {
        if (narrar) printf("    %d > %d -> direita\n", id, n->id);
        insere(&n->dir, id, tag);
    } else {
        if (narrar) printf("    %d já cadastrado\n", id);
        return;
    }
    atualiza(n);
    rebalanceia(ref);
}

/* Busca comum de BST, contando comparações. O(log n) garantido na AVL. */
static const No *busca(const No *n, int id, int *comps) {
    *comps = 0;
    while (n != NULL) {
        (*comps)++;
        if (narrar) printf("    compara com %d\n", n->id);
        if (id == n->id) return n;
        n = id < n->id ? n->esq : n->dir;
    }
    return NULL;
}

static void em_ordem(const No *n) {
    if (n == NULL) return;
    em_ordem(n->esq);
    printf("    %4d  %s\n", n->id, n->tag);
    em_ordem(n->dir);
}

static void libera(No *n) {
    if (n == NULL) return;
    libera(n->esq);
    libera(n->dir);
    free(n);
}

static void zera_contadores(void) {
    rot_ll = rot_rr = rot_lr = rot_rl = 0;
    comparacoes = 0;
}

/* ------------------------------------------------------------------------- */
/* Demonstrações                                                              */
/* ------------------------------------------------------------------------- */
/* Altura que uma BST COMUM (sem rotações) teria com as mesmas inserções. */
static int altura_bst_comum(const int *ids, int n) {
    No *raiz = NULL;
    for (int i = 0; i < n; i++) {
        No **p = &raiz;
        while (*p != NULL) p = ids[i] < (*p)->id ? &(*p)->esq : &(*p)->dir;
        *p = novo_no(ids[i], "");
    }
    int alt = altura_real(raiz);
    libera(raiz);
    return alt;
}

/*
 * Um dos 4 casos, com 3 inserções numa árvore vazia.
 * `ref_raiz` guarda o ENDEREÇO da variável `raiz`: se a rotação trocar a raiz
 * da árvore, o desenho feito dentro de rebalanceia() já enxerga a nova.
 */
static void caso_minimo(const char *titulo, int a, int b, int c) {
    printf("\n================ %s: inserir %d, %d, %d ================\n", titulo, a, b, c);
    No *raiz = NULL;
    ref_raiz = &raiz;
    const int ids[] = {a, b, c};
    for (int i = 0; i < 3; i++) {
        printf("\n>>> inserir %d\n", ids[i]);
        insere(&raiz, ids[i], "");
        if (i < 2) desenha(raiz);   /* a 3ª inserção é desenhada antes/depois da rotação */
    }
    pausar();
    libera(raiz);
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    printf("=== ÁRVORE AVL: FATOR DE BALANCEAMENTO E ROTAÇÕES ===\n");
    printf("Rótulo id(fb): fb = altura(esq) - altura(dir). Regra AVL: fb em {-1, 0, +1}.\n");
    printf("\"!\" marca o nó que saiu da regra (fb = +2 ou -2).\n");
    pausar();

    caso_minimo("CASO LL", 300, 200, 100);
    caso_minimo("CASO RR", 100, 200, 300);
    caso_minimo("CASO LR", 300, 100, 200);
    caso_minimo("CASO RL", 100, 300, 200);

    /* ---- Exemplo real: índice de tags SCADA ---- */
    printf("\n================ ÍNDICE DE TAGS SCADA POR ID ================\n");
    printf("Instrumentos comissionados um a um; IDs quase sempre crescentes.\n");
    static const struct { int id; const char *tag; } tags[] = {
        {110, "FT-110  vazão de água de alimentação"},
        {120, "PT-120  pressão da caldeira"},
        {130, "TT-130  temperatura do reator"},
        {140, "LT-140  nível do tanque pulmão"},
        {150, "XV-150  válvula on/off de descarga"},
        {160, "M-160   bomba de recirculação"},
        {170, "TT-170  temperatura do trocador"},
        {105, "AI-105  analisador de pH"},
        {101, "PB-101  botão de emergência"},
        {103, "ZS-103  fim de curso da comporta"},
        {165, "FV-165  válvula de controle de vazão"},
        {167, "PT-167  pressão na linha de vapor"},
        {102, "TT-102  temperatura do mancal"},
    };
    const int qt = (int)(sizeof tags / sizeof tags[0]);

    zera_contadores();
    No *raiz = NULL;
    ref_raiz = &raiz;
    int ids[sizeof tags / sizeof tags[0]];
    for (int i = 0; i < qt; i++) {
        ids[i] = tags[i].id;
        printf("\n>>> inserir %s\n", tags[i].tag);
        long antes = rot_ll + rot_rr + rot_lr + rot_rl;
        insere(&raiz, tags[i].id, tags[i].tag);
        if (rot_ll + rot_rr + rot_lr + rot_rl == antes) {   /* sem rotação: desenha aqui */
            printf("    sem desequilíbrio; árvore após inserir %d:\n", tags[i].id);
            desenha(raiz);
        }
        printf("    altura %d com %d nós\n", h(raiz), conta_nos(raiz));
        pausar();
    }
    printf("\nRotações: LL=%ld RR=%ld LR=%ld RL=%ld | altura final %d com %d tags "
           "(uma BST comum, nessa ordem, teria altura %d)\n",
           rot_ll, rot_rr, rot_lr, rot_rl, h(raiz), qt, altura_bst_comum(ids, qt));

    printf("\nListagem do índice (em-ordem = ordenado por ID):\n");
    em_ordem(raiz);
    int c;
    printf("\nBuscar o tag 165:\n");
    const No *achou = busca(raiz, 165, &c);
    printf("    -> %s em %d comparações\n", achou ? achou->tag : "não encontrado", c);
    pausar();
    libera(raiz);

    /* ---- Tabela: inserção em ordem crescente, BST x AVL ---- */
    printf("\n================ BST x AVL COM IDs EM ORDEM CRESCENTE ================\n");
    printf("  %7s | %8s %8s %11s | %11s %10s | %13s %13s\n", "n", "h BST", "h AVL",
           "1,44log2 n", "busca BST", "busca AVL", "comps BST", "comps AVL");
    narrar = 0;
    const int tamanhos[] = {10, 100, 1000, 10000, 100000};
    for (int t = 0; t < 5; t++) {
        int n = tamanhos[t];
        zera_contadores();
        No *avl = NULL;
        for (int i = 0; i < n; i++) insere(&avl, i, "");
        int cb;
        busca(avl, n - 1, &cb);
        /* BST comum com chaves crescentes = "lista" pela direita: valores exatos
         * pela fórmula (altura n-1, buscar o último custa n, inserir tudo
         * custa 0 + 1 + ... + (n-1) = n(n-1)/2). */
        printf("  %7d | %8d %8d %11.1f | %11d %10d | %13lld %13ld\n", n, n - 1, h(avl),
               1.44 * log2(n + 2.0), n, cb, (long long)n * (n - 1) / 2, comparacoes);
        if (n == 100000)
            printf("\n  Com 100000 IDs crescentes a AVL fez LL=%ld RR=%ld LR=%ld RL=%ld rotações\n",
                   rot_ll, rot_rr, rot_lr, rot_rl);
        libera(avl);
    }
    printf("  (cada uma O(1)). Buscar qualquer tag custa no máximo h+1 comparações,\n");
    printf("  contra até 100000 na BST degenerada.\n");
    printf("  Colunas \"comps\": total de comparações para inserir tudo.\n");

    return 0;
}
