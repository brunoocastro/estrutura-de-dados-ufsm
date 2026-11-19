/*
 * ============================================================================
 * Arquivo : 03_valor_ponteiro.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Funções, 0.7 Ponteiros)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   C sempre passa os parâmetros por valor (cópia). Para alterar uma variável
 *   do chamador, passamos seu ENDEREÇO e usamos * para acessar o valor lá.
 *   Os endereços impressos mostram que a cópia mora em outro lugar.
 *
 * EXEMPLO REAL
 *   scanf("%d", &x) é exatamente isso: scanf precisa do endereço de x para
 *   escrever nele. Esquecer o & é um dos erros mais comuns.
 *
 * COMPLEXIDADE
 *   tenta_alterar e altera: O(1).
 *
 * COMO EXECUTAR
 *   ./run 03_valor_ponteiro
 * ============================================================================
 */
#include <stdio.h>

/* O(1): "valor" é uma variável NOVA, inicializada com a cópia do argumento. */
void tenta_alterar(int valor) {
    valor = 99; /* Altera somente a cópia local. */
    printf("Dentro da funcao, a copia vale %d e mora em %p.\n", valor, (void *)&valor);
}

/* O(1): "endereco" é uma cópia do endereço; *endereco é a variável original. */
void altera(int *endereco) {
    *endereco = 99; /* Altera a variável no endereço recebido. */
    printf("Dentro da funcao, escrevi 99 em %p.\n", (void *)endereco);
}

int main(void) {
    int numero = 10;
    printf("Antes: numero = %d | endereco = %p\n", numero, (void *)&numero);
    tenta_alterar(numero);
    printf("Depois de passagem por valor: numero = %d\n", numero);
    altera(&numero);
    printf("Depois de passar ponteiro: numero = %d\n", numero);
    return 0;
}
