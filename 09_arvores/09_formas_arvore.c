/*
 * ============================================================================
 * Arquivo : 09_formas_arvore.c
 * Módulo  : 09 — Árvores                      (Ementa: Unidade 5)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Comparar, num só programa, formas de ARMAZENAR, INSERIR, REMOVER e
 *   PERCORRER árvores:
 *     1. Árvore binária em vetor estático (pai=(i-1)/2, esq=2i+1, dir=2i+2),
 *        com inserção por nível.
 *     2. Min-heap em vetor: inserção subindo (`sift_up`) e remoção da raiz
 *        descendo (`sift_down`); extrair sempre o menor ordena os dados (é a
 *        ideia do heapsort). Versão passo a passo: 10_heap_alarmes.c.
 *     3. BST com inserção ITERATIVA (ponteiro duplo `NoBin **`, sem recursão)
 *        e listagem crescente (Esq->Raiz->Dir) e decrescente (Dir->Raiz->Esq).
 *     4. Árvore genérica (primeiro_filho / proximo_irmao): dissolver um nó
 *        intermediário, promovendo os filhos dele para o lugar que ele ocupava
 *        na lista de irmãos.
 *
 * EXEMPLO REAL
 *   Vetor/heap: firmware de microcontrolador sem malloc, filas de prioridade
 *   de alarmes e de tarefas de um RTOS. BST iterativa: índices em sistemas
 *   com pilha pequena. Dissolver nó: extinguir uma área intermediária do
 *   organograma da planta; os equipamentos passam a responder à área acima.
 *
 * COMPLEXIDADE (n = número de elementos, h = altura, g = grau)
 *   Operação                     Tempo         Espaço extra
 *   insere_por_nivel_vetor       O(1)          O(1)
 *   heap_insere / remove_minimo  O(log n)      O(1)   (heap é sempre completo)
 *   ordenar extraindo do heap    O(n log n)    O(1)
 *   abb_insere_iterativo         O(h)          O(1)   (sem pilha de recursão)
 *   em-ordem (cresc./decresc.)   O(n)          O(h)
 *   dissolve_no_generica         O(n)          O(h)   (busca + junção de listas)
 *
 * COMO EXECUTAR
 *   ./run 09_formas_arvore
 * ============================================================================
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* ==========================================================================
 * PARTE 1: ÁRVORE BINÁRIA ARMAZENADA EM VETOR (SEM PONTEIROS / SEM MALLOC)
 * ==========================================================================
 * Muito usada em sistemas embarcados (microcontroladores) e em Heaps:
 * Para qualquer nó no índice `i` (começando em 0):
 *   - Filho esquerdo está em: 2*i + 1
 *   - Filho direito  está em: 2*i + 2
 *   - Pai            está em: (i - 1) / 2
 */
#define CAP_VETOR 15

typedef struct {
    int dados[CAP_VETOR];
    int tamanho; /* Quantidade atual de elementos ocupados da esquerda p/ direita */
} ArvoreVetor;

/* Inserção por nível (completa da esquerda para a direita): O(1) */
bool insere_por_nivel_vetor(ArvoreVetor *av, int valor) {
    if (av->tamanho >= CAP_VETOR) return false; /* Vetor cheio */
    av->dados[av->tamanho++] = valor;           /* Ocupa a próxima vaga do nível */
    return true;
}

/* Imprime a árvore em vetor mostrando quem é pai e quem são os filhos de cada índice */
void mostra_arvore_vetor(const ArvoreVetor *av) {
    printf("  Indices no vetor: ");
    for (int i = 0; i < av->tamanho; i++) printf("[%d]=%d ", i, av->dados[i]);
    printf("\n");

    for (int i = 0; i < av->tamanho; i++) {
        int esq = 2 * i + 1; /* Fórmula do filho esquerdo */
        int dir = 2 * i + 2; /* Fórmula do filho direito  */
        printf("  No [%d] (valor %2d): ", i, av->dados[i]);
        if (i == 0) printf("RAIZ      | ");
        else        printf("pai=[%d]=%2d | ", (i - 1) / 2, av->dados[(i - 1) / 2]);

        if (esq < av->tamanho) printf("esq=[%d]=%2d ", esq, av->dados[esq]);
        else                   printf("esq=---    ");

        if (dir < av->tamanho) printf("dir=[%d]=%2d", dir, av->dados[dir]);
        else                   printf("dir=---");
        printf("\n");
    }
}

