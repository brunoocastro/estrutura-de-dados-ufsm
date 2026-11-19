/*
 * ============================================================================
 * Arquivo : 01_char.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: Unidade 1.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar que char é um inteiro pequeno (1 byte): 'A' e 65 são o mesmo valor,
 *   e a "aritmética de caracteres" ('7' - '0' == 7) é muito usada para
 *   converter texto em número.
 *
 * EXEMPLO REAL
 *   Um CLP recebe "T=27" pela serial: cada dígito chega como char e precisa
 *   virar número com c - '0' antes de ser usado no controle.
 *
 * COMPLEXIDADE
 *   Todas as operações são O(1).
 *
 * COMO EXECUTAR
 *   ./run 01_char
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    /* Aspas simples representam um único caractere. */
    char letra = 'A';
    char digito = '7';
    char nova_linha = '\n';

    printf("letra = %c\n", letra);
    printf("digito = %c\n", digito);
    printf("valor numerico de letra = %d\n", letra);
    printf("valor numerico de digito = %d (o caractere '7', nao o numero 7)\n", digito);
    printf("tamanho de letra = %zu byte(s)\n", sizeof(letra));
    printf("endereco de letra = %p\n", (void *)&letra);

    /* Os dígitos '0'..'9' são consecutivos em qualquer codificação aceita pelo
     * padrão C; por isso subtrair '0' converte o caractere no valor numérico. */
    printf("\ndigito - '0' = %d  (agora sim o numero 7)\n", digito - '0');
    printf("letra + 1 = %c  (as letras costumam ser consecutivas em ASCII)\n", letra + 1);

    /* '\n' é uma sequência de escape que representa uma quebra de linha. */
    printf("\nEste texto vem antes de nova_linha%cEste texto vem depois.\n", nova_linha);

    /* Armadilha: char simples pode ter ou não sinal, conforme a plataforma
     * (x86: com sinal; ARM: normalmente sem sinal). Para bytes "crus" vindos de
     * sensores ou da rede, prefira unsigned char ou uint8_t. */
    return 0;
}
