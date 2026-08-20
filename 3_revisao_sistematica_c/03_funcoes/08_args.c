/*
 * ARGUMENTOS DA LINHA DE COMANDO
 *
 * argc informa quantos argumentos existem; argv e um vetor de strings.
 * argv[0] normalmente e o nome usado para chamar o programa.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 08_args.c -o 08_args
 * ./08_args um dois tres
 */
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Quantidade de argumentos: %d\n", argc);
    for (int i = 0; i < argc; i++)
        printf("argv[%d] = %s | endereco da string = %p\n",
               i, argv[i], (void *)argv[i]);
    return 0;
}
