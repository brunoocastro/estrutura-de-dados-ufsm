/*
 * ============================================================================
 * Arquivo : 05_merge_sort_visual.c
 * Módulo  : 08 — Ordenação e Busca            (Ementa: Unidade 4.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver o MERGE SORT (ordenação por intercalação) em duas fases: DIVIDIR o
 *   vetor ao meio até sobrar 1 elemento (recursão indentada) e INTERCALAR
 *   pares de metades já ordenadas, escolhendo a cada passo o menor da frente
 *   de cada metade. Contadores comprovam ~n log2 n comparações em QUALQUER
 *   entrada (ordenada, aleatória ou invertida).
 *
 * EXEMPLO REAL
 *   - Juntar dois logs de eventos já ordenados por timestamp (ex.: dois CLPs
 *     enviando alarmes a um mesmo SCADA) é exatamente a função `intercala`.
 *   - Ordenação EXTERNA: históricos maiores que a RAM são ordenados em blocos
 *     que cabem na memória e depois intercalados a partir do disco.
 *   - Ordenações estáveis das linguagens (Python sorted/list.sort, Java para
 *     objetos) usam Timsort, um merge sort adaptativo.
 *
 * COMPLEXIDADE (n = número de elementos)
 *   Operação        Melhor      Médio        Pior        Espaço
 *   intercalar      O(n)        O(n)         O(n)        O(n) auxiliar
 *   merge sort      O(n log n)  O(n log n)   O(n log n)  O(n) + O(log n) pilha
 *   Por quê: dividir ao meio dá log2 n níveis; em cada nível, as
 *   intercalações juntas copiam os n elementos uma vez (O(n) por nível).
 *   Diferente do quicksort, a divisão é SEMPRE ao meio: não existe pior caso
 *   quadrático. O preço é o vetor auxiliar de n posições. É ESTÁVEL (no empate
 *   pega da metade esquerda, ver `<=` em intercala).
 *
 * COMO EXECUTAR
 *   ./run 05_merge_sort_visual               (ENTER avança cada passo)
 *   ./run 05_merge_sort_visual < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static long comparacoes, copias;   /* copias = escritas no vetor auxiliar e de volta */
static int detalhe;                /* 2 = cada escolha da intercalação, 1 = cada chamada, 0 = mudo */

static void mostra_trecho(const int v[], int lo, int hi) {
    printf("[");
    for (int k = lo; k <= hi; k++) printf("%s%d", k > lo ? " " : "", v[k]);
    printf("]");
}

/* Intercala v[lo..meio] e v[meio+1..hi], ambos JÁ ORDENADOS, usando aux.
 * A cada passo compara as "frentes" a = v[i] e b = v[j] e copia a menor.
 * Quando uma metade acaba, o resto da outra é copiado sem comparar.
 * Usar `<=` (e não `<`) no empate é o que torna o merge sort ESTÁVEL.
 * O(hi - lo + 1) tempo; usa aux[lo..hi]. */
static void intercala(int v[], int aux[], int lo, int meio, int hi, int nivel) {
    int i = lo, j = meio + 1, k = lo;
    if (detalhe >= 1) {
        printf("%*sintercala ", 4 + 4 * nivel, "");
        mostra_trecho(v, lo, meio);
        printf(" + ");
        mostra_trecho(v, meio + 1, hi);
        printf("\n");
    }
    while (i <= meio && j <= hi) {
        comparacoes++;
        int pega_esq = v[i] <= v[j];
        if (detalhe == 2)
            printf("%*s  frente %d x %d -> pega %d da %s\n", 4 + 4 * nivel, "", v[i], v[j],
                   pega_esq ? v[i] : v[j], pega_esq ? "esquerda" : "direita");
        aux[k++] = pega_esq ? v[i++] : v[j++];
        copias++;
    }
    while (i <= meio) { aux[k++] = v[i++]; copias++; }   /* sobra da esquerda */
    while (j <= hi)   { aux[k++] = v[j++]; copias++; }   /* sobra da direita  */
    for (k = lo; k <= hi; k++) { v[k] = aux[k]; copias++; }
    if (detalhe >= 1) {
        printf("%*s       = ", 4 + 4 * nivel, "");
        mostra_trecho(v, lo, hi);
        printf("\n");
    }
}

/* Merge sort recursivo (top-down). Pré: aux tem pelo menos hi + 1 posições.
 * Alocar aux UMA vez fora da recursão evita n chamadas a malloc. */
static void merge_sort(int v[], int aux[], int lo, int hi, int nivel) {
    if (detalhe >= 1) {
        printf("%*s%s ", 4 + 4 * nivel, "", lo == hi ? "base " : "divide");
        mostra_trecho(v, lo, hi);
        printf("\n");
    }
    if (lo >= hi) return;                    /* 1 elemento: já ordenado */
    int meio = lo + (hi - lo) / 2;
    merge_sort(v, aux, lo, meio, nivel + 1);
    merge_sort(v, aux, meio + 1, hi, nivel + 1);
    intercala(v, aux, lo, meio, hi, nivel);
    if (detalhe >= 1 && nivel <= 1) pausar();
}

