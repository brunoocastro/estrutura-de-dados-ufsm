/*
 * ============================================================================
 * Arquivo : 04_quicksort_visual.c
 * Módulo  : 08 — Ordenação e Busca            (Ementa: Unidade 4.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver o QUICKSORT dividir para conquistar: o pivô marcado como <p>, os
 *   índices i e j andando durante a PARTIÇÃO (Lomuto e Hoare) e a recursão
 *   indentada por nível. Depois, provocar o PIOR CASO (vetor já ordenado com
 *   pivô = último elemento) e corrigi-lo com a MEDIANA-DE-TRÊS, medindo
 *   comparações e profundidade da recursão.
 *
 * EXEMPLO REAL
 *   É o algoritmo por trás da maioria das funções de ordenação de propósito
 *   geral (qsort da glibc em vários casos, std::sort do C++ como introsort,
 *   Arrays.sort de primitivos no Java). Num SCADA, ordenar 100 mil registros
 *   do histórico por valor para achar percentis; num CLP, ordenar a lista de
 *   pedidos de um AGV por distância. A partição sozinha também resolve
 *   "separe as peças aprovadas das reprovadas" em O(n), sem memória extra.
 *
 * COMPLEXIDADE (n = número de elementos)
 *   Operação                 Melhor      Médio        Pior        Espaço
 *   particionar (Lomuto)     O(n)        O(n)         O(n)        O(1)
 *   particionar (Hoare)      O(n)        O(n)         O(n)        O(1)
 *   quicksort                O(n log n)  O(n log n)   O(n²)       O(log n) pilha*
 *   Por quê: cada nível da recursão percorre ao todo n elementos (O(n) por
 *   nível). Se o pivô divide ao meio, há log2 n níveis → O(n log n). Se o
 *   pivô é sempre o menor ou o maior (vetor ordenado + pivô no extremo), cada
 *   partição tira só 1 elemento: n níveis de n, n-1, n-2... → n²/2.
 *   *A pilha é O(log n) no caso médio, mas O(n) no pior caso (cuidado em
 *   microcontroladores: a recursão profunda estoura a pilha!). Não é estável.
 *
 * COMO EXECUTAR
 *   ./run 04_quicksort_visual               (ENTER avança cada passo)
 *   ./run 04_quicksort_visual < /dev/null   (executa direto, sem pausas)
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

/* Estado global da demonstração (simplifica as assinaturas didáticas). */
static long comparacoes, trocas;
static int  prof_max;          /* maior profundidade de recursão atingida  */
static int  detalhe;           /* 2 = cada passo, 1 = só cada partição, 0 = mudo */
static int  n_total;           /* tamanho do vetor inteiro (para desenhar) */

/* ------------------------------------------------------------------------- */
/* Desenho                                                                    */
/* ------------------------------------------------------------------------- */

/* Desenha o vetor inteiro; fora de [lo, hi] mostra "." (não pertence a esta
 * chamada). O pivô aparece como <p>. Indentado pelo nível da recursão. O(n). */
static void desenha(const int v[], int lo, int hi, int piv, int nivel) {
    printf("%*s", 4 + 2 * nivel, "");
    for (int k = 0; k < n_total; k++) {
        if (k < lo || k > hi) printf("  . ");
        else if (k == piv)    printf("<%2d>", v[k]);
        else                  printf(" %2d ", v[k]);
    }
    printf("\n");
}

/* Linha de marcadores i e j embaixo do desenho (células de 4 colunas). */
static void marcas(int i, int j, int nivel) {
    printf("%*s", 4 + 2 * nivel, "");
    for (int k = 0; k < n_total; k++) {
        if (k == i && k == j) printf(" ij ");
        else if (k == i)      printf("  i ");
        else if (k == j)      printf("  j ");
        else                  printf("    ");
    }
    printf("\n");
}

static void troca(int v[], int a, int b) {
    if (a == b) return;
    int t = v[a]; v[a] = v[b]; v[b] = t;
    trocas++;
}

/* ------------------------------------------------------------------------- */
/* Partição de LOMUTO                                                         */
/* ------------------------------------------------------------------------- */

