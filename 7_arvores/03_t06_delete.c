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
    if (raiz == NULL) return;
    libera(raiz->esquerda);
    libera(raiz->direita);
    free(raiz);
}

/*
 * Remove o ramo com raiz em `valor`. Recebe o endereço do ponteiro que aponta
 * para a subárvore, para poder anular o ponteiro do pai (ou a própria raiz).
 * Retorna 1 se removeu, 0 se o valor não existe.
 */
int delete(No **raiz, int valor) {
    if (*raiz == NULL) return 0;
    if ((*raiz)->valor == valor) {
        libera(*raiz);
        *raiz = NULL;
        return 1;
    }
    return delete(&(*raiz)->esquerda, valor) || delete(&(*raiz)->direita, valor);
}

/* Imprime a árvore deitada: direita em cima, esquerda embaixo. */
void imprime(const No *raiz, int nivel) {
    if (raiz == NULL) return;
    imprime(raiz->direita, nivel + 1);
    for (int i = 0; i < nivel; i++) printf("    ");
    printf("%d\n", raiz->valor);
    imprime(raiz->esquerda, nivel + 1);
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
