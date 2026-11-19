/*
 * ============================================================================
 * Arquivo : 06_strings_custo_oculto.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.5 Strings)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar dois "custos escondidos" de strings em C que transformam um laço
 *   O(n) em O(n²) sem que o código pareça lento:
 *     1. chamar strlen() na condição do for (recalcula a cada volta);
 *     2. montar um texto com strcat() repetido (cada chamada procura o fim).
 *   Um contador de caracteres lidos mostra a diferença passo a passo e numa
 *   tabela para n = 10, 100, 1000, 10000.
 *
 * EXEMPLO REAL
 *   Um firmware que monta uma linha CSV "t;v1;v2;...;vN" com strcat para
 *   enviar a um supervisório: com 10 canais passa despercebido; com 10 000
 *   amostras o microcontrolador perde o prazo de tempo real.
 *
 * COMPLEXIDADE (n = comprimento do texto)
 *   Versão                              Caracteres lidos   Big O
 *   for (i < strlen(s))                 ~ n²               O(n²)
 *   len = strlen(s); for (i < len)      ~ 2n               O(n)
 *   strcat repetido (k pedaços)         ~ soma dos prefixos O(n²)
 *   ponteiro para o fim do texto        ~ n                O(n)
 *   Por quê: strlen não "sabe" o tamanho; ele anda até o '\0' toda vez.
 *
 * COMO EXECUTAR
 *   ./run 06_strings_custo_oculto               (ENTER avança cada passo)
 *   ./run 06_strings_custo_oculto < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static unsigned long long leituras = 0; /* caracteres lidos pelas funções abaixo */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* O(n): strlen instrumentado; conta cada caractere examinado (inclusive '\0'). */
static size_t strlen_contado(const char *s) {
    size_t n = 0;
    while (leituras++, s[n] != '\0')
        n++;
    return n;
}

/* O(|destino| + |origem|): strcat instrumentado. Primeiro ACHA o fim de
 * destino (é aqui que mora o custo escondido), depois copia a origem. */
static void strcat_contado(char *destino, const char *origem) {
    char *fim = destino + strlen_contado(destino);
    while ((*fim++ = *origem++) != '\0')
        leituras++;
}

/* Versão ingênua: strlen na condição -> n voltas x O(n) cada = O(n²). */
static void maiusculas_ingenuo(char *s, int mostrar) {
    for (size_t i = 0; i < strlen_contado(s); i++) {
        s[i] = (char)toupper((unsigned char)s[i]);
        leituras++;
        if (mostrar)
            printf("    volta %zu: strlen releu a string inteira -> total lido = %llu\n",
                   i, leituras);
    }
}

/* Versão correta: mede uma vez, guarda em len -> O(n). */
static void maiusculas_rapido(char *s, int mostrar) {
    size_t len = strlen_contado(s);
    for (size_t i = 0; i < len; i++) {
        s[i] = (char)toupper((unsigned char)s[i]);
        leituras++;
        if (mostrar)
            printf("    volta %zu: so le s[%zu]                -> total lido = %llu\n",
                   i, i, leituras);
    }
}

/* Monta "k;k;k;..." (k pedaços de 2 chars) com strcat repetido: O(n²). */
static void csv_com_strcat(char *buf, size_t pedacos) {
    buf[0] = '\0';
    for (size_t i = 0; i < pedacos; i++)
        strcat_contado(buf, "7;");
}

/* Mesmo resultado guardando um ponteiro para o fim: cada pedaço custa O(1)
 * para achar onde escrever, total O(n). */
static void csv_com_ponteiro_fim(char *buf, size_t pedacos) {
    char *fim = buf;
    for (size_t i = 0; i < pedacos; i++) {
        const char *pedaco = "7;";
        while ((*fim = *pedaco++) != '\0') { fim++; leituras++; }
    }
    *fim = '\0';
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    printf("=== 1) strlen na condicao do for: \"sensor\" -> \"SENSOR\" ===\n");
    char a[] = "sensor";
    leituras = 0;
    maiusculas_ingenuo(a, 1);
    printf("  resultado \"%s\": %llu leituras para 6 letras\n", a, leituras);
    pausar();

    char b[] = "sensor";
    leituras = 0;
    maiusculas_rapido(b, 1);
    printf("  resultado \"%s\": %llu leituras para 6 letras\n", b, leituras);
    pausar();

    printf("\n=== 2) Montando uma linha CSV com 5 pedacos \"7;\" ===\n");
    char linha[64];
    linha[0] = '\0';
    leituras = 0;
    for (int i = 0; i < 5; i++) {
        unsigned long long antes = leituras;
        strcat_contado(linha, "7;");
        printf("  strcat #%d: \"%-10s\" custou %llu leituras (andou %zu chars ate o fim)\n",
               i + 1, linha, leituras - antes, strlen(linha) - 2);
    }
    pausar();

    printf("\n=== 3) Crescimento (caracteres lidos) ===\n");
    printf("%7s | %14s | %10s || %14s | %10s\n",
           "n", "for+strlen", "len salvo", "strcat", "ptr fim");
    printf("--------+----------------+------------++----------------+-----------\n");
    for (size_t n = 10; n <= 10000; n *= 10) {
        char *s = malloc(n + 1);
        char *csv = malloc(n + 1);       /* n/2 pedaços de 2 chars + '\0' */
        if (s == NULL || csv == NULL) {
            free(s);
            free(csv);
            printf("sem memoria\n");
            return 0;
        }
        memset(s, 'a', n);
        s[n] = '\0';
        leituras = 0; maiusculas_ingenuo(s, 0);   unsigned long long l1 = leituras;
        memset(s, 'a', n);
        leituras = 0; maiusculas_rapido(s, 0);    unsigned long long l2 = leituras;
        leituras = 0; csv_com_strcat(csv, n / 2);  unsigned long long l3 = leituras;
        leituras = 0; csv_com_ponteiro_fim(csv, n / 2); unsigned long long l4 = leituras;
        printf("%7zu | %14llu | %10llu || %14llu | %10llu\n", n, l1, l2, l3, l4);
        free(s);
        free(csv);
    }
    printf("\nMultiplicar n por 10 multiplica as colunas O(n^2) por ~100 e as O(n) por ~10.\n");
    printf("Regra pratica: guarde o comprimento (ou um ponteiro para o fim) em vez de\n");
    printf("pedir a strlen/strcat que o descubram de novo a cada volta.\n");
    return 0;
}
