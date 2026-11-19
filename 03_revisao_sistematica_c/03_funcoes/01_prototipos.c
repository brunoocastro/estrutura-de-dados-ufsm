/*
 * ============================================================================
 * Arquivo : 01_prototipos.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Funções)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Um protótipo informa ao compilador o nome, o retorno e os parâmetros de
 *   uma função antes de sua chamada; a definição contém o corpo executado.
 *   Sem protótipo, o compilador não consegue conferir os tipos da chamada.
 *
 * EXEMPLO REAL
 *   Os arquivos .h de uma biblioteca de driver (ex.: "adc.h") são listas de
 *   protótipos: o programa principal sabe COMO chamar sem ver o código.
 *
 * COMPLEXIDADE
 *   soma e mostra_mensagem: O(1) (printf de uma string é O(tamanho)).
 *
 * COMO EXECUTAR
 *   ./run 01_prototipos
 * ============================================================================
 */
#include <stdio.h>

/* Declarações antecipadas: as funções são definidas depois de main. */
int soma(int a, int b);
void mostra_mensagem(const char *mensagem);

int main(void) {
    int resultado = soma(7, 5);
    mostra_mensagem("Funcoes podem retornar valores ou apenas executar uma acao.");
    printf("soma(7, 5) = %d\n", resultado);
    return 0;
}

/* O(1): retorna a soma. */
int soma(int a, int b) {
    return a + b;
}

/* Imprime a mensagem; void = não retorna valor. */
void mostra_mensagem(const char *mensagem) {
    printf("mensagem: %s\n", mensagem);
}
