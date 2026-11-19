/*
 * ============================================================================
 * Arquivo : 01_lista_encadeada.c
 * Módulo  : 07 — Listas encadeadas           (Ementa: Unidade 1.3 / 0.7)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Implementação de referência de uma lista simplesmente encadeada de
 *   inteiros como TAD: inicializar, inserir e remover por índice, consultar
 *   por índice, buscar valor, imprimir e liberar. Cada nó é criado com malloc
 *   e devolvido com free. A estrutura é "recorrente": um nó contém um
 *   ponteiro para outro nó do mesmo tipo (struct No *proximo).
 *
 * EXEMPLO REAL
 *   Lista de receitas (setpoints) carregadas num controlador de batelada: o
 *   número de receitas muda em tempo de execução, elas são inseridas e
 *   removidas em qualquer posição, e não se quer reservar um vetor enorme
 *   "por garantia".
 *
 * COMPLEXIDADE (n = tamanho da lista, i = índice)
 *   Operação              Melhor   Pior    Espaço extra
 *   lista_insere(0)       O(1)     O(1)    O(1)
 *   lista_insere(i)       O(1)     O(n)    O(1)   (caminha i-1 nós)
 *   lista_remove(i)       O(1)     O(n)    O(1)
 *   lista_obtem(i)        O(1)     O(n)    O(1)   (sem acesso direto por índice)
 *   lista_contem          O(1)     O(n)    O(1)
 *   lista_imprime/libera  O(n)     O(n)    O(1)
 *   Por quê: religar ponteiros é O(1); o custo vem de CAMINHAR até a posição,
 *   pois só conhecemos o endereço do primeiro nó. Inserir no fim é O(n) aqui
 *   porque a lista não guarda ponteiro para o último nó.
 *
 * COMO EXECUTAR
 *   ./run 01_lista_encadeada
 *   Versões passo a passo: 02_lista_passo_a_passo, 03_vetor_vs_lista.
 * ============================================================================
 */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *proximo;   /* NULL no último nó */
} No;

typedef struct {
    No *inicio;           /* NULL quando a lista está vazia */
    size_t tamanho;       /* mantido a cada operação: consultar é O(1) */
} Lista;

/* O(1). Deve ser chamada antes de qualquer outra operação: uma Lista não
 * inicializada tem lixo em 'inicio', e percorrê-la é comportamento indefinido. */
void lista_inicializa(Lista *lista) {
    lista->inicio = NULL;
    lista->tamanho = 0;
}

/*
 * Insere 'valor' de modo que ele passe a ocupar a posição 'indice'.
 * Índices válidos: 0..tamanho (tamanho = inserir no fim).
 * Retorna 1 em sucesso; 0 se o índice for inválido ou faltar memória
 * (nesses casos a lista não é alterada).
 * O(indice): caminha até o nó anterior à posição.
 */
int lista_insere(Lista *lista, size_t indice, int valor) {
    if (indice > lista->tamanho)
        return 0;

    No *novo = malloc(sizeof *novo);
    if (novo == NULL)
        return 0;
    novo->valor = valor;

    if (indice == 0) {
        /* Caso especial: não existe nó anterior, quem muda é 'inicio'.
         * (02_lista_passo_a_passo.c elimina este if com No **;
         *  06_lista_sentinela.c elimina com um nó sentinela.) */
        novo->proximo = lista->inicio;
        lista->inicio = novo;
    } else {
        No *anterior = lista->inicio;
        for (size_t i = 1; i < indice; i++)
            anterior = anterior->proximo;
        /* A ordem importa: primeiro o novo aponta para o sucessor; se
         * fizéssemos anterior->proximo = novo antes, o endereço do sucessor
         * seria perdido e o resto da lista vazaria. */
        novo->proximo = anterior->proximo;
        anterior->proximo = novo;
    }

    lista->tamanho++;
    return 1;
}

/*
 * Remove o nó da posição 'indice' (0..tamanho-1). Se 'valor' não for NULL,
 * devolve nele o valor removido. Retorna 1 em sucesso, 0 se o índice for
 * inválido (inclusive com a lista vazia).
 * O(indice).
 */
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
        anterior->proximo = removido->proximo;   /* o anterior "pula" o removido */
    }

    if (valor != NULL)
        *valor = removido->valor;   /* ler ANTES do free: depois seria uso de memória liberada */
    free(removido);
    lista->tamanho--;
    return 1;
}

