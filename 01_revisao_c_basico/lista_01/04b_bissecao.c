/*
 * ============================================================================
 * Arquivo : 04b_bissecao.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.2 e 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 4b: método da bisseção para achar a raiz de
 *   f(x) = x^3 - x - 2 em [inicio, fim], parando quando o intervalo fica
 *   menor que a tolerância. Mostra cada iteração.
 *
 * EXEMPLO REAL
 *   Calibração: achar a temperatura em que a curva de um termistor dá a
 *   resistência medida; ajuste de ganho por tentativa em bancada. É a mesma
 *   ideia da busca binária (Unidade 4): cortar o intervalo pela metade.
 *
 * COMPLEXIDADE (L = fim - inicio, tol = tolerância)
 *   O(log2(L / tol)) iterações, O(1) espaço: cada volta divide o intervalo
 *   por 2. Com L = 1 e tol = 1e-6 bastam 20 iterações.
 *
 * COMO EXECUTAR
 *   echo "1 2 0.000001" | ./run 04b_bissecao
 *   ./run 04b_bissecao < /dev/null    (usa o exemplo padrão 1 2 0.000001)
 * ============================================================================
 */
#include <math.h>
#include <stdio.h>

/* Encontra a raiz de f(x) = x^3 - x - 2 no intervalo informado. */
static double funcao(double x) {
    return x * x * x - x - 2.0;
}

int main(void) {
    double inicio, fim, tolerancia;

    printf("Digite inicio, fim e tolerancia: ");
    int lidos = scanf("%lf %lf %lf", &inicio, &fim, &tolerancia);
    if (lidos == EOF) {                 /* sem entrada: usa um exemplo padrão */
        inicio = 1.0;
        fim = 2.0;
        tolerancia = 1e-6;
        printf("\nSem entrada: usando inicio = 1, fim = 2, tolerancia = 1e-6.\n");
    } else if (lidos != 3 || tolerancia <= 0.0 || inicio >= fim) {
        fprintf(stderr, "Parametros invalidos.\n");
        return 1;
    }
    /* Teorema de Bolzano: só há garantia de raiz se f muda de sinal. */
    if (funcao(inicio) * funcao(fim) > 0.0) {
        printf("O intervalo nao possui extremos com sinais opostos.\n");
        return 1;
    }

    int iteracao = 0;
    printf("%4s %14s %14s %14s\n", "it", "inicio", "fim", "f(meio)");
    while (fim - inicio > tolerancia) {
        double meio = (inicio + fim) / 2.0;
        iteracao++;
        printf("%4d %14.8f %14.8f %14.8f\n", iteracao, inicio, fim, funcao(meio));
        if (funcao(inicio) * funcao(meio) <= 0.0)
            fim = meio;          /* a raiz está na metade da esquerda */
        else
            inicio = meio;       /* a raiz está na metade da direita */
    }

    double raiz = (inicio + fim) / 2.0;
    printf("Raiz aproximada: %.10f (%d iteracoes)\n", raiz, iteracao);
    printf("f(raiz): %.10f\n", funcao(raiz));
    return 0;
}
