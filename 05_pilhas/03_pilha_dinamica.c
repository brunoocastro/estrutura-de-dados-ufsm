/*
 * ============================================================================
 * Arquivo : 03_pilha_dinamica.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Implementação DINÂMICA (encadeada): cada push aloca um nó no heap com
 *   malloc e o liga ao topo antigo; cada pop desliga o nó do topo e o libera
 *   com free. Não existe capacidade fixa: o limite é a memória disponível, e o
 *   "overflow" vira malloc devolvendo NULL.
 *
 *       topo
 *        |
 *        v
 *     [50|*]-->[40|*]-->[30|*]-->[20|*]-->[10|/]--> NULL
 *
 *   O topo fica no INÍCIO da lista: inserir e remover no início custa O(1).
 *   (Se o topo fosse o fim de uma lista simples, o pop precisaria percorrer a
 *   lista inteira para achar o penúltimo nó: O(n).)
 *
 * EXEMPLO REAL
 *   Pilha de contextos/menus abertos numa IHM cujo número de telas abertas
 *   não é conhecido de antemão.
 *
 * COMPLEXIDADE (n = elementos)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   push            O(1)        O(1)        O(1)        O(1) por nó
 *   pop             O(1)        O(1)        O(1)        O(1)
 *   imprime/libera  O(n)        O(n)        O(n)        O(1)
 *   Memória: n * sizeof(No) (valor + ponteiro), mais o custo interno de cada
 *   malloc. Por elemento, gasta mais que o vetor; em compensação, só aloca o
 *   que usa.
 *
 * ARMADILHAS
 *   - Ler removido->proximo DEPOIS do free (uso após liberação).
 *   - Esquecer de liberar os nós restantes no fim (vazamento de memória).
 *
 * COMO EXECUTAR
 *   ./run 03_pilha_dinamica
 * ============================================================================
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *proximo;
} No;

typedef struct {
    No *topo;
    size_t tamanho;
} Pilha;

void inicializa(Pilha *pilha) {
    pilha->topo = NULL;
    pilha->tamanho = 0;
}

/* O(1): o novo nó aponta para o topo antigo e passa a ser o topo.
 * Retorna 0 se malloc falhar (é o "overflow" da versão dinâmica). */
int push(Pilha *pilha, int valor) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL)
        return 0;
    novo->valor = valor;
    novo->proximo = pilha->topo;
    pilha->topo = novo;
    pilha->tamanho++;
    return 1;
}

/* O(1): guarda o endereço do nó do topo, avança o topo para o próximo e só
 * então libera o nó antigo. A ordem importa: depois do free, ler
 * removido->proximo seria uso de memória já liberada. */
int pop(Pilha *pilha, int *valor) {
    if (pilha->topo == NULL)
        return 0;
    No *removido = pilha->topo;
    *valor = removido->valor;
    pilha->topo = removido->proximo;
    free(removido);
    pilha->tamanho--;
    return 1;
}

void imprime_pilha(const Pilha *pilha) {
    printf("Pilha (topo para base):\n");
    if (pilha->topo == NULL) {
        printf("  [vazia]\n");
        return;
    }
    size_t indice = 0;
    for (const No *no = pilha->topo; no != NULL; no = no->proximo, indice++)
        printf("  no[%zu] = %d | endereco numerico = %" PRIuPTR
               " | proximo = %" PRIuPTR " | tamanho do no = %zu byte(s)%s\n",
               indice, no->valor, (uintptr_t)no, (uintptr_t)no->proximo,
               sizeof *no, indice == 0 ? " <- topo" : "");
}

/* O(n): esvazia a pilha liberando todos os nós. Chame sempre ao terminar,
 * mesmo que a pilha pareça vazia: é barato e evita vazamentos. */
void libera(Pilha *pilha) {
    int ignorado;
    while (pop(pilha, &ignorado))
        ;
}

int main(void) {
    Pilha pilha;
    int valor;
    inicializa(&pilha);

    printf("=== Pilha dinamica ===\n");
    printf("tamanho de cada no = %zu byte(s)\n", sizeof(No));
    printf("Nao ha capacidade fixa: o limite pratico depende da memoria disponivel.\n");
    for (int i = 1; i <= 5; i++) {
        /* push é chamado ANTES do printf: a ordem de avaliação dos argumentos
         * de uma função não é especificada em C, então ler pilha.tamanho no
         * mesmo printf que chama push poderia mostrar o valor antigo. */
        int ok = push(&pilha, i * 10);
        printf("push(%d): %s | tamanho = %zu\n", i * 10, ok ? "ok" : "falha",
               pilha.tamanho);
    }
    imprime_pilha(&pilha);
    while (pop(&pilha, &valor))
        printf("pop() -> %d | tamanho = %zu\n", valor, pilha.tamanho);
    libera(&pilha);
    return 0;
}
