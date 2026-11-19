/*
 * ============================================================================
 * Arquivo : 00_pilha_basica.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Primeiro contato com a regra LIFO (Last In, First Out): o último valor
 *   colocado é o primeiro retirado. É a versão mais simples possível:
 *   - um vetor global de inteiros e uma variável global `topo`;
 *   - sem struct e sem ponteiros escritos pelo programador;
 *   - duas operações: coloca (push) e retira (pop).
 *   `topo` guarda a PRÓXIMA posição livre, que é também a quantidade de
 *   elementos: topo == 0 indica pilha vazia e topo == CAPACIDADE, pilha cheia.
 *
 * EXEMPLO REAL
 *   Uma pilha de bandejas no restaurante universitário: a bandeja colocada por
 *   último fica em cima e é a primeira a sair.
 *
 * COMPLEXIDADE (n = elementos na pilha)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   coloca (push)   O(1)        O(1)        O(1)        O(1)
 *   retira (pop)    O(1)        O(1)        O(1)        O(1)
 *   mostra          O(n)        O(n)        O(n)        O(1)
 *   Por quê: push/pop mexem só na posição `topo`; nada é deslocado.
 *   O vetor reserva CAPACIDADE posições desde o início, mesmo vazio.
 *
 * LIMITAÇÃO (de propósito)
 *   Com variáveis globais só existe UMA pilha no programa. O arquivo
 *   01_pilha_visao_geral.c resolve isso agrupando vetor e topo numa struct.
 *
 * COMO EXECUTAR
 *   ./run 00_pilha_basica
 * ============================================================================
 */
#include <stdio.h>

#define CAPACIDADE 5

int pilha[CAPACIDADE];
int topo = 0; /* próxima posição livre == quantidade de elementos */

/* O(1): grava na posição livre e avança o topo. Retorna 0 se a pilha estiver
 * cheia (overflow), 1 se deu certo. */
int coloca(int valor) {
    if (topo == CAPACIDADE) {
        printf("Nao foi possivel colocar %d: pilha cheia (overflow).\n", valor);
        return 0;
    }
    pilha[topo] = valor;
    topo++;
    return 1;
}

/* O(1): recua o topo e devolve o valor que estava lá.
 * ARMADILHA: -1 é usado como "pilha vazia", então esta versão não consegue
 * guardar o valor -1 sem ambiguidade. As versões seguintes (01, 02) devolvem
 * o valor por ponteiro e usam o retorno só para indicar sucesso/falha. */
int retira(void) {
    if (topo == 0)
        return -1; /* underflow: pilha vazia */
    topo--;
    return pilha[topo]; /* o valor continua no vetor, mas fica "fora" da pilha */
}

/* O(n): imprime do topo para a base. */
void mostra(void) {
    printf("Pilha (topo para base): ");
    if (topo == 0) {
        printf("vazia\n");
        return;
    }
    for (int i = topo - 1; i >= 0; i--)
        printf("%d%s", pilha[i], i == 0 ? "\n" : " ");
}

int main(void) {
    int valor_retirado;

    printf("=== Pilha basica ===\n");
    mostra();

    for (int v = 10; v <= 50; v += 10) {
        printf("coloca(%d)\n", v);
        coloca(v);
        mostra();
    }

    /* Tenta colocar um valor a mais do que a capacidade da pilha. */
    printf("coloca(60)\n");
    if (coloca(60) == 0)
        printf("-> overflow: a pilha tem so %d posicoes.\n", CAPACIDADE);
    mostra();

    for (int k = 0; k < 2; k++) {
        valor_retirado = retira();
        if (valor_retirado != -1)
            printf("retira() -> %d\n", valor_retirado);
        mostra();
    }

    printf("Regra LIFO: 50 e 40, os ultimos a entrar, foram os primeiros a sair;\n"
           "10, o primeiro a entrar, continua no fundo.\n");
    return 0;
}