/* Desenha a árvore de divisão por níveis, sem recursão de impressão:
 * no nível d os blocos têm tamanho ~n / 2^d. Ajuda a ver os log2 n níveis. */
static void desenha_niveis(const int v[], int n) {
    printf("  Árvore de divisão (cada nível processa os %d elementos):\n", n);
    for (int tam = n, nivel = 0; ; nivel++) {
        printf("    nível %d: ", nivel);
        /* reproduz a divisão lo..hi recursiva iterativamente por nível */
        int blocos = 1 << nivel;
        for (int b = 0; b < blocos && b < n; b++) {
            int lo = (int)((long)b * n / blocos), hi = (int)((long)(b + 1) * n / blocos) - 1;
            if (lo > hi) continue;
            mostra_trecho(v, lo, hi);
            printf(" ");
        }
        printf("\n");
        if (tam <= 1) break;
        tam = (tam + 1) / 2;
    }
    printf("    (a divisão real usa meio = lo + (hi-lo)/2; os blocos acima têm os mesmos tamanhos ±1)\n\n");
}

static void imprime(const char *rot, const int v[], int n) {
    printf("  %s", rot);
    for (int k = 0; k < n; k++) printf(" %d", v[k]);
    printf("\n");
}

static void demo_visual(void) {
    int v[] = {38, 27, 43, 3, 9, 82, 10, 25};
    int n = (int)(sizeof v / sizeof v[0]);
    int aux[8];
    printf("=== 1) Divisão e intercalação passo a passo ===\n\n");
    imprime("entrada:", v, n);
    desenha_niveis(v, n);
    pausar();
    comparacoes = copias = 0;
    detalhe = 2;
    merge_sort(v, aux, 0, n - 1, 0);
    imprime("saída:  ", v, n);
    printf("  comparações = %ld (n log2 n = %.0f), cópias = %ld\n\n", comparacoes, n * log2(n), copias);
    pausar();
}

/* Intercalação como aplicação: dois logs ordenados por horário viram um. */
static void demo_logs(void) {
    printf("=== 2) Aplicação direta: unir dois logs ordenados por horário (segundos do dia) ===\n\n");
    /* posições 0..4: CLP-A ; 5..9: CLP-B (cada metade já ordenada) */
    int t[] = {28800, 29100, 30050, 31000, 33500, 28950, 29000, 30500, 32000, 34000};
    int aux[10];
    printf("  CLP-A: ");
    for (int k = 0; k < 5; k++) printf("%02d:%02d ", t[k] / 3600, t[k] / 60 % 60);
    printf("\n  CLP-B: ");
    for (int k = 5; k < 10; k++) printf("%02d:%02d ", t[k] / 3600, t[k] / 60 % 60);
    printf("\n");
    comparacoes = copias = 0;
    detalhe = 0;
    intercala(t, aux, 0, 4, 9, 0);
    printf("  SCADA: ");
    for (int k = 0; k < 10; k++) printf("%02d:%02d ", t[k] / 3600, t[k] / 60 % 60);
    printf("\n  -> uma única passada linear: %ld comparações para 10 eventos (sem reordenar tudo)\n\n",
           comparacoes);
    pausar();
}

/* Tabela: comparações praticamente iguais em qualquer entrada. */
static void demo_tabela(void) {
    printf("=== 3) Comparações do merge sort em entradas diferentes ===\n\n");
    printf("   %7s | %12s %12s %12s | %12s | %10s\n", "n", "ordenada", "aleatória", "invertida", "n log2 n",
           "aux (bytes)");
    printf("   --------+----------------------------------------+--------------+------------\n");
    unsigned semente = 4242u;
    detalhe = 0;
    for (int n = 10; n <= 100000; n *= 10) {
        int *v = malloc((size_t)n * sizeof *v), *aux = malloc((size_t)n * sizeof *aux);
        if (v == NULL || aux == NULL) { free(v); free(aux); printf("   sem memória\n"); return; }
        long c[3];
        for (int e = 0; e < 3; e++) {
            for (int k = 0; k < n; k++) {
                semente = semente * 1103515245u + 12345u;
                v[k] = e == 0 ? k : e == 2 ? n - k : (int)((semente >> 16) % 100000u);
            }
            comparacoes = copias = 0;
            merge_sort(v, aux, 0, n - 1, 0);
            c[e] = comparacoes;
        }
        printf("   %7d | %12ld %12ld %12ld | %12.0f | %10zu\n", n, c[0], c[1], c[2], n * log2(n),
               (size_t)n * sizeof *aux);
        free(v);
        free(aux);
    }
    printf("\n   Leitura: as três colunas crescem como n log n e ficam ABAIXO de n log2 n. A entrada\n");
    printf("   ordenada só economiza comparações (uma metade acaba antes), nunca muda a ordem de\n");
    printf("   grandeza. O custo extra é a memória auxiliar, que cresce com n.\n");
    pausar();
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("MERGE SORT VISUAL (DIVIDIR E INTERCALAR)\n\n");
    demo_visual();
    demo_logs();
    demo_tabela();
    return 0;
}
