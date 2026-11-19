/*
 * ============================================================================
 * Arquivo : 03_soma_array.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.4 Vetores, 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Somar um vetor recursivamente: cada chamada resolve o PRIMEIRO elemento e
 *   delega o resto (array + 1, tamanho - 1) a outra chamada. O desenho mostra,
 *   a cada passo, qual "pedaço" do vetor cada quadro da pilha enxerga: o
 *   ponteiro avança, o vetor nunca é copiado.
 *
 * EXEMPLO REAL
 *   Somar o consumo de energia registrado em cada hora do dia. Na prática se
 *   usa um laço; a versão recursiva é o modelo mental de "resolver um e
 *   delegar o resto", que é como listas encadeadas são percorridas.
 *
 * COMPLEXIDADE (n = tamanho)
 *   Tempo: O(n)  -> n + 1 chamadas.
 *   Pilha: O(n)  -> n + 1 quadros empilhados no ponto mais fundo.
 *   Passar "array + 1" é O(1): só um endereço novo, sem copiar elementos.
 *
 * COMO EXECUTAR
 *   ./run 03_soma_array               (ENTER avança cada passo)
 *   ./run 03_soma_array < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <unistd.h>

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static const int *inicio_vetor;   /* só para desenhar a posição de cada fatia */
static size_t tamanho_total;

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* O(n): desenha o vetor inteiro e destaca entre [ ] a fatia que este quadro vê. */
static void desenha_fatia(const int *array, size_t tamanho, int nivel) {
    size_t ini = (size_t)(array - inicio_vetor);
    printf("%*s", nivel * 4, "");
    for (size_t i = 0; i < tamanho_total; i++) {
        if (i == ini) printf("[");
        printf(" %2d ", inicio_vetor[i]);
        if (tamanho > 0 && i == ini + tamanho - 1) printf("]");
    }
    if (tamanho == 0) printf("[]");
    printf("   (array = inicio + %zu, tamanho = %zu)\n", ini, tamanho);
}

/* O(n) tempo e pilha. Pré-condição: array aponta para "tamanho" ints válidos. */
static int soma_array(const int array[], size_t tamanho, int nivel) {
    printf("%*sENTRA soma(tamanho = %zu)\n", nivel * 4, "", tamanho);
    desenha_fatia(array, tamanho, nivel);
    pausar();
    if (tamanho == 0) {
        printf("%*sCASO BASE: vetor vazio, soma = 0\n", nivel * 4, "");
        return 0;
    }

    int restante = soma_array(array + 1, tamanho - 1, nivel + 1);
    int resultado = array[0] + restante;
    printf("%*sRETORNA: primeiro (%d) + soma do resto (%d) = %d\n",
           nivel * 4, "", array[0], restante, resultado);
    pausar();
    return resultado;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    int array[] = {4, 8, 15, 16, 23};
    size_t tamanho = sizeof array / sizeof array[0];
    inicio_vetor = array;
    tamanho_total = tamanho;

    printf("=== Soma recursiva: soma(v, n) = v[0] + soma(v + 1, n - 1) ===\n\n");
    int total = soma_array(array, tamanho, 0);
    printf("\nSoma = %d | chamadas = %zu | quadros no ponto mais fundo = %zu\n",
           total, tamanho + 1, tamanho + 1);
    printf("Tempo O(n), pilha O(n). Um laco for tem o mesmo tempo e pilha O(1).\n");
    return 0;
}
