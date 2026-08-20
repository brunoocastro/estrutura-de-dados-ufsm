/*
 * PALINDROMO RECURSIVO
 *
 * Compara as pontas e reduz o intervalo. A string deve ser fornecida sem
 * espacos para manter o exemplo focado na recursao.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 05_palindromo.c -o 05_palindromo
 * ./05_palindromo
 */
#include <stdio.h>
#include <string.h>

int palindromo(const char texto[], int esquerda, int direita, int nivel) {
    printf("%*scompara texto[%d] = '%c' com texto[%d] = '%c'\n",
           nivel * 2, "", esquerda, texto[esquerda], direita, texto[direita]);
    if (esquerda >= direita)
        return 1;
    if (texto[esquerda] != texto[direita])
        return 0;
    return palindromo(texto, esquerda + 1, direita - 1, nivel + 1);
}

int main(void) {
    char texto[80];
    printf("Informe uma palavra: ");
    if (scanf("%79s", texto) != 1)
        return 1;
    printf("Resultado: %s\n", palindromo(texto, 0, (int)strlen(texto) - 1, 0)
                            ? "e palindromo" : "nao e palindromo");
    return 0;
}
