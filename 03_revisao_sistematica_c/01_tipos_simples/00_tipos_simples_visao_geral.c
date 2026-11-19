/*
 * ============================================================================
 * Arquivo : 00_tipos_simples_visao_geral.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: Unidade 1.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver, numa única tela, todos os tipos primitivos de C com seus modificadores
 *   (signed/unsigned, short/long), o valor guardado, o endereço na memória e o
 *   tamanho em bytes medido com sizeof.
 *
 * EXEMPLO REAL
 *   Ao ler um registrador Modbus (16 bits) ou um ADC de 12 bits num
 *   microcontrolador, escolher o tipo errado causa estouro ou desperdício de
 *   memória: o tamanho real do tipo naquela plataforma importa.
 *
 * COMPLEXIDADE
 *   Cada leitura/escrita de variável simples é O(1); o programa todo é O(1).
 *
 * COMO EXECUTAR
 *   ./run 00_tipos_simples_visao_geral
 * ============================================================================
 */
#include <stdio.h>

/* Macro para não repetir o mesmo printf 12 vezes. O operador & só funciona
 * porque "valor" é sempre o nome de uma variável (não uma expressão). */
#define MOSTRA_VARIAVEL(nome, formato, valor) \
    printf("%-18s valor = " formato ", endereco = %p, tamanho = %zu byte(s)\n", \
           nome, valor, (void *)&(valor), sizeof(valor))

int main(void) {
    /*
     * Em um literal de caractere, aspas simples representam um caractere.
     * Internamente, 'A' possui um valor inteiro correspondente na tabela de
     * caracteres (normalmente ASCII), diferente da string "A" (2 bytes: 'A','\0').
     */
    char caractere = 'A';

    /* Sem sufixo, este literal inteiro possui tipo int. */
    int inteiro = -42;

    /* O sufixo f força o literal para float; sem ele, 3.14 seria double. */
    float real = 3.14f;

    /* Sem sufixo, um literal com ponto decimal possui tipo double. */
    double real_preciso = 3.1415926535;

    /* Modificadores alteram faixa de valores e/ou tamanho do tipo. */
    signed char caractere_com_sinal = -10;
    unsigned char caractere_sem_sinal = 250;
    short inteiro_curto = -32000;
    unsigned short inteiro_curto_sem_sinal = 65000;

    /* O sufixo L indica que o literal inteiro deve ser tratado como long. */
    long inteiro_longo = 123456789L;

    /* UL combina U (unsigned, sem valores negativos) com L (long). Em sistemas
     * onde long tem 32 bits, 4000000000 só cabe na versão sem sinal. */
    unsigned long inteiro_longo_sem_sinal = 4000000000UL;

    /* LL significa long long: no mínimo 64 bits pelo padrão C99/C11. */
    long long inteiro_muito_longo = 1234567890123LL;

    /* ULL combina unsigned (U) e long long (LL). */
    unsigned long long inteiro_muito_longo_sem_sinal = 1234567890123ULL;

    printf("=== Tipos simples e modificadores ===\n");
    MOSTRA_VARIAVEL("char", "%c", caractere);
    MOSTRA_VARIAVEL("int", "%d", inteiro);
    MOSTRA_VARIAVEL("float", "%f", real);          /* float vira double no printf */
    MOSTRA_VARIAVEL("double", "%f", real_preciso);
    MOSTRA_VARIAVEL("signed char", "%d", caractere_com_sinal);
    MOSTRA_VARIAVEL("unsigned char", "%u", caractere_sem_sinal);
    MOSTRA_VARIAVEL("short", "%hd", inteiro_curto);
    MOSTRA_VARIAVEL("unsigned short", "%hu", inteiro_curto_sem_sinal);
    MOSTRA_VARIAVEL("long", "%ld", inteiro_longo);
    MOSTRA_VARIAVEL("unsigned long", "%lu", inteiro_longo_sem_sinal);
    MOSTRA_VARIAVEL("long long", "%lld", inteiro_muito_longo);
    MOSTRA_VARIAVEL("unsigned long long", "%llu", inteiro_muito_longo_sem_sinal);

    printf("\n=== Tamanhos dos tipos neste ambiente ===\n");
    printf("sizeof(char)                = %zu\n", sizeof(char));
    printf("sizeof(short)               = %zu\n", sizeof(short));
    printf("sizeof(int)                 = %zu\n", sizeof(int));
    printf("sizeof(long)                = %zu\n", sizeof(long));
    printf("sizeof(long long)           = %zu\n", sizeof(long long));
    printf("sizeof(float)               = %zu\n", sizeof(float));
    printf("sizeof(double)              = %zu\n", sizeof(double));
    printf("sizeof(unsigned int)        = %zu\n", sizeof(unsigned int));
    printf("sizeof(int *)               = %zu  (um endereco)\n", sizeof(int *));

    printf("\nObservacao: por definicao sizeof(char) == 1; o tamanho de int e de long\n");
    printf("nao e fixo pelo padrao e varia entre plataformas (ex.: long tem 4 bytes no\n");
    printf("Windows 64 bits e 8 bytes no Linux/macOS 64 bits).\n");
    return 0;
}