/* ==========================================================================
 * PARTE 2: MIN-HEAP EM VETOR (INSERÇÃO, REMOÇÃO E ORDENAÇÃO POR PRIORIDADE)
 * ==========================================================================
 * Regra do Min-Heap: todo pai é MENOR ou IGUAL aos seus dois filhos.
 * Logo, o menor elemento do conjunto está SEMPRE na raiz (índice 0)!
 */
static void troca(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * Inserção no Min-Heap (`sift_up`):
 * 1. Coloca o novo valor no final do vetor (última folha livre);
 * 2. Enquanto ele for menor que o pai, troca com o pai (sobe na árvore).
 */
bool heap_insere(ArvoreVetor *heap, int valor) {
    if (heap->tamanho >= CAP_VETOR) return false;
    int i = heap->tamanho++;
    heap->dados[i] = valor;

    /* Sobe enquanto não chegou na raiz (i > 0) e for menor que o pai */
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (heap->dados[i] >= heap->dados[pai]) break; /* Ordem do heap restaurada */
        troca(&heap->dados[i], &heap->dados[pai]);
        i = pai; /* Continua subindo a partir da posição do pai */
    }
    return true;
}

/*
 * Remoção da raiz no Min-Heap (`sift_down`):
 * 1. Guarda o menor valor (que está em `dados[0]`);
 * 2. Move a última folha do vetor para a raiz (`dados[0]`) e reduz `tamanho`;
 * 3. Desce esse valor trocando-o sempre com o MENOR dos seus filhos até restaurar a regra.
 * Pré-condição: heap->tamanho > 0 (quem chama deve verificar; heap vazio não tem raiz).
 */
int heap_remove_minimo(ArvoreVetor *heap) {
    int minimo = heap->dados[0];                 /* Raiz = menor elemento */
    heap->dados[0] = heap->dados[--heap->tamanho]; /* Última folha sobe p/ raiz */

    int i = 0;
    for (;;) {
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;
        int menor = i;

        /* Descobre quem é o menor entre o nó atual, o filho esquerdo e o direito */
        if (esq < heap->tamanho && heap->dados[esq] < heap->dados[menor]) menor = esq;
        if (dir < heap->tamanho && heap->dados[dir] < heap->dados[menor]) menor = dir;

        if (menor == i) break; /* Já é menor ou igual aos dois filhos: terminou! */
        troca(&heap->dados[i], &heap->dados[menor]);
        i = menor;             /* Continua descendo pelo lado onde houve a troca */
    }
    return minimo;
}

/* ==========================================================================
 * PARTE 3: ABB COM INSERÇÃO ITERATIVA + ORDENAÇÃO CRESCENTE E DECRESCENTE
 * ==========================================================================
 */
typedef struct NoBin {
    int valor;
    struct NoBin *esquerda;
    struct NoBin *direita;
} NoBin;

/*
 * Inserção ITERATIVA na ABB (sem recursão):
 * Usa um ponteiro duplo `NoBin **atual` que caminha desde o endereço da raiz
 * até encontrar o ponteiro `NULL` exato onde o novo nó deve ser pendurado.
 */
void abb_insere_iterativo(NoBin **raiz, int valor) {
    NoBin **atual = raiz;
    while (*atual != NULL) {
        if (valor < (*atual)->valor) {
            atual = &(*atual)->esquerda; /* Caminha para o ponteiro da esquerda */
        } else if (valor > (*atual)->valor) {
            atual = &(*atual)->direita;  /* Caminha para o ponteiro da direita  */
        } else {
            return; /* Valor já existe: ignora duplicata */
        }
    }
    /* `*atual` é o próprio campo (`raiz`, `esquerda` ou `direita`) que estava NULL! */
    NoBin *novo = malloc(sizeof *novo);
    if (novo == NULL) exit(EXIT_FAILURE);
    novo->valor = valor;
    novo->esquerda = NULL;
    novo->direita = NULL;
    *atual = novo; /* Engancha diretamente sem precisar de variável `pai` */
}

