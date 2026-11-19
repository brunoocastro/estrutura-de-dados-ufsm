/*
 * ============================================================================
 * Arquivo : 03_reais.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: Unidade 1.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Comparar float, double e long double (precisão, tamanho, faixa) e mostrar
 *   por que nunca se compara número real com ==: 0.1 + 0.2 não é exatamente 0.3.
 *
 * EXEMPLO REAL
 *   Um controlador que testa "if (temperatura == setpoint)" pode nunca
 *   disparar. O correto é comparar com uma tolerância (histerese/banda morta).
 *
 * COMPLEXIDADE
 *   Todas as operações são O(1).
 *
 * COMO EXECUTAR
 *   ./run 03_reais
 * ============================================================================
 */
#include <float.h>
#include <math.h>
#include <stdio.h>

int main(void) {
    float simples = 3.1415926535f; /* f evita conversão de double para float. */
    double dupla = 3.1415926535; /* Sem sufixo: literal de ponto é double. */
    long double estendida = 3.141592653589793238L; /* L: long double neste contexto. */

    printf("float:       %.10f | tamanho = %zu | endereco = %p\n",
           simples, sizeof(simples), (void *)&simples);
    printf("double:      %.10f | tamanho = %zu | endereco = %p\n",
           dupla, sizeof(dupla), (void *)&dupla);
    printf("long double: %.15Lf | tamanho = %zu | endereco = %p\n",
           estendida, sizeof(estendida), (void *)&estendida);

    printf("\nPrecisao aproximada informada pelo compilador:\n");
    printf("FLT_DIG = %d, DBL_DIG = %d, LDBL_DIG = %d (digitos decimais confiaveis)\n",
           FLT_DIG, DBL_DIG, LDBL_DIG);
    printf("FLT_MAX = %e\n", FLT_MAX);
    printf("DBL_MAX = %e\n", DBL_MAX);

    /* 0.1 não tem representação binária exata (assim como 1/3 em decimal). */
    double a = 0.1 + 0.2;
    double esperado = 0.3;
    double tolerancia = 1e-9;
    printf("\n=== Comparando reais ===\n");
    printf("0.1 + 0.2 = %.17f\n", a);
    printf("0.3       = %.17f\n", esperado);
    printf("(0.1 + 0.2 == 0.3)            -> %s\n", a == esperado ? "verdadeiro" : "FALSO");
    printf("(fabs(diferenca) < 1e-9)      -> %s\n",
           fabs(a - esperado) < tolerancia ? "verdadeiro" : "falso");

    printf("\nNumeros reais geralmente sao aproximacoes; nem todo decimal e representado exatamente.\n");
    return 0;
}
