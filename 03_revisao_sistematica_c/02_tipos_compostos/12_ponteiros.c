/*
 * ============================================================================
 * Arquivo : 12_ponteiros.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.7 Ponteiros)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Panorama completo de ponteiros: ponteiro para ponteiro (int **), ponteiro
 *   recebido por função, relação ponteiro/vetor, acesso a struct com ->, NULL
 *   e um primeiro malloc/free. Cada endereço é impresso para o aluno conferir.
 *
 * EXEMPLO REAL
 *   Funções de biblioteca de comunicação recebem "struct Config *cfg" para não
 *   copiar a struct inteira e para poder alterá-la; buffers de recepção serial
 *   são alocados com malloc quando o tamanho só é conhecido em execução.
 *
 * COMPLEXIDADE (n = elementos do vetor)
 *   &x, *p, p->campo, **pp      O(1)   (um ou dois acessos à memória)
 *   mostra_array                O(n)   (visita cada elemento)
 *   malloc / free               ~O(1) amortizado na prática (depende do alocador)
 *   Passar struct por ponteiro: O(1); por valor: O(sizeof(struct)) de cópia.
 *
 * COMO EXECUTAR
 *   ./run 12_ponteiros
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>

struct Ponto {
    int x;
    int y;
};

/* O(1): recebe uma cópia do ENDEREÇO (sizeof(int *) bytes), não do valor.
 * const int * = "não vou alterar o int por este ponteiro". */
void mostra_inteiro(const int *ponteiro) {
    printf("\n=== Ponteiro recebido por funcao ===\n");
    printf("valor apontado (*ponteiro) = %d\n", *ponteiro);
    printf("endereco armazenado = %p\n", (const void *)ponteiro);
    printf("tamanho do ponteiro = %zu byte(s)\n", sizeof ponteiro);
    printf("tamanho do valor apontado = %zu byte(s)\n", sizeof *ponteiro);
}

/* O(n): o "array" recebido é um ponteiro para o primeiro elemento. O
 * deslocamento em bytes de array[i] é i * sizeof(int): por isso o acesso é O(1).
 * A conversão para (const char *) faz a subtração contar BYTES, e não ints. */
void mostra_array(const int array[], size_t tamanho) {
    printf("\n=== Ponteiro e array ===\n");
    printf("array recebido como endereco inicial = %p\n", (const void *)array);
    for (size_t i = 0; i < tamanho; i++) {
        const int *elemento = &array[i];
        printf("array[%zu] = %d | endereco = %p | deslocamento = %zu byte(s)\n",
               i, *elemento, (const void *)elemento,
               (size_t)((const char *)elemento - (const char *)&array[0]));
    }
}

int main(void) {
    int numero = 42;
    int *ponteiro = &numero;
    int **ponteiro_duplo = &ponteiro;
    int array[] = {10, 20, 30};
    struct Ponto ponto = {3, 7};
    struct Ponto *ponteiro_ponto = &ponto;

    printf("=== Variavel e ponteiro ===\n");
    printf("numero = %d\n", numero);
    printf("&numero = %p\n", (void *)&numero);
    printf("ponteiro = %p\n", (void *)ponteiro);
    printf("*ponteiro = %d\n", *ponteiro);
    printf("&ponteiro = %p\n", (void *)&ponteiro);
    printf("sizeof(numero) = %zu byte(s)\n", sizeof numero);
    printf("sizeof(ponteiro) = %zu byte(s)\n", sizeof ponteiro);
    printf("sizeof(*ponteiro) = %zu byte(s)\n", sizeof *ponteiro);

    printf("\n=== Ponteiro para ponteiro ===\n");
    printf("ponteiro_duplo = %p\n", (void *)ponteiro_duplo);
    printf("*ponteiro_duplo = %p\n", (void *)*ponteiro_duplo);
    printf("**ponteiro_duplo = %d\n", **ponteiro_duplo);
    printf("endereco de ponteiro_duplo = %p\n", (void *)&ponteiro_duplo);

    *ponteiro = 100;
    printf("\nDepois de *ponteiro = 100:\n");
    printf("numero = %d | *ponteiro = %d | **ponteiro_duplo = %d\n",
           numero, *ponteiro, **ponteiro_duplo);

    mostra_inteiro(&numero);
    mostra_array(array, sizeof array / sizeof array[0]);

    printf("\n=== Acesso a struct por ponteiro ===\n");
    printf("ponto = {%d, %d}\n", ponto.x, ponto.y);
    printf("&ponto = %p\n", (void *)&ponto);
    printf("ponteiro_ponto = %p\n", (void *)ponteiro_ponto);
    printf("ponteiro_ponto->x = %d\n", ponteiro_ponto->x);
    printf("(*ponteiro_ponto).y = %d\n", (*ponteiro_ponto).y);
    ponteiro_ponto->x = 50;
    printf("Depois de ponteiro_ponto->x = 50: ponto.x = %d\n", ponto.x);

    printf("\n=== Ponteiro NULL ===\n");
    int *nulo = NULL;
    printf("nulo = %p\n", (void *)nulo);
    printf("Um ponteiro NULL nao aponta para um objeto valido e nao deve ser desreferenciado.\n");

    /* Memória dinâmica: o bloco vem do heap e vive até o free, mesmo depois
     * que a função que o criou retorna. "sizeof *dinamico" (e não sizeof(int))
     * continua certo se o tipo do ponteiro mudar. Detalhes, vetor que cresce,
     * matriz dinâmica e erros clássicos: ../05_alocacao_dinamica/. */
    printf("\n=== Memoria dinamica ===\n");
    size_t quantidade = 4;
    int *dinamico = malloc(quantidade * sizeof *dinamico);
    if (dinamico == NULL) {
        printf("malloc falhou.\n");
        return 1;
    }
    for (size_t i = 0; i < quantidade; i++) {
        dinamico[i] = (int)(i + 1) * 100;
        printf("dinamico[%zu] = %d | endereco = %p\n",
               i, dinamico[i], (void *)&dinamico[i]);
    }
    printf("bloco alocado = %zu byte(s)\n", quantidade * sizeof *dinamico);
    free(dinamico);
    dinamico = NULL; /* evita usar por engano o endereço já liberado */
    printf("memoria liberada; dinamico agora vale %p\n", (void *)dinamico);

    printf("\nResumo: p e um endereco; *p e o valor nesse endereco; &x e o endereco de x.\n");
    return 0;
}