/* Pivô = v[hi]. Invariante: v[lo..i] <= pivô e v[i+1..j-1] > pivô.
 * j varre da esquerda para a direita; quando v[j] <= pivô, ele é trazido para
 * a região "menores" (i avança e troca). No fim o pivô vai para i + 1, que é
 * a sua POSIÇÃO DEFINITIVA. Devolve essa posição. O(n), espaço O(1).
 * Armadilha: com muitos valores iguais, todos caem do mesmo lado → O(n²). */
static int particiona_lomuto(int v[], int lo, int hi, int nivel) {
    int pivo = v[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        comparacoes++;
        int menor = v[j] <= pivo;
        if (detalhe == 2) {
            desenha(v, lo, hi, hi, nivel);
            marcas(i, j, nivel);
            printf("%*s%d <= %d ? %s\n", 4 + 2 * nivel, "", v[j], pivo,
                   menor ? "sim -> i avança e troca v[i] com v[j]" : "não -> fica à direita");
        }
        if (menor) { i++; troca(v, i, j); }
    }
    if (detalhe == 2)
        printf("%*sfim da varredura: pivô %d troca com v[i+1] = v[%d] (fronteira entre <= e >)\n",
               4 + 2 * nivel, "", pivo, i + 1);
    troca(v, i + 1, hi);                       /* pivô para o lugar definitivo */
    return i + 1;
}

/* ------------------------------------------------------------------------- */
/* Partição de HOARE (versão do Schildt, "C Completo e Total")                */
/* ------------------------------------------------------------------------- */

/* Pivô = VALOR do elemento do meio. i anda para a direita enquanto v[i] < x;
 * j anda para a esquerda enquanto v[j] > x; se i <= j, troca e avança ambos.
 * Termina quando i passa j: v[lo..j] <= x e v[i..hi] >= x.
 * Faz em média 3x menos trocas que Lomuto e lida bem com valores repetidos.
 * O pivô NÃO fica necessariamente na posição final; por isso a recursão é
 * sobre [lo..j] e [i..hi]. Devolve i e j por ponteiro. O(n). */
static void particiona_hoare(int v[], int lo, int hi, int *pi, int *pj, int nivel) {
    int meio = lo + (hi - lo) / 2;
    int x = v[meio];
    int i = lo, j = hi;
    if (detalhe == 2) printf("%*spivô x = v[%d] = %d\n", 4 + 2 * nivel, "", meio, x);
    do {
        while (comparacoes++, v[i] < x) i++;
        while (comparacoes++, x < v[j]) j--;
        if (i <= j) {
            if (detalhe == 2) {
                desenha(v, lo, hi, -1, nivel);
                marcas(i, j, nivel);
                printf("%*sv[i]=%d >= %d e v[j]=%d <= %d -> troca e avança i e j\n", 4 + 2 * nivel, "",
                       v[i], x, v[j], x);
            }
            troca(v, i, j);
            i++;
            j--;
        }
    } while (i <= j);
    *pi = i;
    *pj = j;
}

/* ------------------------------------------------------------------------- */
/* Quicksorts                                                                 */
/* ------------------------------------------------------------------------- */

static void entra(int nivel, int lo, int hi, const char *nome) {
    if (nivel > prof_max) prof_max = nivel;
    if (detalhe >= 1) printf("%*s%s(%d..%d)\n", 4 + 2 * nivel, "", nome, lo, hi);
}

/* Quicksort com Lomuto e pivô ingênuo (último elemento). */
static void quick_lomuto(int v[], int lo, int hi, int nivel) {
    entra(nivel, lo, hi, "quick");
    if (lo >= hi) return;                    /* 0 ou 1 elemento: já ordenado */
    int p = particiona_lomuto(v, lo, hi, nivel);
    if (detalhe >= 1) {
        desenha(v, lo, hi, p, nivel);
        printf("%*s-> pivô %d na posição final %d: esquerda %d elem., direita %d elem.\n", 4 + 2 * nivel, "",
               v[p], p, p - lo, hi - p);
        if (detalhe == 2) pausar();
    }
    quick_lomuto(v, lo, p - 1, nivel + 1);
    quick_lomuto(v, p + 1, hi, nivel + 1);
}

