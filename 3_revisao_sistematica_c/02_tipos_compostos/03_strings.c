/*
 * STRINGS EM C
 *
 * C nao possui um tipo primitivo string: uma string e um array de char
 * terminado obrigatoriamente por '\0' (byte de valor zero).
 *
 * Compile e execute nesta pasta:
 *   gcc -std=c11 -Wall -Wextra -pedantic 03_strings.c -o 03_strings
 *   ./03_strings
 */
#include <stdio.h>
#include <string.h>

int main(void) {
    char texto[] = "C"; /* tamanho implicito: {'C', '\0'}, portanto 2 bytes. */
    char explicita[6] = {'O', 'l', 'a', '!', '\0', '\0'};
    char capacidade[20] = "mensagem"; /* capacidade 20, conteudo menor. */
    const char *literal = "nao altere este literal";

    printf("=== Tamanho logico e capacidade ===\n");
    printf("texto = %s | strlen = %zu | sizeof = %zu\n", texto, strlen(texto), sizeof texto);
    printf("explicita = %s | strlen = %zu | sizeof = %zu\n", explicita, strlen(explicita), sizeof explicita);
    printf("capacidade = %s | strlen = %zu | sizeof = %zu\n", capacidade, strlen(capacidade), sizeof capacidade);
    printf("literal = %s | sizeof ponteiro = %zu\n", literal, sizeof literal);

    printf("\n=== Bytes, enderecos e terminador ===\n");
    for (size_t i = 0; i < sizeof capacidade; i++) {
        printf("capacidade[%zu] = %d ('%c') | endereco = %p\n",
               i, capacidade[i], capacidade[i] == '\0' ? '.' : capacidade[i],
               (void *)&capacidade[i]);
    }

    printf("\nstrlen conta caracteres antes de \\0; sizeof conta toda a capacidade do array.\n");
    printf("Para ler texto com espacos, fgets e mais segura que scanf(\"%%s\").\n");
    printf("Um literal de string nao deve ser alterado; const char * documenta isso.\n");
    return 0;
}
