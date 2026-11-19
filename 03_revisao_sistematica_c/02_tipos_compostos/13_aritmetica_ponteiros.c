/*
 * ============================================================================
 * Arquivo : 13_aritmetica_ponteiros.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.7 Ponteiros)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver um ponteiro "andando" sobre um vetor, passo a passo: p + 1 avança
 *   sizeof(*p) bytes, *(p + i) é o mesmo que p[i], p - q dá a distância em
 *   elementos e "v + n" é a sentinela de fim. Depois, um ponteiro para
 *   ponteiro (const uint8_t **) funciona como cursor de leitura de um quadro
 *   de comunicação.
 *
 * EXEMPLO REAL
 *   Decodificar um quadro Modbus RTU recebido pela serial: um cursor percorre
 *   os bytes, lendo endereço do escravo (1 byte), código da função (1 byte) e
 *   registradores de 16 bits (2 bytes cada).
 *
 * COMPLEXIDADE (n = elementos)
 *   Operação              Custo   Por quê
 *   p + k, p[k], *(p+k)   O(1)    o deslocamento k*sizeof(*p) é uma única conta
 *   p - q                 O(1)    (endereço_p - endereço_q) / sizeof(*p)
 *   percorrer até v + n   O(n)    um passo por elemento
 *   maior_elemento        O(n)    precisa ver todos os elementos
 *
 * COMO EXECUTAR
 *   ./run 13_aritmetica_ponteiros               (ENTER avança cada passo)
 *   ./run 13_aritmetica_ponteiros < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define N 6

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* O(n): desenha o vetor e uma seta "p" sob o elemento apontado. Se p == v + n
 * (uma posição depois do último), a seta aparece na casa "fim": esse endereço
 * pode ser calculado e comparado, mas NUNCA desreferenciado. */
static void desenha(const int v[], size_t n, const int *p, const char *legenda) {
    printf("    indice : ");
    for (size_t i = 0; i < n; i++) printf("%5zu", i);
    printf("  fim\n    valor  : ");
    for (size_t i = 0; i < n; i++) printf("%5d", v[i]);
    printf("    -\n    bytes  : ");
    for (size_t i = 0; i < n; i++) printf("%5zu", i * sizeof v[0]);
    printf("%5zu\n             ", n * sizeof v[0]);
    for (const int *q = v; q < p; q++) printf("     ");
    printf("    ^ p   %s\n", legenda);
}

/* O(n): devolve um PONTEIRO para o maior elemento de [ini, fim). Devolver o
 * endereço (e não o valor) permite ao chamador alterar o elemento no vetor. */
static int *maior_elemento(int *ini, int *fim) {
    int *maior = ini;
    for (int *p = ini + 1; p < fim; p++)
        if (*p > *maior)
            maior = p;
    return maior;
}

/* O(1): lê 1 byte e avança o cursor. Recebe const uint8_t ** porque precisa
 * ALTERAR o ponteiro do chamador (o cursor), não só o byte apontado. */
static uint8_t ler_u8(const uint8_t **cursor) {
    uint8_t valor = **cursor;
    (*cursor)++;
    return valor;
}

/* O(1): lê 2 bytes no formato big-endian (byte alto primeiro, como no Modbus). */
static uint16_t ler_u16(const uint8_t **cursor) {
    uint16_t alto = ler_u8(cursor);
    uint16_t baixo = ler_u8(cursor);
    return (uint16_t)((alto << 8) | baixo);
}

