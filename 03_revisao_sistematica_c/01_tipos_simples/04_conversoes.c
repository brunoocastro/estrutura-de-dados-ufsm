/*
 * ============================================================================
 * Arquivo : 04_conversoes.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: Unidade 1.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar conversões implícitas (promoções) e explícitas (cast): divisão
 *   inteira, truncamento e a armadilha de misturar signed com unsigned.
 *
 * EXEMPLO REAL
 *   Converter a leitura bruta de um ADC de 12 bits (0..4095) em tensão:
 *   leitura * 3.3 / 4095 funciona; leitura / 4095 * 3.3 dá sempre 0 (divisão
 *   inteira antes da multiplicação).
 *
 * COMPLEXIDADE
 *   Todas as operações são O(1).
 *
 * COMO EXECUTAR
 *   ./run 04_conversoes
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    int parte_inteira = 7;
    int divisor = 2;
    float resultado_inteiro = parte_inteira / divisor; /* Divisão inteira antes de guardar. */
    float resultado_real = (float)parte_inteira / divisor; /* Cast força divisão real. */
    int truncado = (int)3.99f; /* Conversão para int descarta a parte fracionária. */

    printf("7 / 2 armazenado em float = %.2f\n", resultado_inteiro);
    printf("(float)7 / 2 = %.2f\n", resultado_real);
    printf("(int)3.99f = %d  (trunca, nao arredonda)\n", truncado);

    /* Exemplo real: ADC de 12 bits. A ordem das operações decide o resultado. */
    int leitura_adc = 2048;
    double errado = leitura_adc / 4095 * 3.3;       /* 2048/4095 == 0 em int */
    double certo = leitura_adc * 3.3 / 4095;        /* 3.3 é double: tudo vira double */
    printf("\nADC = %d -> tensao (ordem errada) = %.3f V\n", leitura_adc, errado);
    printf("ADC = %d -> tensao (ordem certa)  = %.3f V\n", leitura_adc, certo);

    /* Conversão implícita entre tipos pode alterar precisão ou faixa. */
    char pequeno = 65;
    int inteiro = pequeno; /* char é promovido para int nas expressões. */
    printf("\nchar 65 impresso como caractere = %c\n", pequeno);
    printf("mesmo valor promovido para int = %d\n", inteiro);

    /* Armadilha: em -1 < 1u, o -1 é convertido para unsigned e vira UINT_MAX.
     * O compilador avisa (-Wextra/-Wsign-compare) quando a comparação é direta;
     * aqui usamos uma variável unsigned para mostrar o efeito. */
    int negativo = -1;
    unsigned int limite = 1u;
    printf("\n(unsigned)-1 = %u\n", (unsigned int)negativo);
    printf("(unsigned)-1 < 1u ? %s  (surpresa: -1 virou um numero enorme)\n",
           (unsigned int)negativo < limite ? "sim" : "nao");

    printf("\nUse casts quando a conversao for intencional e documente a perda possivel.\n");
    return 0;
}