/* Percurso Em-Ordem Crescente: Esquerda (menores) -> Nó -> Direita (maiores) */
void abb_em_ordem_crescente(const NoBin *r) {
    if (r == NULL) return;
    abb_em_ordem_crescente(r->esquerda);
    printf("%d ", r->valor);
    abb_em_ordem_crescente(r->direita);
}

/* Percurso Em-Ordem Inverso (Decrescente): Direita (maiores) -> Nó -> Esquerda (menores) */
void abb_em_ordem_decrescente(const NoBin *r) {
    if (r == NULL) return;
    abb_em_ordem_decrescente(r->direita); /* 1º Visita todos os maiores */
    printf("%d ", r->valor);              /* 2º Imprime o nó atual      */
    abb_em_ordem_decrescente(r->esquerda);/* 3º Visita todos os menores */
}

void abb_libera(NoBin *r) {
    if (r == NULL) return;
    abb_libera(r->esquerda);
    abb_libera(r->direita);
    free(r);
}

/* ==========================================================================
 * PARTE 4: DISSOLVER UM NÓ EM ÁRVORE GENÉRICA (PRIMEIRO FILHO / PRÓXIMO IRMÃO)
 * ==========================================================================
 * Remove UM nó intermediário, mas NÃO os seus descendentes: os filhos dele são
 * "promovidos" e passam a ser filhos do avô, na mesma posição que o nó ocupava.
 */
typedef struct NoGen {
    int valor;
    struct NoGen *primeiro_filho;
    struct NoGen *proximo_irmao;
} NoGen;

NoGen *cria_gen(int valor) {
    NoGen *n = malloc(sizeof *n);
    if (n == NULL) exit(EXIT_FAILURE);
    n->valor = valor;
    n->primeiro_filho = NULL;
    n->proximo_irmao = NULL;
    return n;
}

void adiciona_filho_gen(NoGen *pai, NoGen *filho) {
    NoGen **p = &pai->primeiro_filho;
    while (*p != NULL) p = &(*p)->proximo_irmao;
    *p = filho;
}

void libera_gen(NoGen *n) {
    while (n != NULL) {
        NoGen *irmao = n->proximo_irmao;
        libera_gen(n->primeiro_filho);
        free(n);
        n = irmao;
    }
}

/*
 * Dissolve o nó com `valor` que esteja ABAIXO de `raiz` (a raiz nunca é
 * dissolvida, senão a árvore viraria uma floresta com várias raízes).
 *
 * Na árvore genérica, os filhos de cada nó formam uma LISTA ENCADEADA ligada
 * por `proximo_irmao`. Dissolver o nó X é emendar listas:
 *
 *   antes:  pai -> [A] -> [X] -> [B]        X tem filhos [x1] -> [x2]
 *   depois: pai -> [A] -> [x1] -> [x2] -> [B]
 *
 * O ponteiro duplo `p` aponta para o CAMPO que guarda X (o `primeiro_filho` do
 * pai ou o `proximo_irmao` do irmão anterior); assim, trocar `*p` religa a
 * lista sem precisar guardar o "anterior". O(n): busca em profundidade.
 */
