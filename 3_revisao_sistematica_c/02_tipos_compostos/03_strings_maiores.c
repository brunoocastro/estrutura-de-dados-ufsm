/*
 * MANIPULACAO DE STRINGS MAIORES E ENTRADA COM ESPACOS
 *
 * Este exemplo usa buffers fixos, fgets, strlen, strcspn, strncpy, strncat,
 * strstr e strchr. Todas as operacoes respeitam a capacidade dos arrays e
 * preservam o terminador '\0'.
 *
 * Compile e execute nesta pasta:
 *   gcc -std=c11 -Wall -Wextra -pedantic 03_strings_maiores.c -o 03_strings_maiores
 *   ./03_strings_maiores
 */
#include <stdio.h>
#include <string.h>

#define CAPACIDADE 120

/* fgets pode armazenar o '\n'; esta funcao remove a quebra, se existir. */
void remove_quebra_linha(char texto[]) {
    texto[strcspn(texto, "\n")] = '\0';
}

/* Copia no maximo destino_capacidade - 1 caracteres e termina a string. */
void copia_segura(char destino[], size_t destino_capacidade, const char origem[]) {
    if (destino_capacidade == 0)
        return;
    strncpy(destino, origem, destino_capacidade - 1);
    destino[destino_capacidade - 1] = '\0';
}

/* Retorna 1 se a concatenacao couber, ou 0 se ela for truncada. */
int concatena_seguro(char destino[], size_t destino_capacidade, const char origem[]) {
    size_t tamanho_atual = strlen(destino);
    size_t espaco = destino_capacidade - tamanho_atual;

    if (tamanho_atual >= destino_capacidade)
        return 0;
    if (strlen(origem) >= espaco)
        return 0;

    strncat(destino, origem, espaco - 1);
    return 1;
}

int main(void) {
    char nome[CAPACIDADE];
    char cidade[CAPACIDADE];
    char frase[CAPACIDADE * 2];
    char palavra[CAPACIDADE];
    char *encontrada;
    char *separador;

    printf("=== Entrada de strings maiores ===\n");
    printf("Digite seu nome completo: ");
    if (fgets(nome, sizeof nome, stdin) == NULL)
        return 1;
    remove_quebra_linha(nome);

    printf("Digite sua cidade: ");
    if (fgets(cidade, sizeof cidade, stdin) == NULL)
        return 1;
    remove_quebra_linha(cidade);

    /* Monta a frase em etapas, verificando a capacidade a cada concatenacao. */
    frase[0] = '\0';
    if (!concatena_seguro(frase, sizeof frase, "Pessoa: ") ||
        !concatena_seguro(frase, sizeof frase, nome) ||
        !concatena_seguro(frase, sizeof frase, " | Cidade: ") ||
        !concatena_seguro(frase, sizeof frase, cidade)) {
        printf("A frase ficou grande demais para o buffer.\n");
        return 1;
    }

    printf("\n=== Resultado da montagem ===\n");
    printf("%s\n", frase);
    printf("Comprimento da frase: %zu caractere(s)\n", strlen(frase));
    printf("Capacidade do buffer: %zu byte(s)\n", sizeof frase);

    /* strstr localiza uma substring; strchr localiza o primeiro caractere. */
    encontrada = strstr(frase, cidade);
    if (encontrada != NULL)
        printf("A cidade aparece na frase no deslocamento %td.\n", encontrada - frase);

    separador = strchr(frase, '|');
    if (separador != NULL)
        printf("O separador '|' aparece no deslocamento %td.\n", separador - frase);

    printf("\nDigite uma palavra para copiar com limite: ");
    if (fgets(palavra, sizeof palavra, stdin) == NULL)
        return 1;
    remove_quebra_linha(palavra);

    char copia[20];
    copia_segura(copia, sizeof copia, palavra);
    printf("Copia limitada a %zu caracteres: \"%s\"\n", sizeof copia - 1, copia);
    printf("Endereco de frase = %p | tamanho ocupado pelo array = %zu\n",
           (void *)frase, sizeof frase);
    printf("Importante: strlen mede o texto; sizeof mede a capacidade do array.\n");
    return 0;
}
