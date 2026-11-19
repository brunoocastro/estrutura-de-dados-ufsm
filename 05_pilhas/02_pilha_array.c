/*
 * ============================================================================
 * Arquivo : 02_pilha_array.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Implementação ESTÁTICA (vetor de capacidade fixa) com a convenção
 *   topo = índice do último elemento: vazia quando topo == -1 e cheia quando
 *   topo == CAPACIDADE - 1. A capacidade é propositalmente pequena (3) para o
 *   aluno ver o OVERFLOW (push em pilha cheia) e o UNDERFLOW (pop em vazia).
 *
 * EXEMPLO REAL
 *   Buffer de alarmes de um controlador embarcado com memória reservada em
 *   tempo de compilação (sem malloc, como exigem muitas normas de software
 *   embarcado): quando enche, o alarme novo precisa ser recusado ou tratado.
 *
 * COMPLEXIDADE (n = elementos, C = CAPACIDADE)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   push/pop        O(1)        O(1)        O(1)        O(1)
 *   mostra          O(n)        O(n)        O(n)        O(1)
 *   memória total   O(C) reservada desde o início, usada ou não.
 *   Por quê: `p->dados[++p->topo] = valor` primeiro avança o índice e depois
 *   grava; `p->dados[p->topo--]` lê e depois recua. Uma operação, sem laços.
 *
 * ARMADILHA
 *   Trocar ++p->topo por p->topo++ grava uma posição ANTES do novo topo e,
 *   com a pilha vazia, escreve em dados[-1] (fora do vetor: comportamento
 *   indefinido).
 *
 * COMO EXECUTAR
 *   ./run 02_pilha_array
 * ============================================================================
 */
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#define CAPACIDADE 3

typedef struct {
    int dados[CAPACIDADE];
    int topo;
} Pilha;

void inicializa(Pilha *p) { p->topo = -1; }
int vazia(const Pilha *p) { return p->topo == -1; }
int cheia(const Pilha *p) { return p->topo == CAPACIDADE - 1; }

int push(Pilha *p, int valor) {
    if (cheia(p))
        return 0; /* Overflow da pilha: não há posição disponível. */
    p->dados[++p->topo] = valor;
    return 1;
}

int pop(Pilha *p, int *valor) {
    if (vazia(p))
        return 0; /* Underflow: não é possível remover de pilha vazia. */
    *valor = p->dados[p->topo--];
    return 1;
}

void mostra(const Pilha *p) {
    printf("topo = %d | elementos (topo para base):\n", p->topo);
    for (int i = p->topo; i >= 0; i--) {
        uintptr_t endereco = (uintptr_t)&p->dados[i];
        printf("  dados[%d] = %d | endereco numerico = %" PRIuPTR
               " | distancia da base = %" PRIuPTR " byte(s)%s\n",
               i, p->dados[i], endereco,
               endereco - (uintptr_t)&p->dados[0],
               i == p->topo ? " <- topo" : "");
    }
    if (p->topo < 0)
        printf("  [vazia]\n");
}

int main(void) {
    Pilha pilha;
    int removido;
    inicializa(&pilha);

    printf("sizeof(Pilha) = %zu byte(s)\n", sizeof pilha);
    printf("capacidade maxima = %d elemento(s) = %zu byte(s)\n",
           CAPACIDADE, sizeof pilha.dados);
    printf("limite teorico de elementos por SIZE_MAX = %zu\n",
           SIZE_MAX / sizeof pilha.dados[0]);
    for (int i = 1; i <= 4; i++)
        printf("push(%d): %s\n", i * 10, push(&pilha, i * 10) ? "ok" : "overflow");
    mostra(&pilha);
    while (pop(&pilha, &removido))
        printf("pop() -> %d\n", removido);
    printf("pop em pilha vazia: %s\n", pop(&pilha, &removido) ? "ok" : "underflow");
    return 0;
}
