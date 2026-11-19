/*
 * DPEE1038 - Tarefa Avaliada 05
 * Lista encadeada recursiva, pilha e fila com alocacao dinamica.
 *
 * Compilacao a partir da raiz do repositorio:
 * gcc -std=c11 -Wall -Wextra -pedantic 6_listas/02_t05_listas.c \
 *   -o /tmp/02_t05_listas
 * /tmp/02_t05_listas
 *
 * Explicacao de remove_item com quatro nos:
 * Considere L -> [10] -> [20] -> [30] -> [20] -> NULL e item = 30.
 * 1. A primeira chamada recebe o endereco de L. *L aponta para 10;
 *    10 nao e 30, entao a funcao chama remove_item(&(no10->prox), 30).
 * 2. A segunda chamada recebe o endereco do campo prox do no 10. *L aponta
 *    para 20; 20 nao e 30, entao chama remove_item(&(no20->prox), 30).
 * 3. A terceira chamada recebe o endereco do campo prox do no 20. *L aponta
 *    para 30; o valor foi encontrado. aux guarda esse no, *L passa a apontar
 *    para o proximo no (o segundo 20), aux e liberado com free e a chamada
 *    retorna 1.
 * 4. A chamada do no 20 retorna esse 1; a chamada do no 10 tambem retorna 1.
 *    A lista resultante e [10] -> [20] -> [20] -> NULL.
 *
 * O parametro struct lista **L aponta para o elo que contem o no atual: pode
 * ser o inicio da lista ou o campo prox de outro no. Assim, ao encontrar o
 * valor, *L = (*L)->prox atualiza o elo correto, inclusive quando remove o
 * primeiro no. A versao do enunciado precisa de `return` na chamada recursiva;
 * sem propagar o resultado, a funcao nao devolve um valor definido nos casos
 * em que a busca avanca para os nos seguintes.
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct lista {
    int valor;
    struct lista *prox;
};

/* Remove a primeira ocorrencia de item; retorna 1 se removeu, senao 0. */
int remove_item(struct lista **L, int item) {
    if (*L == NULL)
        return 0;

    if ((*L)->valor == item) {
        struct lista *aux = *L;
        *L = (*L)->prox;
        free(aux);
        return 1;
    }

    return remove_item(&((*L)->prox), item);
}

/* Retorna recursivamente a quantidade de nos. */
size_t tamanho(const struct lista *L) {
    if (L == NULL)
        return 0;
    return 1 + tamanho(L->prox);
}

/*
 * Percorre a lista recursivamente. Esta solucao atende ao enunciado, mas a
 * verificacao usual e mais eficiente: L == NULL, em O(1), sem recursao.
 */
int vazia(const struct lista *L) {
    if (L == NULL)
        return 1;
    if (L->prox == NULL)
        return 0;
    return vazia(L->prox);
}

/* Conta recursivamente todas as ocorrencias de item. */
size_t conta_item(const struct lista *L, int item) {
    if (L == NULL)
        return 0;
    return (L->valor == item) + conta_item(L->prox, item);
}

/* Insere item no ultimo elo; retorna 0 somente se a alocacao falhar. */
int insere_item_final(struct lista **L, int item) {
    if (*L == NULL) {
        struct lista *novo = malloc(sizeof *novo);
        if (novo == NULL)
            return 0;
        novo->valor = item;
        novo->prox = NULL;
        *L = novo;
        return 1;
    }

    return insere_item_final(&((*L)->prox), item);
}

void imprime_lista(const struct lista *L) {
    for (const struct lista *atual = L; atual != NULL; atual = atual->prox)
        printf("%d -> ", atual->valor);
    puts("NULL");
}

/* Libera os nos iterativamente para nao consumir a pilha de chamadas. */
void libera_lista(struct lista **L) {
    while (*L != NULL) {
        struct lista *aux = *L;
        *L = (*L)->prox;
        free(aux);
    }
}

typedef struct {
    struct lista *topo;
    size_t tamanho;
} Pilha;

/* Insere no topo: O(1). */
int push(Pilha *pilha, int valor) {
    struct lista *novo = malloc(sizeof *novo);
    if (novo == NULL)
        return 0;

    novo->valor = valor;
    novo->prox = pilha->topo;
    pilha->topo = novo;
    pilha->tamanho++;
    return 1;
}

