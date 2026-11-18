/*
 * Lista simplesmente encadeada de inteiros.
 *
 * Cada no guarda um valor e o endereco do proximo no. A lista possui
 * apenas o ponteiro para o inicio; por isso percorrer ate o fim custa O(n).
 * Todo no criado com malloc deve ser liberado com free.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 01_lista_encadeada.c -o /tmp/01_lista_encadeada
 * /tmp/01_lista_encadeada
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *proximo;
} No;

typedef struct {
    No *inicio;
    size_t tamanho;
} Lista;

void lista_inicializa(Lista *lista) {
    lista->inicio = NULL;
    lista->tamanho = 0;
}

/* Insere antes do indice; tamanho e um indice valido para inserir no fim. */
int lista_insere(Lista *lista, size_t indice, int valor) {
    if (indice > lista->tamanho)
        return 0;

    No *novo = malloc(sizeof *novo);
    if (novo == NULL)
        return 0;
    novo->valor = valor;

    if (indice == 0) {
        novo->proximo = lista->inicio;
        lista->inicio = novo;
    } else {
        No *anterior = lista->inicio;
        for (size_t i = 1; i < indice; i++)
            anterior = anterior->proximo;
        novo->proximo = anterior->proximo;
        anterior->proximo = novo;
    }

    lista->tamanho++;
    return 1;
}

/* Remove o indice e, se valor nao for NULL, devolve o valor removido. */
int lista_remove(Lista *lista, size_t indice, int *valor) {
    if (indice >= lista->tamanho)
        return 0;

    No *removido;
    if (indice == 0) {
        removido = lista->inicio;
        lista->inicio = removido->proximo;
    } else {
        No *anterior = lista->inicio;
        for (size_t i = 1; i < indice; i++)
            anterior = anterior->proximo;
        removido = anterior->proximo;
        anterior->proximo = removido->proximo;
    }

    if (valor != NULL)
        *valor = removido->valor;
    free(removido);
    lista->tamanho--;
    return 1;
}

int lista_contem(const Lista *lista, int valor) {
    for (const No *atual = lista->inicio; atual != NULL; atual = atual->proximo) {
        if (atual->valor == valor)
            return 1;
    }
    return 0;
}

void lista_imprime(const Lista *lista) {
    printf("Lista (%zu elemento(s)): ", lista->tamanho);
    if (lista->inicio == NULL) {
        puts("NULL");
        return;
    }

    for (const No *atual = lista->inicio; atual != NULL; atual = atual->proximo) {
        printf("%d -> ", atual->valor);
    }
    puts("NULL");
}

void lista_libera(Lista *lista) {
    No *atual = lista->inicio;
    while (atual != NULL) {
        No *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    lista->inicio = NULL;
    lista->tamanho = 0;
}

int main(void) {
    Lista lista;
    int removido;
    lista_inicializa(&lista);

    puts("Inserindo 10 no inicio e 30 no fim:");
    if (!lista_insere(&lista, 0, 10) || !lista_insere(&lista, lista.tamanho, 30)) {
        fputs("Falha ao alocar memoria para a lista.\n", stderr);
        lista_libera(&lista);
        return EXIT_FAILURE;
    }
    lista_imprime(&lista);

    puts("Inserindo 20 no meio, 5 no inicio e 40 no fim:");
    if (!lista_insere(&lista, 1, 20) || !lista_insere(&lista, 0, 5) ||
        !lista_insere(&lista, lista.tamanho, 40)) {
        fputs("Falha ao alocar memoria para a lista.\n", stderr);
        lista_libera(&lista);
        return EXIT_FAILURE;
    }
    lista_imprime(&lista);

    printf("Busca pelo valor 20: %s\n", lista_contem(&lista, 20) ? "encontrado" : "nao encontrado");
    if (lista_remove(&lista, 2, &removido))
        printf("Removido do indice 2: %d\n", removido);
    lista_imprime(&lista);

    if (!lista_insere(&lista, lista.tamanho + 1, 99))
        puts("Indice invalido: insercao recusada sem alterar a lista.");
    lista_imprime(&lista);

    lista_libera(&lista);
    puts("Depois de liberar os nos:");
    lista_imprime(&lista);
    return EXIT_SUCCESS;
}
