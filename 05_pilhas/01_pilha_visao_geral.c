/*
 * ============================================================================
 * Arquivo : 01_pilha_visao_geral.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidades 2.1 e 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   A pilha como TAD (tipo abstrato de dados): vetor + topo agrupados numa
 *   struct, e as operações clássicas
 *     push  -> insere no topo
 *     pop   -> remove e devolve o topo
 *     peek  -> consulta o topo sem remover
 *     esta_vazia / esta_cheia
 *   Como cada função recebe um ponteiro `Pilha *`, o programa pode ter várias
 *   pilhas independentes (no 00_pilha_basica.c só havia uma, global).
 *   A impressão mostra o endereço de cada posição: elementos vizinhos ficam a
 *   sizeof(int) bytes de distância, porque o vetor é CONTÍGUO na memória.
 *
 * EXEMPLO REAL
 *   Pilha de peças num magazine alimentador vertical de uma célula de
 *   montagem: a peça colocada por último é a primeira a ser retirada.
 *
 * COMPLEXIDADE (n = elementos na pilha)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   push/pop/peek   O(1)        O(1)        O(1)        O(1)
 *   imprime_pilha   O(n)        O(n)        O(n)        O(1)
 *   Por quê: o topo é um índice; acessar valores[topo] é aritmética de
 *   endereço (base + topo * sizeof(int)), sem percorrer nada.
 *
 * CONVENÇÃO DO TOPO
 *   Aqui `topo` = quantidade de elementos = próxima posição livre (vazia:
 *   topo == 0). O 02_pilha_array.c usa a outra convenção comum, topo = índice
 *   do último elemento (vazia: topo == -1). As duas são corretas; o que não
 *   pode é misturar as duas no mesmo código (erro clássico de off-by-one).
 *
 * COMO EXECUTAR
 *   ./run 01_pilha_visao_geral
 * ============================================================================
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#define CAPACIDADE 5

typedef struct {
    int valores[CAPACIDADE];
    size_t topo; /* quantidade de elementos; também é o índice da próxima posição */
} Pilha;

/* O(1). Pós-condição: pilha vazia. Não é preciso zerar o vetor: o que está
 * acima de `topo` é considerado lixo e nunca é lido. */
void inicializa(Pilha *pilha) {
    pilha->topo = 0;
}

int esta_vazia(const Pilha *pilha) {
    return pilha->topo == 0;
}

int esta_cheia(const Pilha *pilha) {
    return pilha->topo == CAPACIDADE;
}

/* O(1). Retorna 1 se inseriu, 0 se a pilha estava cheia (overflow). */
int push(Pilha *pilha, int valor) {
    if (esta_cheia(pilha))
        return 0;
    pilha->valores[pilha->topo] = valor;
    pilha->topo++;
    return 1;
}

/* O(1). O valor sai por ponteiro e o retorno indica sucesso; assim qualquer
 * int (inclusive -1) pode ser guardado. Retorna 0 em pilha vazia (underflow). */
int pop(Pilha *pilha, int *valor) {
    if (esta_vazia(pilha))
        return 0;
    pilha->topo--;
    *valor = pilha->valores[pilha->topo];
    return 1;
}

/* O(1). Igual ao pop, mas não altera o topo. */
int peek(const Pilha *pilha, int *valor) {
    if (esta_vazia(pilha))
        return 0;
    *valor = pilha->valores[pilha->topo - 1];
    return 1;
}

/* O(n): do topo para a base, com o endereço de cada posição. uintptr_t é um
 * inteiro capaz de guardar um endereço; com ele podemos subtrair endereços e
 * ver a distância em bytes até a base. */
void imprime_pilha(const Pilha *pilha) {
    printf("\nPilha (topo para base):\n");
    if (esta_vazia(pilha)) {
        printf("  [vazia]\n");
        return;
    }
    for (size_t i = pilha->topo; i > 0; i--) {
        uintptr_t endereco = (uintptr_t)&pilha->valores[i - 1];
        printf("  valores[%zu] = %d | endereco numerico = %" PRIuPTR
               " | distancia da base = %" PRIuPTR " byte(s)%s\n",
               i - 1, pilha->valores[i - 1], endereco,
               endereco - (uintptr_t)&pilha->valores[0],
               i == pilha->topo ? " <- topo" : "");
    }
}

int main(void) {
    Pilha pilha;
    int valor;
    inicializa(&pilha);

    printf("=== Pilha LIFO ===\n");
    printf("capacidade maxima = %d elemento(s) = %zu byte(s)\n",
           CAPACIDADE, sizeof pilha.valores);
    printf("limite teorico de elementos por SIZE_MAX = %zu\n",
           SIZE_MAX / sizeof pilha.valores[0]);
    printf("push(10): %s\n", push(&pilha, 10) ? "ok" : "pilha cheia");
    printf("push(20): %s\n", push(&pilha, 20) ? "ok" : "pilha cheia");
    printf("push(30): %s\n", push(&pilha, 30) ? "ok" : "pilha cheia");

    imprime_pilha(&pilha);
    if (peek(&pilha, &valor))
        printf("peek: topo = %d (continua na pilha)\n", valor);
    while (pop(&pilha, &valor))
        printf("pop: removido = %d\n", valor);

    printf("pilha vazia: %s\n", esta_vazia(&pilha) ? "sim" : "nao");
    printf("pop em pilha vazia: %s\n", pop(&pilha, &valor) ? "ok" : "underflow");
    return 0;
}
