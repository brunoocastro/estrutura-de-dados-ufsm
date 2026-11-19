/*
 * ============================================================================
 * Arquivo : 03_matrizes.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.4 Vetores e Matrizes)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver que uma matriz C é um "vetor de vetores" guardado linha após linha
 *   (row-major): m[l][c] fica em base + (l * COLUNAS + c) * sizeof(elemento).
 *   Por isso o acesso é O(1) e percorrer tudo custa O(linhas * colunas).
 *
 * EXEMPLO REAL
 *   Uma imagem de câmera de inspeção (pixels), um mapa de ocupação de um AGV
 *   ou a tabela de calibração 2D de um sensor (temperatura x pressão).
 *
 * COMPLEXIDADE (L = linhas, C = colunas)
 *   Operação                  Custo     Por quê
 *   acesso m[l][c]            O(1)      uma conta de endereço
 *   percorrer a matriz        O(L*C)    visita cada célula uma vez
 *   somar uma linha           O(C)      uma linha tem C células
 *   somar uma coluna          O(L)      uma coluna tem L células
 *   Espaço: L * C * sizeof(elemento), contíguo.
 *
 * COMO EXECUTAR
 *   ./run 03_matrizes
 * ============================================================================
 */
#include <stdint.h>
#include <stdio.h>

#define LINHAS 2
#define COLUNAS 3

int main(void) {
    int matriz[LINHAS][COLUNAS] = {{1, 2, 3}, {4, 5, 6}};

    printf("=== Dimensoes e memoria ===\n");
    printf("linhas = %zu\n", sizeof matriz / sizeof matriz[0]);
    printf("colunas = %zu\n", sizeof matriz[0] / sizeof matriz[0][0]);
    printf("elementos = %zu\n", sizeof matriz / sizeof matriz[0][0]);
    printf("tamanho total = %zu byte(s)\n", sizeof matriz);

    /* C armazena matrizes em ordem de linha (row-major). A coluna "desloc."
     * mostra a posição de cada célula se a matriz fosse um vetor comum. */
    printf("\n=== Elementos em ordem de memoria ===\n");
    uintptr_t base = (uintptr_t)&matriz[0][0];
    for (size_t linha = 0; linha < LINHAS; linha++) {
        for (size_t coluna = 0; coluna < COLUNAS; coluna++) {
            size_t desloc = (size_t)((uintptr_t)&matriz[linha][coluna] - base) / sizeof(int);
            printf("matriz[%zu][%zu] = %d | endereco = %p | desloc. = %zu = %zu*%d + %zu\n",
                   linha, coluna, matriz[linha][coluna],
                   (void *)&matriz[linha][coluna], desloc, linha, COLUNAS, coluna);
        }
    }

    /* A matriz é um array de LINHAS arrays; cada linha possui COLUNAS int. */
    printf("\nsizeof(matriz[0]) = %zu byte(s), uma linha completa.\n", sizeof matriz[0]);
    printf("sizeof(matriz[0][0]) = %zu byte(s), um elemento.\n", sizeof matriz[0][0]);
    printf("O endereco logo apos matriz[0][2] e o de matriz[1][0],\n");
    printf("mas escrever matriz[0][3] e acesso fora da linha (comportamento indefinido).\n");

    printf("\n=== Matriz desenhada, com soma de linhas e colunas ===\n");
    int soma_coluna[COLUNAS] = {0};
    for (size_t linha = 0; linha < LINHAS; linha++) {      /* O(L*C) no total */
        int soma_linha = 0;
        for (size_t coluna = 0; coluna < COLUNAS; coluna++) {
            printf("%4d", matriz[linha][coluna]);
            soma_linha += matriz[linha][coluna];
            soma_coluna[coluna] += matriz[linha][coluna];
        }
        printf("  | soma da linha %zu = %d\n", linha, soma_linha);
    }
    for (size_t coluna = 0; coluna < COLUNAS; coluna++) printf("----");
    printf("\n");
    for (size_t coluna = 0; coluna < COLUNAS; coluna++) printf("%4d", soma_coluna[coluna]);
    printf("  <- soma de cada coluna\n");

    /* Dica de desempenho: percorrer linha por linha (o laço de coluna por
     * dentro) acessa endereços vizinhos e aproveita a memória cache; inverter
     * os laços faz "saltos" de COLUNAS elementos a cada acesso. O Big O é o
     * mesmo, mas em matrizes grandes o tempo real pode ser várias vezes maior. */
    printf("\nPercorra matrizes por linha (laco da coluna por dentro): mesma O(L*C),\n");
    printf("porem acessos contiguos sao mais rapidos por causa da memoria cache.\n");
    printf("Matrizes cujo tamanho so e conhecido em execucao: ../05_alocacao_dinamica/.\n");
    return 0;
}
