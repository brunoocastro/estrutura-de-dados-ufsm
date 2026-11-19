/*
 * ============================================================================
 * Arquivo : 07_leitura_linha_dinamica.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.5 Strings, 0.7 Alocação)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Juntar strings, ponteiros e heap: ler linhas de QUALQUER tamanho (sem o
 *   limite fixo de um char buf[80]) com um buffer que dobra, e guardá-las num
 *   vetor dinâmico de char * em que cada linha ocupa exatamente
 *   strlen + 1 bytes (uma "matriz irregular" de caracteres).
 *   O crescimento do buffer é narrado na primeira linha longa.
 *
 * EXEMPLO REAL
 *   Ler um arquivo de receita/configuração ou comandos chegando por uma porta
 *   serial: algumas linhas têm 5 caracteres, outras 500. Com buffer fixo, as
 *   longas seriam cortadas (ou estourariam o buffer, se lidas sem limite).
 *
 * COMPLEXIDADE (L = tamanho de uma linha, k = número de linhas)
 *   ler_linha         O(L) amortizado (o buffer dobra: < 2L cópias)
 *   guardar k linhas  O(k) amortizado (o vetor de ponteiros também dobra)
 *   liberar tudo      O(k): um free por linha + um free do vetor
 *   Memória: soma de (L_i + 1) + k ponteiros, contra k * MAX de uma matriz fixa.
 *
 * COMO EXECUTAR
 *   ./run 07_leitura_linha_dinamica                    (digite linhas; linha vazia encerra)
 *   ./run 07_leitura_linha_dinamica < arquivo.txt      (lê o arquivo inteiro)
 *   ./run 07_leitura_linha_dinamica < /dev/null        (usa um texto de exemplo)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty()/fmemopen() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char EXEMPLO[] =
    "START\n"
    "SET TT101.SP 75.0\n"
    "SET FT301.SP 12.0\n"
    "# comentario longo: esta linha da receita de producao descreve a etapa de aquecimento "
    "do reator com rampa de 2 C/min, patamar de 30 min e intertravamento com a pressao\n"
    "RUN\n";

/*
 * Lê uma linha de f (sem o '\n') para um bloco novo do heap, que passa a ser
 * do chamador (ele deve dar free). Devolve NULL em EOF sem nenhum caractere ou
 * se faltar memória. O buffer começa com 8 bytes e DOBRA quando enche:
 * O(L) amortizado. Se "narrar" for 1, imprime cada crescimento.
 */
static char *ler_linha(FILE *f, int narrar) {
    size_t cap = 8, len = 0;
    char *buf = malloc(cap);
    if (buf == NULL) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 == cap) {                    /* +1 reservado para o '\0' */
            char *tmp = realloc(buf, cap * 2);
            if (tmp == NULL) { free(buf); return NULL; }
            if (narrar)
                printf("    buffer cheio com %zu caracteres: realloc %zu -> %zu bytes\n",
                       len, cap, cap * 2);
            buf = tmp;
            cap *= 2;
        }
        buf[len++] = (char)c;
    }
    if (c == EOF && len == 0) { free(buf); return NULL; }
    buf[len] = '\0';
    /* Encolhe para o tamanho exato: strlen + 1. Se falhar, fica o maior. */
    char *exato = realloc(buf, len + 1);
    return exato ? exato : buf;
}

int main(void) {
    int interativo = isatty(STDIN_FILENO);
    FILE *entrada = stdin;
    FILE *aberto = NULL;

    /* Sem terminal: espia o primeiro caractere; se a entrada estiver vazia,
     * usa o texto de exemplo como se fosse um arquivo (fmemopen). */
    if (!interativo) {
        int c = fgetc(stdin);
        if (c == EOF) {
            aberto = fmemopen((void *)EXEMPLO, sizeof EXEMPLO - 1, "r");
            if (aberto == NULL) { printf("fmemopen falhou\n"); return 0; }
            entrada = aberto;
            printf("(entrada vazia: usando a receita de exemplo)\n");
        } else {
            ungetc(c, stdin);
        }
    } else {
        printf("Digite linhas de qualquer tamanho (linha vazia ou Ctrl+D encerra):\n");
    }

    char **linhas = NULL;      /* vetor dinâmico de ponteiros para as linhas */
    size_t n = 0, cap = 0;
    int narrou = 0;
    for (;;) {
        /* Narra o crescimento só na primeira linha que passar de 8 caracteres
         * (não dá para saber antes de ler, então narramos a linha 4 do exemplo
         * ou a primeira linha digitada). */
        int narrar = !narrou && (n == 3 || interativo);
        if (narrar) printf("\n  lendo a linha %zu:\n", n + 1);
        char *linha = ler_linha(entrada, narrar);
        if (narrar) narrou = 1;
        if (linha == NULL) break;
        if (interativo && linha[0] == '\0') { free(linha); break; }
        if (n == cap) {                       /* o vetor de ponteiros também dobra */
            size_t nova = cap ? cap * 2 : 2;
            char **tmp = realloc(linhas, nova * sizeof *tmp);
            if (tmp == NULL) { free(linha); break; }
            linhas = tmp;
            cap = nova;
        }
        linhas[n++] = linha;                  /* guarda o PONTEIRO; o texto já está no heap */
    }

    printf("\n=== %zu linha(s) guardada(s) ===\n", n);
    size_t bytes_texto = 0, maior = 0;
    for (size_t i = 0; i < n; i++) {
        size_t len = strlen(linhas[i]);
        bytes_texto += len + 1;
        if (len > maior) maior = len;
        printf("  linhas[%zu] -> %p | %3zu+1 bytes | %.60s%s\n", i, (void *)linhas[i],
               len, linhas[i], len > 60 ? "..." : "");
    }

    printf("\n=== Memoria ===\n");
    printf("  dinamico : %zu bytes de texto + %zu ponteiros (%zu bytes)\n",
           bytes_texto, cap, cap * sizeof *linhas);
    printf("  fixo     : char m[%zu][%zu] = %zu bytes (toda linha com o tamanho da maior)\n",
           n, maior + 1, n * (maior + 1));

    /* Libera cada linha e depois o vetor de ponteiros (ordem inversa da criação). */
    for (size_t i = 0; i < n; i++) free(linhas[i]);
    free(linhas);
    if (aberto != NULL) fclose(aberto);
    printf("\n  liberados: %zu linhas + 1 vetor de ponteiros.\n", n);
    return 0;
}
