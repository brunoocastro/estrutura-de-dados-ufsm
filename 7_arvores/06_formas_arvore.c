/*
 * Comparativo completo de Árvores em C:
 *   1. Formas de ARMAZENAR:
 *      - Árvore Binária em Vetor Estático (índices pai=(i-1)/2, esq=2i+1, dir=2i+2)
 *      - Árvore Binária Dinâmica (ponteiros esquerda/direita)
 *      - Árvore Genérica Dinâmica (primeiro_filho / proximo_irmao)
 *   2. Formas de INSERIR:
 *      - Inserção por Nível (em árvore armazenada em vetor)
 *      - Inserção Iterativa na ABB (com `while`, sem gastar pilha de recursão)
 *      - Inserção em Min-Heap (subindo o elemento - `sift_up` / `bubble_up`)
 *   3. Formas de REMOVER:
 *      - Remoção da raiz (menor elemento) no Min-Heap (`sift_down`)
 *      - Remoção de um nó específico em Árvore Genérica (religando irmãos/filhos)
 *   4. Formas de ORDENAR e PERCORRER:
 *      - Em-Ordem Crescente (Esq -> Raiz -> Dir)
 *      - Em-Ordem Decrescente (Dir -> Raiz -> Esq)
 *      - Heap Sort / Fila de Prioridade (extraindo sempre o menor em O(log n))
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
 * PARTE 4: REMOÇÃO DE UM NÓ EM ÁRVORE GENÉRICA (PRIMEIRO FILHO / PRÓXIMO IRMÃO)
 * ==========================================================================
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
 * Remove o nó com `valor` (e toda a sua subárvore) de uma Árvore Genérica:
 * Diferente da árvore binária (onde bastava pôr NULL no ponteiro do pai),
 * na árvore genérica os filhos formam uma LISTA ENCADEADA. Por isso, ao
 * remover um filho, precisamos religar o ponteiro para o `proximo_irmao` dele
 * para não perder os irmãos seguintes!
 */
bool remove_no_generica(NoGen **ponteiro_lista, int valor) {
    for (NoGen **p = ponteiro_lista; *p != NULL; p = &(*p)->proximo_irmao) {
        if ((*p)->valor == valor) {
            NoGen *alvo = *p;
            /*
             * Religa a lista de irmãos: quem apontava para `alvo` passa a
             * apontar para `alvo->proximo_irmao` (não perde os irmãos ao lado!).
             */
            *p = alvo->proximo_irmao;
            alvo->proximo_irmao = NULL; /* Isola `alvo` dos irmãos antes de liberar */
            libera_gen(alvo);           /* Libera `alvo` e todos os seus descendentes */
            return true;
        }
        /* Se não é este nó, procura recursivamente dentro da lista de filhos dele */
        if (remove_no_generica(&(*p)->primeiro_filho, valor)) return true;
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

    printf("\n=== 2. FILA DE PRIORIDADE / HEAP SORT EM VETOR (MIN-HEAP) ===\n");
    ArvoreVetor heap = { .tamanho = 0 };
    int leituras[] = {45, 12, 80, 5, 30, 18};
    for (size_t i = 0; i < sizeof leituras / sizeof leituras[0]; i++) {
        heap_insere(&heap, leituras[i]);
    }
    printf("  Vetor interno do Min-Heap (raiz [0] e sempre o menor):\n");
    mostra_arvore_vetor(&heap);
    printf("  Removendo sempre a raiz (Heap Sort crescente): ");
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

    printf("\n=== 4. REMOCAO EM ARVORE GENERica (PRESERVANDO OS IRMAOS) ===\n");
    /*
     *        1
     *     /  |  \
     *    2   3   4   <-- Remover o filho do meio (3) sem perder o irmão (4)!
     *   / \      |
     *  5   6     7
     */
    NoGen *raiz_gen = cria_gen(1);
    NoGen *n2 = cria_gen(2), *n3 = cria_gen(3), *n4 = cria_gen(4);
    adiciona_filho_gen(raiz_gen, n2);
    adiciona_filho_gen(raiz_gen, n3);
    adiciona_filho_gen(raiz_gen, n4);
    adiciona_filho_gen(n2, cria_gen(5));
    adiciona_filho_gen(n2, cria_gen(6));
    adiciona_filho_gen(n4, cria_gen(7));

    printf("  Antes de remover o no 2 (que tem filhos 5 e 6 e irmaos 3 e 4):\n");
    imprime_gen(raiz_gen, 2);

    remove_no_generica(&raiz_gen, 2);
    printf("  Depois de remover o no 2 (irmaos 3 e 4 continuam intactos!):\n");
    imprime_gen(raiz_gen, 2);

    libera_gen(raiz_gen);
    return 0;
}
