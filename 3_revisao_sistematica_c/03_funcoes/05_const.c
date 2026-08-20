/*
 * const EM PARAMETROS
 *
 * const documenta que uma funcao nao deve modificar os dados apontados.
 * Isso permite passar arrays e strings somente para leitura.
 *
 * gcc -std=c11 -Wall -Wextra -pedantic 05_const.c -o 05_const
 * ./05_const
 */
#include <stdio.h>

void imprime(const int valores[], size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++)
        printf("%d%s", valores[i], i + 1 == tamanho ? "\n" : " ");
}

size_t comprimento(const char texto[]) {
    size_t tamanho = 0;
    while (texto[tamanho] != '\0')
        tamanho++;
    return tamanho;
}

int main(void) {
    const int valores[] = {4, 8, 15, 16, 23, 42};
    const char mensagem[] = "C";
    imprime(valores, sizeof valores / sizeof valores[0]);
    printf("comprimento de %s = %zu\n", mensagem, comprimento(mensagem));
    printf("const protege os dados contra alteracoes dentro da funcao.\n");
    return 0;
}