/* Copia em *valor o elemento da posição 'indice'. Retorna 0 se o índice for
 * inválido. O(indice): ao contrário do vetor, não há como saltar direto. */
int lista_obtem(const Lista *lista, size_t indice, int *valor) {
    if (indice >= lista->tamanho)
        return 0;
    const No *atual = lista->inicio;
    for (size_t i = 0; i < indice; i++)
        atual = atual->proximo;
    *valor = atual->valor;
    return 1;
}

/* Retorna 1 se 'valor' está na lista. O(n) no pior caso (busca sequencial). */
int lista_contem(const Lista *lista, int valor) {
    for (const No *atual = lista->inicio; atual != NULL; atual = atual->proximo) {
        if (atual->valor == valor)
            return 1;
    }
    return 0;
}

/* Imprime no formato [5|*]-->[10|*]-->NULL. O(n). */
void lista_imprime(const Lista *lista) {
    printf("Lista (%zu elemento(s)): inicio-->", lista->tamanho);
    for (const No *atual = lista->inicio; atual != NULL; atual = atual->proximo)
        printf("[%d|*]-->", atual->valor);
    puts("NULL");
}

/* Libera todos os nós. O(n). O próximo é guardado ANTES do free: ler
 * atual->proximo depois de free(atual) é comportamento indefinido. */
void lista_libera(Lista *lista) {
    No *atual = lista->inicio;
    while (atual != NULL) {
        No *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    lista->inicio = NULL;   /* a lista volta a ser uma lista vazia válida */
    lista->tamanho = 0;
}

int main(void) {
    Lista lista;
    int valor;
    lista_inicializa(&lista);

    puts("Lista recém-inicializada:");
    lista_imprime(&lista);
    if (!lista_remove(&lista, 0, NULL))
        puts("Remover de lista vazia: recusado sem alterar nada.");

    puts("\nInserindo 10 no início e 30 no fim:");
    if (!lista_insere(&lista, 0, 10) || !lista_insere(&lista, lista.tamanho, 30)) {
        fputs("Falha ao alocar memória para a lista.\n", stderr);
        lista_libera(&lista);
        return EXIT_FAILURE;
    }
    lista_imprime(&lista);

    puts("\nInserindo 20 no meio (índice 1), 5 no início e 40 no fim:");
    if (!lista_insere(&lista, 1, 20) || !lista_insere(&lista, 0, 5) ||
        !lista_insere(&lista, lista.tamanho, 40)) {
        fputs("Falha ao alocar memória para a lista.\n", stderr);
        lista_libera(&lista);
        return EXIT_FAILURE;
    }
    lista_imprime(&lista);

    if (lista_obtem(&lista, 3, &valor))
        printf("\nElemento no índice 3: %d (precisou caminhar 3 nós)\n", valor);
    printf("Busca pelo valor 20: %s\n", lista_contem(&lista, 20) ? "encontrado" : "não encontrado");
    printf("Busca pelo valor 99: %s\n", lista_contem(&lista, 99) ? "encontrado" : "não encontrado");

    puts("\nRemovendo o meio (índice 2), o início (0) e o fim (tamanho-1):");
    if (lista_remove(&lista, 2, &valor))
        printf("  removido do índice 2: %d\n", valor);
    if (lista_remove(&lista, 0, &valor))
        printf("  removido do índice 0: %d\n", valor);
    if (lista_remove(&lista, lista.tamanho - 1, &valor))
        printf("  removido do fim: %d\n", valor);
    lista_imprime(&lista);

    puts("\nÍndices inválidos:");
    if (!lista_insere(&lista, lista.tamanho + 1, 99))
        puts("  inserir em tamanho+1: recusado.");
    if (!lista_remove(&lista, lista.tamanho, NULL))
        puts("  remover em tamanho: recusado.");
    if (!lista_obtem(&lista, 7, &valor))
        puts("  obter índice 7: recusado.");
    lista_imprime(&lista);

    lista_libera(&lista);
    puts("\nDepois de liberar os nós:");
    lista_imprime(&lista);
    return EXIT_SUCCESS;
}