/* Mediana-de-três: ordena v[lo], v[meio], v[hi] entre si e põe a mediana em
 * v[hi], onde Lomuto espera o pivô. Em vetor ordenado ou invertido a mediana
 * é o elemento do MEIO → partições equilibradas. Custa 3 comparações. O(1). */
static void mediana_de_tres(int v[], int lo, int hi) {
    int meio = lo + (hi - lo) / 2;
    comparacoes += 3;
    if (v[meio] < v[lo]) troca(v, meio, lo);
    if (v[hi]   < v[lo]) troca(v, hi, lo);
    if (v[hi]   < v[meio]) troca(v, hi, meio);
    /* agora v[lo] <= v[meio] <= v[hi]: a mediana está em meio */
    troca(v, meio, hi);
}

static void quick_mediana3(int v[], int lo, int hi, int nivel) {
    entra(nivel, lo, hi, "quick_m3");
    if (lo >= hi) return;
    if (hi - lo >= 2) mediana_de_tres(v, lo, hi);
    int p = particiona_lomuto(v, lo, hi, nivel);
    if (detalhe >= 1) {
        desenha(v, lo, hi, p, nivel);
        printf("%*s-> pivô %d na posição %d: esquerda %d, direita %d\n", 4 + 2 * nivel, "", v[p], p,
               p - lo, hi - p);
    }
    quick_mediana3(v, lo, p - 1, nivel + 1);
    quick_mediana3(v, p + 1, hi, nivel + 1);
}

/* Quicksort de Hoare, como o qs() do Schildt. */
static void quick_hoare(int v[], int lo, int hi, int nivel) {
    entra(nivel, lo, hi, "qs");
    if (lo >= hi) return;
    int i, j;
    particiona_hoare(v, lo, hi, &i, &j, nivel);
    if (detalhe >= 1) {
        desenha(v, lo, hi, -1, nivel);
        printf("%*s-> partes [%d..%d] <= pivô <= [%d..%d]\n", 4 + 2 * nivel, "", lo, j, i, hi);
        if (detalhe == 2) pausar();
    }
    if (lo < j) quick_hoare(v, lo, j, nivel + 1);
    if (i < hi) quick_hoare(v, i, hi, nivel + 1);
}

/* ------------------------------------------------------------------------- */
/* Demonstrações                                                              */
/* ------------------------------------------------------------------------- */

static void zera(int n) { comparacoes = trocas = 0; prof_max = 0; n_total = n; }

static void imprime(const char *rot, const int v[], int n) {
    printf("  %s", rot);
    for (int k = 0; k < n; k++) printf(" %d", v[k]);
    printf("\n");
}

static void demo_lomuto(void) {
    int v[] = {38, 27, 43, 3, 9, 82, 10, 25};
    int n = (int)(sizeof v / sizeof v[0]);
    printf("=== 1) Quicksort com partição de LOMUTO (pivô = último) ===\n");
    printf("  Legenda: <p> pivô   . fora desta chamada   i fim da região <= pivô   j elemento analisado\n\n");
    imprime("entrada:", v, n);
    zera(n);
    detalhe = 2;
    quick_lomuto(v, 0, n - 1, 0);
    imprime("saída:  ", v, n);
    printf("  comparações = %ld, trocas = %ld, profundidade máx. = %d (log2 %d = %.0f)\n\n", comparacoes,
           trocas, prof_max, n, log2(n));
    pausar();
}

static void demo_hoare(void) {
    int v[] = {38, 27, 43, 3, 9, 82, 10, 25};
    int n = (int)(sizeof v / sizeof v[0]);
    printf("=== 2) Quicksort com partição de HOARE (qs do Schildt, pivô = elemento do meio) ===\n\n");
    imprime("entrada:", v, n);
    zera(n);
    detalhe = 2;
    quick_hoare(v, 0, n - 1, 0);
    imprime("saída:  ", v, n);
    printf("  comparações = %ld, trocas = %ld (compare com Lomuto acima)\n\n", comparacoes, trocas);
    pausar();
}

