/*
 * Tipos simples (primitivos) em C
 *
 * Compile e execute:
 *   gcc -std=c11 -Wall -Wextra -pedantic 01_tipos_simples.c -o 01_tipos_simples
 *   ./01_tipos_simples
 *
 * Os tamanhos podem variar entre compiladores e arquiteturas. Por isso,
 * sizeof e a forma correta de verificar o tamanho usado neste ambiente.
 */
#include <stdio.h>

#define MOSTRA_VARIAVEL(nome, formato, valor) \
    printf("%-18s valor = " formato ", endereco = %p, tamanho = %zu byte(s)\n", \
           nome, valor, (void *)&(valor), sizeof(valor))

int main(void) {
    /* Tipos básicos: seus valores e endereços são observados na memória. */
    /*
     * Em um literal de caractere, aspas simples representam um caractere.
     * Internamente, 'A' possui um valor inteiro correspondente na tabela de
     * caracteres (normalmente ASCII), diferente de uma string "A".
     */
    char caractere = 'A';

    /* Sem sufixo, este literal inteiro normalmente possui tipo int. */
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

    /*
     * O sufixo L indica que o literal inteiro deve ser tratado como long.
     * Isso evita depender do tipo int quando o valor é maior ou quando
     * queremos deixar explícita a intenção do código.
     */
    long inteiro_longo = 123456789L;

    /*
     * UL combina U (unsigned, sem valores negativos) com L (long).
     * O U também ajuda a representar corretamente valores grandes positivos.
     */
    unsigned long inteiro_longo_sem_sinal = 4000000000UL;

    /* LL significa long long, um tipo inteiro de maior capacidade mínima. */
    long long inteiro_muito_longo = 1234567890123LL;

    /* ULL combina unsigned (U) e long long (LL). */
    unsigned long long inteiro_muito_longo_sem_sinal = 1234567890123ULL;

    printf("=== Tipos simples e modificadores ===\n");
    MOSTRA_VARIAVEL("char", "%c", caractere);
    MOSTRA_VARIAVEL("int", "%d", inteiro);
    MOSTRA_VARIAVEL("float", "%f", real);
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

    printf("\nObservacao: um byte em C e sizeof(char); o tamanho de int nao e fixo\n");
    printf("pelo padrao e pode variar entre plataformas de 32 e 64 bits.\n");
    return 0;
}
