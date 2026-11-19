/*
 * ============================================================================
 * Arquivo : 03_inverte_vetor.c
 * Módulo  : 02 — Revisão de funções, Lista L02   (Ementa: Unidade 0.4 e 0.6)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Exercício 3: função inverte_vetor(v, n) que inverte o vetor no próprio
 *   lugar (in-place), trocando o primeiro com o último, o segundo com o
 *   penúltimo, e assim por diante.
 *
 * EXEMPLO REAL
 *   Mostrar o histórico de alarmes do mais recente para o mais antigo;
 *   inverter a ordem dos bytes de uma palavra recebida em outra convenção.
 *
 * COMPLEXIDADE (n = tamanho do vetor)
 *   O(n) tempo (n/2 trocas), O(1) espaço extra: não cria um segundo vetor.
 *   A função altera o vetor da main porque vetores não são copiados na
 *   chamada (ver ../01_funcoes_passagem_valor.c, Parte 3).
 *
 * COMO EXECUTAR
 *   echo "1 2 3 0" | ./run 03_inverte_vetor      (0 encerra a leitura)
 * ============================================================================
 */
#include <stdio.h>

#define MAX 100

/* Troca elementos simetricos: primeiro com ultimo, e assim por diante. */
void inverte_vetor(int v[], int tamanho) {
    for (int inicio = 0, fim = tamanho - 1; inicio < fim; inicio++, fim--) {
        int temporario = v[inicio];
        v[inicio] = v[fim];
        v[fim] = temporario;
    }
}

int main(void) {
    int v[MAX], tamanho = 0, valor;

    printf("Informe inteiros (zero encerra a leitura):\n");
    /* A ordem dos testes importa: primeiro o limite do vetor, depois a
       leitura (EOF também encerra), depois o valor sentinela 0. */
    while (tamanho < MAX && scanf("%d", &valor) == 1 && valor != 0)
        v[tamanho++] = valor;

    inverte_vetor(v, tamanho); /* A inversao e feita no proprio vetor. */
    for (int i = 0; i < tamanho; i++)
        printf("%d%s", v[i], i == tamanho - 1 ? "\n" : " ");
    if (tamanho == 0) printf("Nenhum valor lido.\n");
    return 0;
}
