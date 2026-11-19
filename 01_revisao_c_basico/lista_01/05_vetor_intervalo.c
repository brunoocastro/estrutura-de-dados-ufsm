/*
 * ============================================================================
 * Arquivo : 05_vetor_intervalo.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.4)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 5: ler 20 inteiros em um vetor e verificar se todos estão no
 *   intervalo fechado [0, 100].
 *
 * EXEMPLO REAL
 *   Validação de leituras: um sinal em % (abertura de válvula, nível) fora
 *   de 0..100 indica sensor com defeito ou erro de escala.
 *
 * COMPLEXIDADE (n = 20)
 *   O(n) tempo: cada valor é testado uma vez. O(n) espaço para o vetor.
 *   Poderia parar no primeiro inválido (melhor caso O(1)), mas aqui lemos
 *   todos os valores de qualquer forma.
 *
 * COMO EXECUTAR
 *   echo "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 100" | ./run 05_vetor_intervalo
 * ============================================================================
 */
#include <stdio.h>

#define TAMANHO 20

int main(void) {
    int valores[TAMANHO];
    int todos_validos = 1;

    printf("Digite %d valores inteiros:\n", TAMANHO);
    for (int i = 0; i < TAMANHO; i++) {
        int lidos = scanf("%d", &valores[i]);
        if (lidos == EOF) {
            printf("Entrada terminou apos %d valor(es).\n", i);
            return 0;
        }
        if (lidos != 1) {
            fprintf(stderr, "Entrada invalida na posicao %d.\n", i);
            return 1;
        }
    }

    for (int i = 0; i < TAMANHO; i++) {
        if (valores[i] < 0 || valores[i] > 100) {
            printf("Posicao %d: %d fora de [0, 100].\n", i, valores[i]);
            todos_validos = 0;
        }
    }

    printf("%s\n", todos_validos ?
           "Todos os valores pertencem a [0, 100]." :
           "Nem todos os valores pertencem a [0, 100].");
    return 0;
}
