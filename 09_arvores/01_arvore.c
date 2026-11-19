/*
 * ============================================================================
 * Arquivo : 01_arvore.c
 * Módulo  : 09 — Árvores                      (Ementa: Unidade 5)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Apresentar a árvore genérica (n-ária): raiz, filhos, irmãos, folhas, grau
 *   e altura. Cada nó pode ter qualquer quantidade de filhos. A representação
 *   usa "primeiro filho / próximo irmão": cada nó guarda só dois ponteiros, um
 *   para o primeiro filho (desce um nível) e outro para o irmão seguinte (anda
 *   no mesmo nível). Os filhos de um nó formam, portanto, uma LISTA ENCADEADA.
 *
 * EXEMPLO REAL
 *   Pastas de um sistema de arquivos, menus de uma IHM, árvore de ativos de
 *   uma planta (Planta -> Área -> CLP -> E/S; ver 02_automacao_scada.c).
 *
 * COMPLEXIDADE (n = número de nós, g = grau do nó)
 *   Operação          Tempo     Espaço extra
 *   adiciona_filho    O(g)      O(1)         percorre a lista de irmãos
 *   conta/altura      O(n)      O(h) pilha   visita cada nó uma vez
 *   busca             O(n)      O(h) pilha   sem regra de ordem: pode ver todos
 *   libera            O(n)      O(h) pilha
 *   Por quê: sem uma regra de ordenação (como na BST), não há como saber em
 *   qual filho está o valor; a busca precisa olhar a árvore inteira.
 *
 * COMO EXECUTAR
 *   ./run 01_arvore
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int valor;                 /* Dado armazenado no nó                           */
    struct No *primeiro_filho; /* Aponta para o 1º filho (desce um nível)         */
    struct No *proximo_irmao;  /* Aponta para o irmão seguinte (mesmo nível/pai)  */
} No;

No *cria_no(int valor) {
    No *no = malloc(sizeof *no); /* Aloca dinamicamente no heap um novo nó */
    if (no == NULL) {            /* Sempre verifica se havia memória disponível */
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    no->valor = valor;
    /* Inicializa os ponteiros com NULL pois o nó nasce sem filhos e sem irmãos */
    no->primeiro_filho = NULL;
    no->proximo_irmao = NULL;
    return no;
}

/* Adiciona `filho` como último filho de `pai`. Retorna o próprio filho.
 * O(g): anda até o fim da lista de irmãos. Guardar um ponteiro para o último
 * filho deixaria em O(1), ao custo de mais um campo por nó. */
No *adiciona_filho(No *pai, No *filho) {
    if (pai->primeiro_filho == NULL) {
        /* Caso 1: O pai ainda não tinha nenhum filho; este passa a ser o 1º */
        pai->primeiro_filho = filho;
    } else {
        /* Caso 2: Já existem filhos; percorre a lista de irmãos até o último */
        No *irmao = pai->primeiro_filho;
        while (irmao->proximo_irmao != NULL) {
            irmao = irmao->proximo_irmao; /* Avança lateralmente pelos irmãos */
        }
        irmao->proximo_irmao = filho; /* Engancha o novo filho no final da fila */
    }
    return filho;
}

/* Imprime a árvore com indentação proporcional à profundidade (pré-ordem). */
void imprime(const No *no, int profundidade) {
    if (no == NULL) return; /* Caso base da recursão: subárvore vazia */

    /* 1º Visita a raiz atual (Pré-Ordem: pai é impresso antes dos filhos) */
    for (int i = 0; i < profundidade; i++) printf("  ");
    printf("%d\n", no->valor);

    /* 2º Percorre a lista encadeada de filhos, aumentando a profundidade em +1 */
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        imprime(f, profundidade + 1);
    }
}

