/*
 * ============================================================================
 * Arquivo : 05_strings_maiores.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.5 Strings)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ler textos com espaços (fgets), remover o '\n', copiar e concatenar sem
 *   estourar o buffer e procurar caracteres/substrings (strchr, strstr).
 *   Sem entrada (stdin vazio), o programa usa textos de exemplo.
 *
 * EXEMPLO REAL
 *   Montar a linha "Pessoa: ... | Cidade: ..." é o mesmo que montar uma linha
 *   de log ou um telegrama para enviar por serial: o buffer tem tamanho fixo e
 *   um estouro corrompe memória vizinha (falha clássica de segurança).
 *
 * COMPLEXIDADE (n = comprimento das strings envolvidas)
 *   Função                 Custo        Por quê
 *   fgets                  O(n)         copia até '\n' ou até encher o buffer
 *   strcspn / strchr       O(n)         percorrem a string procurando
 *   copia_segura           O(n)         copia no máximo capacidade - 1 chars
 *   concatena_seguro       O(|d|+|o|)   precisa achar o fim do destino (strlen)
 *   strstr (ingênuo)       O(n * m)     n = texto, m = padrão (pior caso)
 *
 * COMO EXECUTAR
 *   ./run 05_strings_maiores
 *   ./run 05_strings_maiores < /dev/null   (usa os textos de exemplo)
 * ============================================================================
 */
#include <stdio.h>
#include <string.h>

#define CAPACIDADE 120

/* O(n): fgets pode armazenar o '\n'; esta função o troca por '\0', se existir.
 * strcspn devolve o índice do primeiro '\n' (ou o comprimento, se não houver). */
static void remove_quebra_linha(char texto[]) {
    texto[strcspn(texto, "\n")] = '\0';
}

/* O(n): lê uma linha; se a entrada acabou (EOF), usa o texto padrão.
 * Isso permite rodar o exemplo com stdin vazio. */
static void le_linha(const char *pergunta, char destino[], size_t capacidade,
                     const char *padrao) {
    printf("%s", pergunta);
    if (fgets(destino, (int)capacidade, stdin) == NULL) {
        snprintf(destino, capacidade, "%s", padrao);
        printf("%s  (exemplo: entrada vazia)\n", destino);
        return;
    }
    remove_quebra_linha(destino);
}

/* O(n): copia no máximo destino_capacidade - 1 caracteres e SEMPRE termina a
 * string. Cuidado: strncpy sozinho NÃO coloca '\0' se a origem for longa. */
static void copia_segura(char destino[], size_t destino_capacidade, const char origem[]) {
    if (destino_capacidade == 0)
        return;
    strncpy(destino, origem, destino_capacidade - 1);
    destino[destino_capacidade - 1] = '\0';
}

/* O(|destino| + |origem|): retorna 1 se a concatenação couber, ou 0 (sem
 * alterar o destino) se não couber. O teste vem ANTES de calcular o espaço
 * livre, para nunca subtrair um número maior de um menor em size_t. */
static int concatena_seguro(char destino[], size_t destino_capacidade, const char origem[]) {
    size_t tamanho_atual = strlen(destino);
    if (tamanho_atual >= destino_capacidade)
        return 0;
    size_t espaco = destino_capacidade - tamanho_atual; /* inclui o lugar do '\0' */
    if (strlen(origem) >= espaco)
        return 0;
    memcpy(destino + tamanho_atual, origem, strlen(origem) + 1); /* copia com o '\0' */
    return 1;
}

int main(void) {
    char nome[CAPACIDADE];
    char cidade[CAPACIDADE];
    char frase[CAPACIDADE * 2];
    char palavra[CAPACIDADE];

    printf("=== Entrada de strings maiores ===\n");
    le_linha("Digite seu nome completo: ", nome, sizeof nome, "Ada Lovelace da Silva");
    le_linha("Digite sua cidade: ", cidade, sizeof cidade, "Santa Maria");

    /* Monta a frase em etapas, verificando a capacidade a cada concatenação. */
    frase[0] = '\0';
    if (!concatena_seguro(frase, sizeof frase, "Pessoa: ") ||
        !concatena_seguro(frase, sizeof frase, nome) ||
        !concatena_seguro(frase, sizeof frase, " | Cidade: ") ||
        !concatena_seguro(frase, sizeof frase, cidade)) {
        printf("A frase ficou grande demais para o buffer.\n");
        return 0;
    }

    printf("\n=== Resultado da montagem ===\n");
    printf("%s\n", frase);
    printf("Comprimento da frase: %zu caractere(s)\n", strlen(frase));
    printf("Capacidade do buffer: %zu byte(s)\n", sizeof frase);

    /* strstr localiza uma substring; strchr localiza o primeiro caractere.
     * Ambos devolvem um PONTEIRO para dentro de frase (ou NULL); a diferença
     * de ponteiros dá o índice (tipo ptrdiff_t, impresso com %td). */
    char *encontrada = strstr(frase, cidade);
    if (encontrada != NULL)
        printf("A cidade aparece na frase no deslocamento %td.\n", encontrada - frase);

    char *separador = strchr(frase, '|');
    if (separador != NULL)
        printf("O separador '|' aparece no deslocamento %td.\n", separador - frase);

    printf("\n");
    le_linha("Digite uma palavra para copiar com limite: ", palavra, sizeof palavra,
             "Paralelepipedo-industrial-de-teste");

    char copia[20];
    copia_segura(copia, sizeof copia, palavra);
    printf("Copia limitada a %zu caracteres: \"%s\"\n", sizeof copia - 1, copia);
    printf("Endereco de frase = %p | tamanho ocupado pelo array = %zu\n",
           (void *)frase, sizeof frase);
    printf("Importante: strlen mede o texto; sizeof mede a capacidade do array.\n");
    return 0;
}
