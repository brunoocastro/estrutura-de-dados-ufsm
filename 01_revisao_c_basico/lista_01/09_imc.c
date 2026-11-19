/*
 * ============================================================================
 * Arquivo : 09_imc.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 9: calcular o IMC e classificar a faixa com uma cadeia
 *   if / else if. Cada teste só verifica o limite superior, porque o
 *   inferior já foi garantido pelo teste anterior.
 *   Decisão sobre os limites: cada faixa inclui o limite inferior e exclui o
 *   superior (IMC = 25,0 é sobrepeso; IMC = 30,0 é obeso; 40,0 é mórbido).
 *
 * EXEMPLO REAL
 *   Classificação de faixas de alarme em CLP: normal / atenção / alarme /
 *   trip, exatamente com a mesma cadeia de comparações.
 *
 * COMPLEXIDADE
 *   O(1): no máximo 4 comparações.
 *
 * COMO EXECUTAR
 *   echo "70 1.75" | ./run 09_imc
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    double peso, altura, imc;

    printf("Digite peso (kg) e altura (m): ");
    int lidos = scanf("%lf %lf", &peso, &altura);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 2 || peso <= 0.0 || altura <= 0.0) {
        fprintf(stderr, "Peso e altura devem ser positivos.\n");
        return 1;
    }

    imc = peso / (altura * altura);
    printf("IMC: %.2f\n", imc);
    if (imc < 20.0)
        printf("Situacao: abaixo do peso.\n");
    else if (imc < 25.0)
        printf("Situacao: peso normal.\n");
    else if (imc < 30.0)
        printf("Situacao: sobrepeso.\n");
    else if (imc < 40.0)
        printf("Situacao: obeso.\n");
    else
        printf("Situacao: obeso morbido.\n");

    return 0;
}
