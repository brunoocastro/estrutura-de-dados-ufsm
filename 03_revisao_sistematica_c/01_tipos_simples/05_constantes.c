/*
 * ============================================================================
 * Arquivo : 05_constantes.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: Unidade 1.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Comparar as três formas de criar constantes em C: #define (texto trocado
 *   pelo pré-processador), const (variável somente leitura, tem endereço) e
 *   enum (constante inteira nomeada, pode dimensionar arrays).
 *
 * EXEMPLO REAL
 *   Número de canais de um módulo de entradas analógicas, período de
 *   amostragem e limites de alarme: nomes legíveis em vez de "números mágicos".
 *
 * COMPLEXIDADE
 *   Todas as operações são O(1).
 *
 * COMO EXECUTAR
 *   ./run 05_constantes
 * ============================================================================
 */
#include <stdio.h>

#define TAMANHO_MAXIMO 5 /* Macro: substituição textual feita pelo pré-processador. */

enum { DIAS_DA_SEMANA = 7 }; /* Constante inteira nomeada pelo compilador. */

int main(void) {
    const float PI = 3.1415926535f;
    const int limite = TAMANHO_MAXIMO;
    /* Em C (diferente de C++), uma variável const NÃO é "expressão constante":
     * int v[limite] criaria um VLA. Por isso arrays usam #define ou enum. */
    int valores[TAMANHO_MAXIMO] = {10, 20, 30, 40, 50};
    int semana[DIAS_DA_SEMANA] = {0};

    printf("PI = %f | tamanho = %zu | endereco = %p\n", PI, sizeof(PI), (void *)&PI);
    printf("limite = %d | tamanho = %zu | endereco = %p\n", limite, sizeof(limite), (void *)&limite);
    printf("TAMANHO_MAXIMO = %d (nao tem endereco: o texto 5 foi colado no codigo)\n", TAMANHO_MAXIMO);
    printf("DIAS_DA_SEMANA = %d | sizeof(semana) = %zu\n", DIAS_DA_SEMANA, sizeof semana);
    printf("primeiro valor = %d | endereco = %p\n", valores[0], (void *)&valores[0]);

    /* PI = 3.0f; // Erro de compilação: uma variável const não pode ser alterada. */
    printf("const impede alteracoes acidentais depois da inicializacao.\n");
    return 0;
}