/* Remove do topo: O(1); retorna 0 quando a pilha esta vazia. */
int pop(Pilha *pilha, int *valor) {
    if (pilha->topo == NULL)
        return 0;

    struct lista *removido = pilha->topo;
    pilha->topo = removido->prox;
    *valor = removido->valor;
    free(removido);
    pilha->tamanho--;
    return 1;
}

void pilha_libera(Pilha *pilha) {
    while (pilha->topo != NULL) {
        struct lista *aux = pilha->topo;
        pilha->topo = aux->prox;
        free(aux);
    }
    pilha->tamanho = 0;
}

typedef struct {
    struct lista *inicio;
    struct lista *fim;
    size_t tamanho;
} Fila;

/* Insere no fim: O(1), usando os ponteiros inicio e fim. */
int fila_insere(Fila *fila, int valor) {
    struct lista *novo = malloc(sizeof *novo);
    if (novo == NULL)
        return 0;

    novo->valor = valor;
    novo->prox = NULL;
    if (fila->fim == NULL)
        fila->inicio = novo;
    else
        fila->fim->prox = novo;
    fila->fim = novo;
    fila->tamanho++;
    return 1;
}

/* Remove do inicio: O(1); retorna 0 quando a fila esta vazia. */
int fila_remove(Fila *fila, int *valor) {
    if (fila->inicio == NULL)
        return 0;

    struct lista *removido = fila->inicio;
    fila->inicio = removido->prox;
    if (fila->inicio == NULL)
        fila->fim = NULL;
    *valor = removido->valor;
    free(removido);
    fila->tamanho--;
    return 1;
}

void fila_libera(Fila *fila) {
    while (fila->inicio != NULL) {
        struct lista *aux = fila->inicio;
        fila->inicio = aux->prox;
        free(aux);
    }
    fila->fim = NULL;
    fila->tamanho = 0;
}

int main(void) {
    struct lista *lista = NULL;
    const int valores[] = {10, 20, 30, 20};
    const size_t quantidade = sizeof valores / sizeof valores[0];

    puts("=== Lista recursiva ===");
    printf("Lista inicialmente vazia? %s\n", vazia(lista) ? "sim" : "nao");
    for (size_t i = 0; i < quantidade; i++) {
        if (!insere_item_final(&lista, valores[i])) {
            fputs("Falha ao alocar no para a lista.\n", stderr);
            libera_lista(&lista);
            return EXIT_FAILURE;
        }
    }
    printf("Lista com %zu nos: ", tamanho(lista));
    imprime_lista(lista);
    printf("Ocorrencias de 20: %zu\n", conta_item(lista, 20));
    printf("Lista vazia? %s\n", vazia(lista) ? "sim" : "nao");

    puts("Removendo 30 da lista de quatro nos:");
    printf("Resultado: %s\n", remove_item(&lista, 30) ? "removido" : "nao encontrado");
    imprime_lista(lista);
    printf("Tamanho depois da remocao: %zu\n", tamanho(lista));
    printf("Remocao de 99: %s\n", remove_item(&lista, 99) ? "removido" : "nao encontrado");
    libera_lista(&lista);
    printf("Lista vazia depois de liberar? %s\n", vazia(lista) ? "sim" : "nao");
    printf("Remocao em lista vazia: %s\n",
           remove_item(&lista, 99) ? "removido" : "nao encontrado");

    puts("\n=== Pilha dinamica ===");
    Pilha pilha = {NULL, 0};
    for (int valor = 1; valor <= 3; valor++) {
        if (!push(&pilha, valor)) {
            fputs("Falha ao alocar no para a pilha.\n", stderr);
            pilha_libera(&pilha);
            return EXIT_FAILURE;
        }
    }
    printf("Tamanho da pilha: %zu; pop: ", pilha.tamanho);
    int valor;
    while (pop(&pilha, &valor))
        printf("%d ", valor);
    puts("(ordem LIFO)");
    pilha_libera(&pilha);

    puts("\n=== Fila dinamica ===");
    Fila fila = {NULL, NULL, 0};
    for (int item = 1; item <= 3; item++) {
        if (!fila_insere(&fila, item)) {
            fputs("Falha ao alocar no para a fila.\n", stderr);
            fila_libera(&fila);
            return EXIT_FAILURE;
        }
    }
    printf("Tamanho da fila: %zu; remocoes: ", fila.tamanho);
    while (fila_remove(&fila, &valor))
        printf("%d ", valor);
    puts("(ordem FIFO)");
    fila_libera(&fila);
    return EXIT_SUCCESS;
}