bool dissolve_no_generica(NoGen *raiz, int valor) {
    if (raiz == NULL) return false;
    for (NoGen **p = &raiz->primeiro_filho; *p != NULL; p = &(*p)->proximo_irmao) {
        NoGen *alvo = *p;
        if (alvo->valor == valor) {
            if (alvo->primeiro_filho == NULL) {
                *p = alvo->proximo_irmao;          /* Sem filhos: só pula o nó */
            } else {
                NoGen *ultimo = alvo->primeiro_filho;
                while (ultimo->proximo_irmao != NULL) ultimo = ultimo->proximo_irmao;
                ultimo->proximo_irmao = alvo->proximo_irmao; /* Netos -> irmãos de X */
                *p = alvo->primeiro_filho;         /* Netos ocupam o lugar de X */
            }
            free(alvo);                            /* Só o próprio nó é liberado */
            return true;
        }
        /* Não é este: procura entre os descendentes dele */
        if (dissolve_no_generica(alvo, valor)) return true;
    }
    return false;
}

void imprime_gen(const NoGen *n, int nivel) {
    if (n == NULL) return;
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("%d\n", n->valor);
    for (const NoGen *f = n->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        imprime_gen(f, nivel + 1);
    }
}

int main(void) {
    printf("=== 1. ARVORE BINARIA ARMAZENADA EM VETOR (INSERCAO POR NIVEL) ===\n");
    ArvoreVetor av = { .tamanho = 0 };
    int dados_nivel[] = {10, 20, 30, 40, 50, 60};
    for (size_t i = 0; i < sizeof dados_nivel / sizeof dados_nivel[0]; i++) {
        insere_por_nivel_vetor(&av, dados_nivel[i]);
    }
    mostra_arvore_vetor(&av);

    printf("\n=== 2. FILA DE PRIORIDADE EM VETOR (MIN-HEAP) ===\n");
    ArvoreVetor heap = { .tamanho = 0 };
    int leituras[] = {45, 12, 80, 5, 30, 18};
    for (size_t i = 0; i < sizeof leituras / sizeof leituras[0]; i++) {
        heap_insere(&heap, leituras[i]);
    }
    printf("  Vetor interno do Min-Heap (raiz [0] e sempre o menor):\n");
    mostra_arvore_vetor(&heap);
    printf("  Removendo sempre a raiz (sai em ordem crescente): ");
    while (heap.tamanho > 0) {
        printf("%d ", heap_remove_minimo(&heap));
    }
    printf("\n");

    printf("\n=== 3. ABB ITERATIVA: ORDENACAO CRESCENTE vs. DECRESCENTE ===\n");
    NoBin *abb = NULL;
    for (size_t i = 0; i < sizeof leituras / sizeof leituras[0]; i++) {
        abb_insere_iterativo(&abb, leituras[i]);
    }
    printf("  Em-Ordem Crescente   (Esq -> Raiz -> Dir): ");
    abb_em_ordem_crescente(abb);
    printf("\n  Em-Ordem Decrescente (Dir -> Raiz -> Esq): ");
    abb_em_ordem_decrescente(abb);
    printf("\n");
    abb_libera(abb);

    printf("\n=== 4. DISSOLVER UM NO EM ARVORE GENERICA (PROMOVENDO OS FILHOS) ===\n");
    /*
     *        1                         1
     *     /  |  \               /   /  |   \
     *    2   3   4     ==>     5   6   3    4
     *   / \      |                          |
     *  5   6     7                          7
     */
    NoGen *raiz_gen = cria_gen(1);
    NoGen *n2 = cria_gen(2), *n3 = cria_gen(3), *n4 = cria_gen(4);
    adiciona_filho_gen(raiz_gen, n2);
    adiciona_filho_gen(raiz_gen, n3);
    adiciona_filho_gen(raiz_gen, n4);
    adiciona_filho_gen(n2, cria_gen(5));
    adiciona_filho_gen(n2, cria_gen(6));
    adiciona_filho_gen(n4, cria_gen(7));

    printf("  Antes de dissolver o no 2 (filhos 5 e 6; irmaos 3 e 4):\n");
    imprime_gen(raiz_gen, 2);

    dissolve_no_generica(raiz_gen, 2);
    printf("  Depois: 5 e 6 viraram filhos de 1, no lugar do 2; 3 e 4 seguem intactos:\n");
    imprime_gen(raiz_gen, 2);

    libera_gen(raiz_gen);
    return 0;
}
