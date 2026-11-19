/*
 * ============================================================================
 * Arquivo : 08_args.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.5 Strings, 0.6 Funções)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   argc informa quantos argumentos existem; argv é um vetor de ponteiros
 *   para strings (char *argv[]). argv[0] normalmente é o nome do programa e
 *   argv[argc] é sempre NULL.
 *
 * EXEMPLO REAL
 *   Ferramentas de linha de comando para configurar equipamentos:
 *   ./configura --porta /dev/ttyUSB0 --baud 9600
 *
 * COMPLEXIDADE (k = número de argumentos)
 *   Percorrer argv: O(k).
 *
 * COMO EXECUTAR
 *   ./run 08_args um dois tres
 * ============================================================================
 */
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Quantidade de argumentos: %d\n", argc);
    for (int i = 0; i < argc; i++)
        printf("argv[%d] = %s | endereco da string = %p\n",
               i, argv[i], (void *)argv[i]);
    printf("argv[argc] = %p (sempre NULL)\n", (void *)argv[argc]);
    return 0;
}
