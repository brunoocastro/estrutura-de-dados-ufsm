/*
 * CONSTANTES: const, #define e enum
 *
 * Compile e execute na pasta 01_tipos_simples:
 *   gcc -std=c11 -Wall -Wextra -pedantic 05_constantes.c -o 05_constantes
 *   ./05_constantes
 */
#include <stdio.h>

#define TAMANHO_MAXIMO 5 /* Macro: substituicao textual feita pelo pre-processador. */

enum { DIAS_DA_SEMANA = 7 }; /* Constante inteira nomeada pelo compilador. */

int main(void) {
    const float PI = 3.1415926535f;
    const int limite = TAMANHO_MAXIMO;
    int valores[TAMANHO_MAXIMO] = {10, 20, 30, 40, 50};

    printf("PI = %f | tamanho = %zu | endereco = %p\n", PI, sizeof(PI), (void *)&PI);
    printf("limite = %d | tamanho = %zu | endereco = %p\n", limite, sizeof(limite), (void *)&limite);
    printf("TAMANHO_MAXIMO = %d\n", TAMANHO_MAXIMO);
    printf("DIAS_DA_SEMANA = %d\n", DIAS_DA_SEMANA);
    printf("primeiro valor = %d | endereco = %p\n", valores[0], (void *)&valores[0]);

    /* PI = 3.0f; // Erro: uma variavel const nao pode ser alterada. */
    printf("const impede alteracoes acidentais depois da inicializacao.\n");
    return 0;
}
