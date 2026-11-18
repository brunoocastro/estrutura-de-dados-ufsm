/*
 * Árvore binária de busca (ABB) de inteiros.
 *
 * Cada nó tem no máximo dois filhos: esquerdo e direito. Além disso, a ABB
 * mantém a regra de ordenação: valores menores ficam à esquerda e maiores à
 * direita. Valores repetidos são ignorados.
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *esquerda;
    struct No *direita;
} No;

No *cria_no(int valor) {
    No *no = malloc(sizeof *no);
    if (no == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    no->valor = valor;
    no->esquerda = NULL;
    no->direita = NULL;
    return no;
}

/* Retorna a (possivelmente nova) raiz da subárvore. */
No *insere(No *raiz, int valor) {
    if (raiz == NULL) return cria_no(valor);
    if (valor < raiz->valor) {
        raiz->esquerda = insere(raiz->esquerda, valor);
    } else if (valor > raiz->valor) {
        raiz->direita = insere(raiz->direita, valor);
    }
    return raiz;
}

const No *busca(const No *raiz, int valor) {
    while (raiz != NULL && raiz->valor != valor) {
        raiz = valor < raiz->valor ? raiz->esquerda : raiz->direita;
    }
    return raiz;
}

No *minimo(No *raiz) {
    while (raiz != NULL && raiz->esquerda != NULL) raiz = raiz->esquerda;
    return raiz;
}

/*
 * Remove `valor` e retorna a nova raiz da subárvore. Casos:
 *  - folha: basta liberar;
 *  - um filho: o filho ocupa o lugar do nó;
 *  - dois filhos: copia o sucessor (menor da subárvore direita) e o remove.
 */
No *remove_valor(No *raiz, int valor) {
    if (raiz == NULL) return NULL;
    if (valor < raiz->valor) {
        raiz->esquerda = remove_valor(raiz->esquerda, valor);
    } else if (valor > raiz->valor) {
        raiz->direita = remove_valor(raiz->direita, valor);
    } else if (raiz->esquerda == NULL || raiz->direita == NULL) {
        No *filho = raiz->esquerda != NULL ? raiz->esquerda : raiz->direita;
        free(raiz);
        return filho;
    } else {
        No *sucessor = minimo(raiz->direita);
        raiz->valor = sucessor->valor;
        raiz->direita = remove_valor(raiz->direita, sucessor->valor);
    }
    return raiz;
}

void pre_ordem(const No *r) {
    if (r == NULL) return;
    printf("%d ", r->valor);
    pre_ordem(r->esquerda);
    pre_ordem(r->direita);
}

void em_ordem(const No *r) {
    if (r == NULL) return;
    em_ordem(r->esquerda);
    printf("%d ", r->valor);
    em_ordem(r->direita);
}

void pos_ordem(const No *r) {
    if (r == NULL) return;
    pos_ordem(r->esquerda);
    pos_ordem(r->direita);
    printf("%d ", r->valor);
}

int conta_nos(const No *r) {
    return r == NULL ? 0 : 1 + conta_nos(r->esquerda) + conta_nos(r->direita);
}

int conta_folhas(const No *r) {
    if (r == NULL) return 0;
    if (r->esquerda == NULL && r->direita == NULL) return 1;
    return conta_folhas(r->esquerda) + conta_folhas(r->direita);
}

int altura(const No *r) {
    if (r == NULL) return -1;
    int he = altura(r->esquerda);
    int hd = altura(r->direita);
    return 1 + (he > hd ? he : hd);
}

void libera(No *r) {
    if (r == NULL) return;
    libera(r->esquerda);
    libera(r->direita);
    free(r);
}

static void imprime_percursos(const No *raiz) {
    printf("  pre-ordem: "); pre_ordem(raiz); printf("\n");
    printf("  em ordem:  "); em_ordem(raiz);  printf("\n");
    printf("  pos-ordem: "); pos_ordem(raiz); printf("\n");
}

int main(void) {
    /*
     *            50
     *          /    \
     *        30      70
     *       /  \    /  \
     *      20  40  60  80
     */
    int valores[] = {50, 30, 70, 20, 40, 60, 80};
    No *raiz = NULL;
    for (size_t i = 0; i < sizeof valores / sizeof valores[0]; i++) {
        raiz = insere(raiz, valores[i]);
    }

    printf("Percursos:\n");
    imprime_percursos(raiz);
    printf("Nos: %d | Folhas: %d | Altura: %d\n",
           conta_nos(raiz), conta_folhas(raiz), altura(raiz));
    printf("Busca 60: %s\n", busca(raiz, 60) ? "encontrado" : "nao encontrado");
    printf("Busca 65: %s\n", busca(raiz, 65) ? "encontrado" : "nao encontrado");

    printf("\nRemove 20 (folha), 70 (dois filhos) e 50 (raiz):\n");
    raiz = remove_valor(raiz, 20);
    raiz = remove_valor(raiz, 70);
    raiz = remove_valor(raiz, 50);
    imprime_percursos(raiz);
    printf("Nos: %d | Altura: %d\n", conta_nos(raiz), altura(raiz));

    libera(raiz);
    return 0;
}