/* O(n): visita cada nó exatamente uma vez. */
int conta_nos(const No *no) {
    if (no == NULL) return 0; /* Árvore vazia tem 0 nós */
    int total = 1;            /* Conta o próprio nó atual (1) */
    /* Soma recursivamente a quantidade de nós de cada subárvore filha */
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        total += conta_nos(f);
    }
    return total;
}

/* O(n). Folha = nó sem filhos (primeiro_filho == NULL). */
int conta_folhas(const No *no) {
    if (no == NULL) return 0;
    /* Se não tem primeiro_filho, este nó não possui descendentes: é uma folha! */
    if (no->primeiro_filho == NULL) return 1;

    int total = 0;
    /* Se não é folha, delega a contagem para todas as subárvores filhas */
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        total += conta_folhas(f);
    }
    return total;
}

/* Altura = número de arestas no caminho mais longo da raiz até uma folha.
 * O(n): precisa da altura de todos os filhos antes do pai (pós-ordem). */
int altura(const No *no) {
    if (no == NULL) return -1; /* Convenção: árvore vazia tem altura -1 */
    int maior = -1;            /* Guarda a maior altura encontrada entre os filhos */
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        int h = altura(f);     /* Calcula a altura da subárvore filha (Pós-Ordem) */
        if (h > maior) maior = h;
    }
    /* A altura do pai é 1 aresta + a maior altura entre seus filhos (folha dá 0) */
    return maior + 1;
}

/* Grau = número de filhos diretos do nó. O(g). */
int grau(const No *no) {
    int g = 0;
    /* Basta contar quantos elementos existem na lista de irmãos do 1º filho */
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) g++;
    return g;
}

/* Busca em profundidade (pré-ordem). O(n) no pior caso: valor ausente. */
const No *busca(const No *no, int valor) {
    if (no == NULL) return NULL;        /* Fim do ramo: não achou aqui */
    if (no->valor == valor) return no;  /* Encontrou no próprio nó atual! */

    /* Como não há regra de ordenação na árvore genérica, busca em cada filho */
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        const No *achou = busca(f, valor);
        if (achou != NULL) return achou; /* Interrompe assim que encontrar */
    }
    return NULL; /* Percorreu todos os filhos e não encontrou */
}

/* Libera em pós-ordem: filhos antes do pai. O(n).
 * Armadilha: dar free(no) e DEPOIS ler no->proximo_irmao é uso após liberação. */
void libera(No *no) {
    while (no != NULL) {
        /* Salva o ponteiro para o irmão ANTES de dar free(no), senão perderíamos o endereço! */
        No *irmao = no->proximo_irmao;
        libera(no->primeiro_filho); /* 1º Libera recursivamente todos os descendentes */
        free(no);                   /* 2º Só agora libera o nó atual com segurança     */
        no = irmao;                 /* 3º Avança para o próximo irmão do mesmo nível   */
    }
}

int main(void) {
    /*
     *            1
     *         /  |  \
     *        2   3   4
     *       / \      |
     *      5   6     7
     *                |
     *                8
     */
    No *raiz = cria_no(1);
    No *n2 = adiciona_filho(raiz, cria_no(2));
    adiciona_filho(raiz, cria_no(3));
    No *n4 = adiciona_filho(raiz, cria_no(4));
    adiciona_filho(n2, cria_no(5));
    adiciona_filho(n2, cria_no(6));
    No *n7 = adiciona_filho(n4, cria_no(7));
    adiciona_filho(n7, cria_no(8));

    printf("Arvore (pre-ordem, indentada por nivel):\n");
    imprime(raiz, 0);

    printf("\nNos: %d\n", conta_nos(raiz));
    printf("Folhas: %d\n", conta_folhas(raiz));
    printf("Altura: %d\n", altura(raiz));
    printf("Grau da raiz: %d\n", grau(raiz));
    printf("Busca 7: %s\n", busca(raiz, 7) ? "encontrado" : "nao encontrado");
    printf("Busca 9: %s\n", busca(raiz, 9) ? "encontrado" : "nao encontrado");

    libera(raiz);
    return 0;
}
