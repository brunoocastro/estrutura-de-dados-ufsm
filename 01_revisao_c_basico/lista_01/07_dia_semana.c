/*
 * ============================================================================
 * Arquivo : 07_dia_semana.c
 * Módulo  : 01 — Revisão de C básico, Lista L01    (Ementa: Unidade 0.3 a 0.5)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 7: ler um número de 1 a 7 e escrever o dia da semana. Usa um
 *   vetor de strings como tabela de consulta em vez de um switch com 7 casos.
 *
 * EXEMPLO REAL
 *   Tradução de códigos em texto na IHM: código de alarme -> mensagem,
 *   código de estado da máquina -> "PARADA", "EM CICLO", "FALHA".
 *
 * COMPLEXIDADE
 *   O(1): validação da faixa + um acesso por índice (dias[numero - 1]).
 *   Armadilha: sem validar a faixa, dias[7] ou dias[-1] lê memória fora do
 *   vetor.
 *
 * COMO EXECUTAR
 *   echo "2" | ./run 07_dia_semana
 * ============================================================================
 */
#include <stdio.h>

int main(void) {
    int numero;
    const char dias[7][14] = {"domingo", "segunda-feira", "terca-feira",
                              "quarta-feira", "quinta-feira", "sexta-feira",
                              "sabado"};

    printf("Digite um numero de 1 a 7: ");
    int lidos = scanf("%d", &numero);
    if (lidos == EOF) {
        printf("\nSem entrada.\n");
        return 0;
    }
    if (lidos != 1) {
        fprintf(stderr, "Entrada invalida.\n");
        return 1;
    }
    if (numero < 1 || numero > 7)
        printf("Nao existe dia da semana com esse numero.\n");
    else
        printf("Dia da semana: %s.\n", dias[numero - 1]);

    return 0;
}
