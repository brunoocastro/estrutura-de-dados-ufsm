/*
 * ============================================================================
 * Arquivo : 08_valida_colchetes.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.1)
 * Autores : Prof. Leonardo R. Emmendorfer e Bruno Silva de Castro
 *           (exemplo de aula, versão 2.0, 03/09/2026)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Degrau 2 da progressão de validadores (07 -> 08 -> 09 -> 10 -> 11):
 *   verificar o pareamento de '[' e ']' usando uma pilha de caracteres com as
 *   operações separadas em funções (coloca/retira), como num TAD. Os demais
 *   caracteres da expressão são ignorados.
 *     "[[a+1][b-1]]" -> correta
 *     "[[[a]"        -> incorreta, pois sobram dois '['
 *   Diferença para o 07: o acesso ao vetor fica escondido em coloca/retira,
 *   que tratam overflow e underflow e informam o sucesso pelo retorno; a
 *   pilha é global e reutilizada entre chamadas (por isso `tamanho = 0`).
 *
 * EXEMPLO REAL
 *   Índices de vetores aninhados, como em v[idx[i]], numa expressão digitada
 *   num campo de fórmula de um sistema supervisório.
 *
 * COMPLEXIDADE (n = caracteres da expressão)
 *   Operação          Melhor      Médio       Pior        Espaço
 *   coloca/retira     O(1)        O(1)        O(1)        O(1)
 *   valida_colchetes  O(1)*       O(n)        O(n)        O(n) pilha
 *   * para no primeiro ']' sem abertura.
 *
 * COMO EXECUTAR
 *   ./run 08_valida_colchetes
 * ============================================================================
 */

#include <stdio.h>

#define CAPACIDADE 10000

char pilha[CAPACIDADE];
int tamanho = 0;

/* Coloca um colchete de abertura no topo da pilha. */
int coloca(char valor) {
    if (tamanho == CAPACIDADE)
        return 0;
    pilha[tamanho++] = valor;
    return 1;
}

/* Retira um elemento e grava o valor retirado em *valor. */
int retira(char *valor) {
    if (tamanho == 0)
        return 0;
    *valor = pilha[--tamanho];
    return 1;
}

int valida_colchetes(const char expressao[]) {
    tamanho = 0;

    for (size_t i = 0; expressao[i] != '\0'; i++) {
        if (expressao[i] == '[') {
            if (!coloca('[')) {
                printf("Erro: a pilha atingiu sua capacidade maxima.\n");
                return 0;
            }
            printf("posicao %zu: coloca '[' (tamanho = %d)\n", i, tamanho);
        } else if (expressao[i] == ']') {
            char abertura;
            if (!retira(&abertura)) {
                printf("Erro na posicao %zu: encontrou ']' sem '[' correspondente.\n", i);
                return 0;
            }
            printf("posicao %zu: retira '%c' (tamanho = %d)\n", i, abertura, tamanho);
        }
    }

    if (tamanho != 0) {
        printf("Erro: ainda falta fechar %d colchete(s).\n", tamanho);
        return 0;
    }
    return 1;
}

int main(void) {
    const char *expressao_correta = "[[a+1][b-1]]";
    const char *expressao_incorreta = "[[[a]";

    printf("=== Expressao correta ===\n%s\n", expressao_correta);
    if (valida_colchetes(expressao_correta))
        printf("Entrada ok.\n");
    else
        printf("Erro: colchetes desbalanceados.\n");

    printf("\n=== Expressao incorreta ===\n%s\n", expressao_incorreta);
    if (valida_colchetes(expressao_incorreta))
        printf("Entrada ok.\n");
    else
        printf("Erro: colchetes desbalanceados.\n");

    return 0;
}