/* O(n): mostra os bytes do quadro e a posição do cursor. */
static void desenha_quadro(const uint8_t *quadro, size_t n, const uint8_t *cursor) {
    printf("    quadro : ");
    for (size_t i = 0; i < n; i++) printf(" %02X", quadro[i]);
    printf("\n             ");
    for (const uint8_t *q = quadro; q < cursor; q++) printf("   ");
    printf(" ^ cursor (byte %td)\n", cursor - quadro);
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    int v[N] = {3, 8, 15, 4, 42, 23};
    int *p = v; /* o nome do vetor "decai" para &v[0] */

    printf("=== 1) Um ponteiro andando pelo vetor ===\n");
    desenha(v, N, p, "p = v  (p aponta para v[0])");
    printf("  *p = %d\n", *p);
    pausar();

    p++;
    desenha(v, N, p, "p++    (avancou 1 elemento)");
    printf("  *p = %d | p avancou %zu bytes, nao 1 byte\n", *p, sizeof *p);
    pausar();

    p += 3;
    desenha(v, N, p, "p += 3");
    printf("  *p = %d | p - v = %td elementos | *(v + 4) = %d | v[4] = %d\n",
           *p, p - v, *(v + 4), v[4]);
    pausar();

    p = v + N;
    desenha(v, N, p, "p = v + N  (sentinela: um depois do ultimo)");
    printf("  pode comparar (p == v + N), mas *p seria comportamento indefinido.\n");
    pausar();

    printf("\n=== 2) Percorrendo com ponteiro ate a sentinela: O(n) ===\n");
    int soma = 0;
    for (const int *q = v; q != v + N; q++) {
        soma += *q;
        printf("  q = &v[%td] -> *q = %2d | soma parcial = %d\n", q - v, *q, soma);
    }
    pausar();

    printf("\n=== 3) Passo depende do TIPO apontado ===\n");
    char c[4];
    double d[4];
    printf("  char  : c + 1 avanca %td byte(s)\n", (char *)(c + 1) - (char *)c);
    printf("  int   : v + 1 avanca %td byte(s)\n", (char *)(v + 1) - (char *)v);
    printf("  double: d + 1 avanca %td byte(s)\n", (char *)(d + 1) - (char *)d);
    pausar();

    printf("\n=== 4) Funcao que devolve um ponteiro para dentro do vetor ===\n");
    int *m = maior_elemento(v, v + N);
    printf("  maior elemento: v[%td] = %d\n", m - v, *m);
    *m = 0; /* altera o vetor original através do ponteiro devolvido */
    desenha(v, N, m, "*m = 0 alterou o proprio vetor");
    pausar();

    printf("\n=== 5) Ponteiro para ponteiro como cursor: quadro Modbus RTU ===\n");
    /* Resposta de "ler 2 registradores": escravo 0x11, função 0x03, 4 bytes de
     * dados, registradores 0x00FA (250) e 0x0119 (281), CRC 2 bytes. */
    const uint8_t quadro[] = {0x11, 0x03, 0x04, 0x00, 0xFA, 0x01, 0x19, 0xA5, 0x3C};
    size_t tam = sizeof quadro;
    const uint8_t *cursor = quadro;
    desenha_quadro(quadro, tam, cursor);
    uint8_t escravo = ler_u8(&cursor);
    printf("  ler_u8  -> escravo = %u\n", (unsigned)escravo);
    desenha_quadro(quadro, tam, cursor);
    uint8_t funcao = ler_u8(&cursor);
    printf("  ler_u8  -> funcao  = %u (ler registradores)\n", (unsigned)funcao);
    uint8_t nbytes = ler_u8(&cursor);
    printf("  ler_u8  -> bytes de dados = %u\n", (unsigned)nbytes);
    desenha_quadro(quadro, tam, cursor);
    pausar();
    for (unsigned r = 0; r < nbytes / 2u; r++) {
        uint16_t reg = ler_u16(&cursor);
        printf("  ler_u16 -> registrador %u = %u (ex.: temperatura %.1f C)\n",
               r, (unsigned)reg, reg / 10.0);
        desenha_quadro(quadro, tam, cursor);
    }
    printf("  restam %td bytes (CRC) ate o fim do quadro\n", (quadro + tam) - cursor);
    printf("\nSem o **, ler_u8 receberia uma COPIA do cursor e o avanco se perderia.\n");
    return 0;
}
