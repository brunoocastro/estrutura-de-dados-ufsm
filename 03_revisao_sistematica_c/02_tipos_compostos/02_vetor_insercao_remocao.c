/*
 * ============================================================================
 * Arquivo : 02_vetor_insercao_remocao.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.4 Vetores e Matrizes)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver, passo a passo, o preço de inserir ou remover no MEIO de um vetor:
 *   todos os elementos à direita precisam andar uma casa. Acessar v[i] é O(1),
 *   mas inserir/remover na posição i é O(n - i). No fim, uma tabela compara
 *   "sempre inserir no fim" com "sempre inserir no início".
 *
 * EXEMPLO REAL
 *   Uma fila de pedidos numa IHM guardada num vetor: atender o primeiro pedido
 *   (remover v[0]) obriga a deslocar todos os outros. É exatamente esse custo
 *   que filas circulares (../../06_filas/) e listas encadeadas (../../07_listas/)
 *   eliminam.
 *
 * COMPLEXIDADE (n = elementos ocupados)
 *   Operação                 Melhor      Pior        Espaço
 *   acesso v[i]              O(1)        O(1)        O(1)
 *   inserir na posição i     O(1) (fim)  O(n) (início) O(1)
 *   remover da posição i     O(1) (fim)  O(n) (início) O(1)
 *   Por quê: o vetor é contíguo; para abrir (ou fechar) um buraco na posição i
 *   é preciso mover os n - i elementos seguintes, um de cada vez.
 *
 * COMO EXECUTAR
 *   ./run 02_vetor_insercao_remocao               (ENTER avança cada passo)
 *   ./run 02_vetor_insercao_remocao < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <unistd.h>

#define CAPACIDADE 8

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* O(capacidade): desenha o vetor; casas livres aparecem como "  .". Se marca
 * for um índice válido, desenha ^ embaixo dele. */
static void desenha(const int v[], size_t n, size_t marca) {
    printf("    indice: ");
    for (size_t i = 0; i < CAPACIDADE; i++) printf("%4zu", i);
    printf("\n    valor : ");
    for (size_t i = 0; i < CAPACIDADE; i++) {
        if (i < n) printf("%4d", v[i]);
        else printf("   .");
    }
    printf("   (n = %zu)\n", n);
    if (marca < CAPACIDADE) {
        printf("            ");
        for (size_t i = 0; i < marca; i++) printf("    ");
        printf("   ^\n");
    }
}

/*
 * Insere "valor" na posição "pos" (0 <= pos <= n < CAPACIDADE).
 * Desloca v[pos..n-1] uma casa para a direita, DE TRÁS PARA FRENTE:
 * se fosse da frente para trás, v[pos] sobrescreveria v[pos+1] antes de ele
 * ser copiado (erro clássico). Custo: n - pos movimentos -> O(n).
 * Retorna o número de movimentos, ou -1 se não houver espaço.
 */
static long inserir(int v[], size_t *n, size_t pos, int valor, int mostrar) {
    if (*n >= CAPACIDADE || pos > *n) return -1;
    long movimentos = 0;
    for (size_t i = *n; i > pos; i--) {
        v[i] = v[i - 1];
        movimentos++;
        if (mostrar) {
            printf("    move v[%zu]=%d para v[%zu]\n", i - 1, v[i - 1], i);
        }
    }
    v[pos] = valor;
    (*n)++;
    return movimentos;
}

/*
 * Remove o elemento da posição "pos" (0 <= pos < n). Desloca v[pos+1..n-1]
 * uma casa para a esquerda, agora DA FRENTE PARA TRÁS. Custo: n - pos - 1.
 */
static long remover(int v[], size_t *n, size_t pos, int mostrar) {
    if (pos >= *n) return -1;
    long movimentos = 0;
    for (size_t i = pos; i + 1 < *n; i++) {
        v[i] = v[i + 1];
        movimentos++;
        if (mostrar) printf("    move v[%zu]=%d para v[%zu]\n", i + 1, v[i + 1], i);
    }
    (*n)--;
    return movimentos;
}

/* Conta (sem desenhar) os movimentos para inserir n elementos sempre na
 * posição 0 ou sempre no fim. Usa a fórmula, pois o vetor de demonstração é
 * pequeno: no início, a k-ésima inserção move k elementos -> 0+1+...+(n-1). */
static unsigned long long custo_inserir_no_inicio(unsigned long long n) {
    return n * (n - 1) / 2;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    int v[CAPACIDADE];
    size_t n = 0;

    printf("=== Vetor de capacidade %d: inserindo 10, 20, 30, 40 no FIM ===\n", CAPACIDADE);
    int iniciais[] = {10, 20, 30, 40};
    for (size_t k = 0; k < 4; k++) {
        long m = inserir(v, &n, n, iniciais[k], 0);
        printf("  insere %d na posicao %zu (fim): %ld movimento(s)\n", iniciais[k], n - 1, m);
    }
    desenha(v, n, CAPACIDADE);
    pausar();

    printf("\n=== Inserir 25 na posicao 2 (meio) ===\n");
    printf("  passo 1: abrir espaco movendo os elementos da direita (de tras para frente)\n");
    long m = inserir(v, &n, 2, 25, 1);
    printf("  passo 2: grava 25 em v[2]\n");
    desenha(v, n, 2);
    printf("  custo: %ld movimentos (n - pos)\n", m);
    pausar();

    printf("\n=== Inserir 5 na posicao 0 (inicio: pior caso) ===\n");
    m = inserir(v, &n, 0, 5, 1);
    desenha(v, n, 0);
    printf("  custo: %ld movimentos = TODOS os elementos andaram\n", m);
    pausar();

    printf("\n=== Remover v[0] (atender o primeiro da fila) ===\n");
    m = remover(v, &n, 0, 1);
    desenha(v, n, 0);
    printf("  custo: %ld movimentos para fechar o buraco\n", m);
    pausar();

    printf("\n=== Remover o ultimo (v[n-1]) ===\n");
    m = remover(v, &n, n - 1, 1);
    desenha(v, n, CAPACIDADE);
    printf("  custo: %ld movimento(s): basta diminuir n -> O(1)\n", m);
    pausar();

    printf("\n=== Crescimento: movimentos para inserir n elementos ===\n");
    printf("%10s | %18s | %18s\n", "n", "sempre no fim", "sempre no inicio");
    printf("-----------+--------------------+-------------------\n");
    for (unsigned long long tam = 10; tam <= 10000; tam *= 10)
        printf("%10llu | %18d | %18llu\n", tam, 0, custo_inserir_no_inicio(tam));
    printf("\nNo fim: O(1) por insercao. No inicio: O(n) por insercao, O(n^2) no total.\n");
    printf("Quando o problema exige inserir/remover nas pontas com frequencia, use\n");
    printf("fila circular (06_filas) ou lista encadeada (07_listas).\n");
    return 0;
}
