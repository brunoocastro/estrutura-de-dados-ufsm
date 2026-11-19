/*
 * ============================================================================
 * Arquivo : 11_ponteiros_simples.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.7 Ponteiros)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   O primeiro contato com ponteiros: uma variável que guarda o ENDEREÇO de
 *   outra. Os três operadores que importam: &x (endereço de x), p (o endereço
 *   guardado) e *p (o valor que está lá). O desenho mostra as duas "caixas".
 *
 * EXEMPLO REAL
 *   Um driver recebe o endereço de uma variável onde deve gravar a leitura do
 *   sensor (ler_temperatura(&t)): ele não recebe uma cópia, recebe "onde
 *   escrever".
 *
 * COMPLEXIDADE
 *   &x, p e *p são O(1): um ponteiro é só um número (endereço) de tamanho fixo.
 *
 * COMO EXECUTAR
 *   ./run 11_ponteiros_simples
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    int numero = 42;
    int *ponteiro = &numero; /* "ponteiro" guarda o endereço de "numero" */

    printf("=== Variavel e ponteiro ===\n");
    printf("numero = %d\n", numero);
    printf("&numero = %p\n", (void *)&numero);
    printf("ponteiro = %p   (igual a &numero)\n", (void *)ponteiro);
    printf("*ponteiro = %d   (o valor guardado no endereco apontado)\n", *ponteiro);
    printf("&ponteiro = %p   (o ponteiro tambem e uma variavel, com seu proprio endereco)\n",
           (void *)&ponteiro);
    printf("sizeof(numero) = %zu byte(s)\n", sizeof numero);
    printf("sizeof(ponteiro) = %zu byte(s)  (todo endereco tem o mesmo tamanho)\n", sizeof ponteiro);
    printf("sizeof(*ponteiro) = %zu byte(s)  (tamanho do int apontado)\n", sizeof *ponteiro);

    printf("\n=== Desenho da memoria ===\n");
    printf("   ponteiro                     numero\n");
    printf("  +----------------+          +--------+\n");
    printf("  | %-14p | -------> | %6d |\n", (void *)ponteiro, numero);
    printf("  +----------------+          +--------+\n");
    printf("   em %p      em %p\n", (void *)&ponteiro, (void *)&numero);

    /* Escrever por *ponteiro altera "numero": é a mesma caixa de memória. */
    *ponteiro = 100;
    printf("\nDepois de *ponteiro = 100: numero = %d\n", numero);

    /* Armadilha: "int *p;" sem inicializar contém lixo; *p seria UB.
     * Inicialize sempre: int *p = NULL; ou int *p = &algo; */
    return 0;
}
