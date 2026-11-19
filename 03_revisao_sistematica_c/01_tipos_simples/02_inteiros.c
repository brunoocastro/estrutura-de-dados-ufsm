/*
 * ============================================================================
 * Arquivo : 02_inteiros.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: Unidade 1.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Comparar int, short, long e long long (com e sem sinal): valor, tamanho,
 *   endereço e limites. Mostrar o "dar a volta" dos unsigned e os tipos de
 *   largura fixa de <stdint.h> (uint8_t, int16_t, uint32_t...).
 *
 * EXEMPLO REAL
 *   Um encoder incremental de 16 bits conta 0..65535 e volta para 0: usar
 *   uint16_t deixa esse comportamento explícito. Registradores Modbus são
 *   uint16_t; contadores de tempo em ms costumam ser uint32_t.
 *
 * COMPLEXIDADE
 *   Todas as operações são O(1).
 *
 * COMO EXECUTAR
 *   ./run 02_inteiros
 * ============================================================================
 */
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    int com_sinal = -42;
    unsigned int sem_sinal = 42U; /* U: literal sem valores negativos. */
    short curto = -32000;
    long longo = 123456789L; /* L: literal com tipo long. */
    long long muito_longo = 1234567890123LL; /* LL: tipo long long. */

    printf("=== Valores e memoria ===\n");
    printf("int: %d | tamanho = %zu | endereco = %p\n", com_sinal, sizeof(com_sinal), (void *)&com_sinal);
    printf("unsigned int: %u | tamanho = %zu | endereco = %p\n", sem_sinal, sizeof(sem_sinal), (void *)&sem_sinal);
    printf("short: %hd | tamanho = %zu | endereco = %p\n", curto, sizeof(curto), (void *)&curto);
    printf("long: %ld | tamanho = %zu | endereco = %p\n", longo, sizeof(longo), (void *)&longo);
    printf("long long: %lld | tamanho = %zu | endereco = %p\n", muito_longo, sizeof(muito_longo), (void *)&muito_longo);

    printf("\n=== Limites desta compilacao ===\n");
    printf("INT_MIN = %d, INT_MAX = %d\n", INT_MIN, INT_MAX);
    printf("UINT_MAX = %u\n", UINT_MAX);
    printf("SHRT_MIN = %d, SHRT_MAX = %d\n", SHRT_MIN, SHRT_MAX);
    printf("LONG_MIN = %ld, LONG_MAX = %ld\n", LONG_MIN, LONG_MAX);
    printf("LLONG_MIN = %lld, LLONG_MAX = %lld\n", LLONG_MIN, LLONG_MAX);

    /* Estouro em unsigned é DEFINIDO: o resultado é calculado módulo 2^bits.
     * Estouro em signed (ex.: INT_MAX + 1) é comportamento indefinido (UB) e
     * por isso NÃO é executado aqui. */
    printf("\n=== Unsigned da a volta (aritmetica modulo 2^n) ===\n");
    uint16_t encoder = 65534;
    for (int passo = 0; passo < 4; passo++) {
        printf("encoder = %5" PRIu16 "\n", encoder);
        encoder++; /* 65535 + 1 vira 0: comportamento garantido pelo padrão. */
    }

    printf("\n=== Tipos de largura fixa (<stdint.h>) ===\n");
    printf("uint8_t : %zu byte(s), maximo = %" PRIu8 "\n", sizeof(uint8_t), (uint8_t)UINT8_MAX);
    printf("int16_t : %zu byte(s), faixa = %" PRId16 " .. %" PRId16 "\n",
           sizeof(int16_t), (int16_t)INT16_MIN, (int16_t)INT16_MAX);
    printf("uint32_t: %zu byte(s), maximo = %" PRIu32 "\n", sizeof(uint32_t), (uint32_t)UINT32_MAX);
    printf("Um uint32_t contando milissegundos da a volta em ~49,7 dias.\n");

    printf("\nO tamanho de int depende da implementacao; sizeof mostra o valor real.\n");
    return 0;
}
