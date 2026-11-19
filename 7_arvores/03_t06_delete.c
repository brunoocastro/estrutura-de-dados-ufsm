/*
 * Tarefa Avaliada 06 - Questão 1
 *
 * Função `delete`: remove de uma árvore binária o ramo (subárvore) cuja raiz
 * contém o valor fornecido. O nó encontrado e todos os seus descendentes são
 * liberados, e o ponteiro do pai passa a valer NULL.
 *
 * A busca percorre a árvore inteira, então funciona para qualquer árvore
 * binária, não apenas para árvores binárias de busca. Se o valor aparecer
 * mais de uma vez, remove-se o primeiro ramo encontrado em pré-ordem.
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *esquerda;
    struct No *direita;
} No;

No *cria_no(int valor, No *esquerda, No *direita) {
    No *no = malloc(sizeof *no);
    if (no == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    no->valor = valor;
    no->esquerda = esquerda;
    no->direita = direita;
    return no;
}

/* Libera uma subárvore inteira em pós-ordem. */
void libera(No *raiz) {
    if (raiz == NULL) return;   /* Caso base: ponteiro nulo não precisa de free */
    libera(raiz->esquerda);     /* 1º Libera toda a subárvore esquerda          */
    libera(raiz->direita);      /* 2º Libera toda a subárvore direita           */
    free(raiz);                 /* 3º Só por último libera o nó pai!            */
}

/*
 * Remove o ramo com raiz em `valor`. Recebe o endereço do ponteiro (`No **`)
 * que aponta para a subárvore, para poder gravar NULL diretamente no campo
 * `esquerda`/`direita` do pai (ou na própria variável `raiz` do main).
 * Retorna 1 se removeu, 0 se o valor não existe.
 */
int delete(No **raiz, int valor) {
    if (*raiz == NULL) return 0;        /* Chegou em folha vazia: valor não está aqui */
    if ((*raiz)->valor == valor) {      /* Achou o nó raiz do ramo que deve ser apagado! */
        libera(*raiz);                  /* Libera o nó e todos os seus descendentes (pós-ordem) */
        *raiz = NULL;                   /* Corta a ligação no pai para evitar ponteiro pendente (dangling pointer) */
        return 1;                       /* Sucesso: ramo removido */
    }
    /*
     * Passa o ENDEREÇO dos campos `esquerda` e `direita` (`&(*raiz)->esquerda`).
     * O operador `||` tem curto-circuito: se achar e remover na esquerda, nem procura na direita.
     */
    return delete(&(*raiz)->esquerda, valor) || delete(&(*raiz)->direita, valor);
}

/* Imprime a árvore deitada: direita em cima, esquerda embaixo. */
void imprime(const No *raiz, int nivel) {
    if (raiz == NULL) return;
    imprime(raiz->direita, nivel + 1);              /* Topo do desenho: subárvore direita */
    for (int i = 0; i < nivel; i++) printf("    "); /* Recuo proporcional à profundidade  */
    printf("%d\n", raiz->valor);                    /* Nó atual no meio                   */
    imprime(raiz->esquerda, nivel + 1);             /* Base do desenho: subárvore esquerda */
}
static void mostra(const No *raiz) {
    if (raiz == NULL) printf("  (arvore vazia)\n");
    else imprime(raiz, 1);
    printf("\n");
}

static void tenta_delete(No **raiz, int valor) {
    printf("delete(%d): %s\n", valor,
           delete(raiz, valor) ? "ramo removido" : "valor nao encontrado");
    mostra(*raiz);
}

int main(void) {
    /*
     *              1
     *           /     \
     *          2       3
     *         / \     / \
     *        4   5   6   7
     *           / \
     *          8   9
     */
    No *raiz = cria_no(1,
        cria_no(2, cria_no(4, NULL, NULL),
                   cria_no(5, cria_no(8, NULL, NULL), cria_no(9, NULL, NULL))),
        cria_no(3, cria_no(6, NULL, NULL), cria_no(7, NULL, NULL)));

    printf("Arvore inicial (deitada, raiz a esquerda):\n");
    mostra(raiz);

    tenta_delete(&raiz, 5);   /* remove 5, 8 e 9 */
    tenta_delete(&raiz, 6);   /* remove uma folha */
    tenta_delete(&raiz, 42);  /* inexistente */
    tenta_delete(&raiz, 1);   /* remove a árvore inteira */

    return 0;
}
