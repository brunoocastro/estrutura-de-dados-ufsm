/*
 * ============================================================================
 * Arquivo : 04_busca_binaria.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Busca binária recursiva: o vetor precisa estar ORDENADO e cada chamada
 *   descarta metade do intervalo. O rastro desenha o intervalo [inicio, fim]
 *   e o meio a cada nível. O estudo completo (sequencial x binária, tabelas)
 *   fica no módulo ../../08_ordenacao_busca/.
 *
 * EXEMPLO REAL
 *   Procurar, numa tabela de calibração ordenada por temperatura, a faixa em
 *   que a leitura atual se encaixa, ou um código numa lista ordenada de
 *   alarmes de um SCADA.
 *
 * COMPLEXIDADE (n = tamanho do vetor)
 *   Tempo: O(log n) -> o intervalo cai pela metade a cada chamada.
 *   Pilha: O(log n) -> no máximo ~log2(n) + 1 quadros (o laço usa O(1)).
 *   Como a chamada recursiva é a ÚLTIMA ação, esta é uma recursão de cauda.
 *
 * COMO EXECUTAR
 *   ./run 04_busca_binaria               (pede o valor procurado)
 *   ./run 04_busca_binaria < /dev/null   (procura 25 e 26)
 * ============================================================================
 */
#include <stdio.h>

static int comparacoes = 0;

/* O(n): desenha o vetor marcando o intervalo ativo e o meio. */
static void desenha(const int v[], int n, int inicio, int fim, int meio, int nivel) {
    printf("%*s", nivel * 2, "");
    for (int i = 0; i < n; i++) {
        if (i < inicio || i > fim) printf("  . ");
        else if (i == meio) printf("[%2d]", v[i]);
        else printf(" %2d ", v[i]);
    }
    printf("\n");
}

/* O(log n). Retorna o índice de "procurado" ou -1. */
static int busca(const int array[], int n, int inicio, int fim, int procurado, int nivel) {
    printf("%*sintervalo [%d, %d]\n", nivel * 2, "", inicio, fim);
    if (inicio > fim) { /* caso base 1: intervalo vazio */
        printf("%*snao encontrado\n", nivel * 2, "");
        return -1;
    }

    /* inicio + (fim - inicio) / 2 evita o estouro de (inicio + fim) / 2
     * quando os índices são muito grandes. */
    int meio = inicio + (fim - inicio) / 2;
    desenha(array, n, inicio, fim, meio, nivel);
    comparacoes++;
    printf("%*smeio = %d, valor = %d -> ", nivel * 2, "", meio, array[meio]);
    if (array[meio] == procurado) { /* caso base 2: achou */
        printf("ENCONTRADO\n");
        return meio;
    }
    if (procurado < array[meio]) {
        printf("%d < %d, descarta a metade direita\n", procurado, array[meio]);
        return busca(array, n, inicio, meio - 1, procurado, nivel + 1);
    }
    printf("%d > %d, descarta a metade esquerda\n", procurado, array[meio]);
    return busca(array, n, meio + 1, fim, procurado, nivel + 1);
}

int main(void) {
    const int array[] = {3, 8, 12, 17, 25, 31, 42, 50, 61, 77, 85, 93, 99};
    const int n = (int)(sizeof array / sizeof array[0]);
    int procurados[2] = {25, 26};
    int quantos = 2;

    printf("Informe o valor procurado: ");
    if (scanf("%d", &procurados[0]) == 1) {
        quantos = 1;
    } else {
        printf("(entrada vazia: procurando 25 e depois 26)\n");
    }

    for (int k = 0; k < quantos; k++) {
        comparacoes = 0;
        printf("\n=== Procurando %d em %d elementos ===\n", procurados[k], n);
        int indice = busca(array, n, 0, n - 1, procurados[k], 0);
        printf("Indice retornado: %d | comparacoes: %d (busca sequencial: ate %d)\n",
               indice, comparacoes, n);
    }
    printf("\nCom n = 1 000 000, a busca binaria faz no maximo ~20 comparacoes.\n");
    return 0;
}
