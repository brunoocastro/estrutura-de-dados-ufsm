/*
 * ============================================================================
 * Arquivo : 01_arrays.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.4 Vetores e Matrizes)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Declarar e inicializar vetores (tamanho explícito, implícito, parcial),
 *   ver que os elementos ficam lado a lado na memória e entender POR QUE o
 *   acesso v[i] custa O(1) qualquer que seja i, enquanto procurar um valor
 *   custa O(n). Mostra também o "decaimento" do vetor para ponteiro.
 *
 * EXEMPLO REAL
 *   Um CLP guarda as últimas 8 leituras de uma entrada analógica num vetor
 *   para calcular média móvel: acessar a leitura i é imediato; descobrir se
 *   algum valor passou do limite exige olhar todas.
 *
 * COMPLEXIDADE (n = número de elementos)
 *   Operação                     Custo   Por quê
 *   acesso v[i] (ler/escrever)   O(1)    endereço = base + i * sizeof(elemento)
 *   busca sequencial por valor   O(n)    no pior caso compara com todos
 *   soma/máximo/média            O(n)    precisa visitar cada elemento uma vez
 *   sizeof v / sizeof v[0]       O(1)    calculado pelo compilador
 *   Espaço: n * sizeof(elemento), fixo desde a declaração.
 *
 * COMO EXECUTAR
 *   ./run 01_arrays
 * ============================================================================
 */
#include <stdint.h>
#include <stdio.h>

/* O(n): busca sequencial. Retorna o índice da primeira ocorrência ou -1.
 * Conta as comparações para mostrar o custo de forma empírica. */
static int busca_sequencial(const int v[], size_t n, int alvo, size_t *comparacoes) {
    *comparacoes = 0;
    for (size_t i = 0; i < n; i++) {
        (*comparacoes)++;
        if (v[i] == alvo)
            return (int)i;
    }
    return -1;
}

/* Ao passar um vetor, a função recebe só o endereço do primeiro elemento: os
 * parâmetros "const int v[]" e "const int *v" são IDÊNTICOS para o compilador.
 * Aqui usamos a forma de ponteiro para deixar isso explícito: sizeof v devolve
 * o tamanho de um endereço. (Com "v[]", o clang até avisa que sizeof engana.)
 * Por isso todo vetor passado a uma função precisa vir acompanhado de n. */
static void mostra_decaimento(const int *v, size_t n) {
    printf("dentro da funcao: sizeof v = %zu (tamanho de um ponteiro), n = %zu\n",
           sizeof v, n);
}

int main(void) {
    /* Tamanho explícito: o número de elementos aparece entre colchetes. */
    int explicito[5] = {10, 20, 30, 40, 50};

    /* Tamanho implícito: o compilador conta os inicializadores (5 elementos). */
    int implicito[] = {10, 20, 30, 40, 50};

    /* Inicialização parcial: os elementos restantes recebem zero. */
    int parcial[5] = {10, 20};

    /* O tamanho pode ser calculado apenas enquanto o nome ainda é um array. */
    size_t quantidade = sizeof explicito / sizeof explicito[0];

    printf("=== Tamanho e inicializacao ===\n");
    printf("quantidade de elementos = %zu\n", quantidade);
    printf("sizeof(explicito) = %zu byte(s)\n", sizeof explicito);
    printf("sizeof(implicito) = %zu byte(s)\n", sizeof implicito);
    printf("sizeof(int) = %zu byte(s)\n", sizeof(int));
    printf("parcial = {");
    for (size_t i = 0; i < 5; i++)
        printf("%d%s", parcial[i], i + 1 < 5 ? ", " : "}\n");

    printf("\n=== Memoria contigua: por que v[i] custa O(1) ===\n");
    /* uintptr_t permite fazer a conta do endereço como número inteiro. */
    uintptr_t base = (uintptr_t)&explicito[0];
    for (size_t i = 0; i < quantidade; i++) {
        uintptr_t calculado = base + i * sizeof explicito[0];
        printf("explicito[%zu] = %d | endereco = %p | base + %zu*%zu = %s\n",
               i, explicito[i], (void *)&explicito[i], i, sizeof explicito[0],
               calculado == (uintptr_t)&explicito[i] ? "confere" : "difere");
    }
    printf("O processador faz UMA conta (base + i*tamanho) e vai direto ao elemento:\n");
    printf("acessar explicito[0] ou explicito[4] custa o mesmo -> O(1).\n");

    printf("\n=== Busca por valor: O(n) ===\n");
    int alvos[] = {10, 30, 50, 99};
    for (size_t k = 0; k < sizeof alvos / sizeof alvos[0]; k++) {
        size_t comparacoes;
        int pos = busca_sequencial(explicito, quantidade, alvos[k], &comparacoes);
        printf("procura %2d -> indice %2d apos %zu comparacao(oes)\n",
               alvos[k], pos, comparacoes);
    }
    printf("Pior caso (valor ausente ou no fim): n comparacoes.\n");

    printf("\n=== Decaimento para ponteiro ===\n");
    printf("em main: sizeof explicito = %zu (o vetor inteiro)\n", sizeof explicito);
    mostra_decaimento(explicito, quantidade);

    /* Índices válidos são 0 a quantidade - 1; explicito[5] seria acesso fora do
     * vetor: comportamento indefinido (C não verifica limites). */
    printf("\nPrimeiro elemento: %d\n", explicito[0]);
    printf("Ultimo elemento: %d (indice quantidade - 1 = %zu)\n",
           explicito[quantidade - 1], quantidade - 1);

    /* Um array tem tamanho fixo: não cresce automaticamente.
     * Para crescer em tempo de execução, veja ../05_alocacao_dinamica/. */
    printf("\nArrays locais possuem tamanho fixo definido na declaracao.\n");
    return 0;
}