static void demo_pior_caso_visual(void) {
    int v[10], w[10];
    int n = 10;
    for (int k = 0; k < n; k++) v[k] = w[k] = (k + 1) * 5;
    printf("=== 3) PIOR CASO: vetor JÁ ORDENADO com pivô = último ===\n\n");
    zera(n);
    detalhe = 1;
    quick_lomuto(v, 0, n - 1, 0);
    printf("  -> a recursão vira uma ESCADA: cada partição tira só o pivô.\n");
    printf("     comparações = %ld (= n(n-1)/2 = %d), profundidade = %d (= n - 1)\n\n", comparacoes,
           n * (n - 1) / 2, prof_max);
    pausar();

    printf("  Mesmo vetor com MEDIANA-DE-TRÊS (pivô = mediana de v[lo], v[meio], v[hi]):\n\n");
    zera(n);
    quick_mediana3(w, 0, n - 1, 0);
    printf("  -> partições equilibradas: comparações = %ld, profundidade = %d\n\n", comparacoes, prof_max);
    pausar();
}

/* Tabela: pivô ingênuo x mediana-de-três x Hoare, em entrada ordenada e
 * aleatória (gerador próprio com semente fixa → resultado reprodutível). */
static void demo_tabela(void) {
    printf("=== 4) Pivô ingênuo x mediana-de-três (comparações | profundidade da recursão) ===\n\n");
    /* Cabeçalho literal: "%-10s" conta bytes, e os acentos UTF-8 desalinhariam. */
    printf("                     | Lomuto (último)        | Lomuto (mediana-de-3)  | Hoare (meio)           |\n");
    printf("        n entrada    |  comparações |   prof. |  comparações |   prof. |  comparações |   prof. |    n log2 n\n");
    printf("   ------------------+------------------------+------------------------+------------------------+------------\n");
    const int tamanhos[] = {10, 100, 1000, 5000};
    unsigned semente = 777u;
    detalhe = 0;
    for (int t = 0; t < 4; t++) {
        int n = tamanhos[t];
        int *base = malloc((size_t)n * sizeof *base), *v = malloc((size_t)n * sizeof *v);
        if (base == NULL || v == NULL) { free(base); free(v); printf("   sem memória\n"); return; }
        for (int e = 0; e < 2; e++) {
            for (int k = 0; k < n; k++) {
                semente = semente * 1103515245u + 12345u;
                base[k] = e == 0 ? k : (int)((semente >> 16) % 100000u);
            }
            void (*algs[3])(int[], int, int, int) = {quick_lomuto, quick_mediana3, quick_hoare};
            long c[3];
            int p[3];
            for (int a = 0; a < 3; a++) {
                for (int k = 0; k < n; k++) v[k] = base[k];
                zera(n);
                algs[a](v, 0, n - 1, 0);
                for (int k = 1; k < n; k++)
                    if (v[k - 1] > v[k]) { printf("   ERRO: não ordenou!\n"); break; }
                c[a] = comparacoes;
                p[a] = prof_max;
            }
            printf("   %6d %s | %12ld | %7d | %12ld | %7d | %12ld | %7d | %11.0f\n", n,
                   e == 0 ? "ordenada  " : "aleatória ", c[0], p[0], c[1], p[1], c[2], p[2], n * log2(n));
        }
        free(base);
        free(v);
    }
    printf("\n   Leitura: na entrada ORDENADA o pivô ingênuo faz ~n²/2 comparações e a recursão desce n\n");
    printf("   níveis (com n = 1 milhão isso estouraria a pilha). Mediana-de-três e pivô do meio voltam\n");
    printf("   a ~n log n. Bibliotecas reais ainda trocam para heapsort se a recursão ficar funda\n");
    printf("   demais (introsort), garantindo O(n log n) no pior caso.\n");
    pausar();
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("QUICKSORT VISUAL\n\n");
    demo_lomuto();
    demo_hoare();
    demo_pior_caso_visual();
    demo_tabela();
    return 0;
}
