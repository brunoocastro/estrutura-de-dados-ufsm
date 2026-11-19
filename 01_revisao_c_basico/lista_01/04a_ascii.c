/*
 * ============================================================================
 * Arquivo : 04a_ascii.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3 e 0.5)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 4a: quais caracteres ASCII correspondem aos inteiros A com
 *   64 < A < 123? Um laço for imprime a tabela inteira; depois o programa
 *   lê um valor A e mostra só o caractere dele.
 *
 * EXEMPLO REAL
 *   Protocolos seriais de texto (comandos AT, Modbus ASCII) transmitem cada
 *   caractere como um byte com o código ASCII: 'A' = 65, '0' = 48.
 *
 * COMPLEXIDADE (k = tamanho da faixa = 58)
 *   Tabela: O(k) tempo, O(1) espaço. Consulta de um valor: O(1).
 *
 * COMO EXECUTAR
 *   echo "65" | ./run 04a_ascii
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    int a;

    /* Laço: todos os valores da faixa aberta (64, 123), 8 por linha. */
    printf("Caracteres ASCII de 65 a 122:\n");
    for (int codigo = 65; codigo < 123; codigo++)
        printf("%3d='%c'%s", codigo, (char)codigo, (codigo - 64) % 8 == 0 ? "\n" : "  ");
    printf("\n\n");

    printf("Digite A: ");
    int lidos = scanf("%d", &a);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 1) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }

    if (a > 64 && a < 123)
        printf("O valor %d representa o caractere ASCII '%c'.\n", a, (char)a);
    else
        printf("A deve estar no intervalo aberto (64, 123).\n");

    return 0;
}
