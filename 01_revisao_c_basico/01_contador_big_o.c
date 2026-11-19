/*
 * ============================================================================
 * Arquivo : 01_contador_big_o.c
 * Módulo  : 01 — Revisão de C básico          (Ementa: Unidade 0.2 a 0.4)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver, com contadores, como cresce o número de operações de cada classe de
 *   complexidade: O(1), O(log n), O(n), O(n log n), O(n²) e O(2^n).
 *   Primeiro cada classe é executada passo a passo com n pequeno; depois uma
 *   tabela compara as contagens para n = 10, 100, 1000 e 10000.
 *
 * EXEMPLO REAL
 *   Cada classe é ilustrada com uma tarefa de automação:
 *     O(1)        ler a leitura do instante k em um vetor de amostras
 *     O(log n)    achar um setpoint em uma tabela ordenada (busca binária)
 *     O(n)        varrer todas as tags de um CLP procurando alarmes
 *     O(n log n)  ordenar o histórico de um supervisório (merge sort)
 *     O(n²)       comparar todos os pares de sensores (procura duplicatas)
 *     O(2^n)      testar todas as combinações liga/desliga de n válvulas
 *
 * COMPLEXIDADE (n = tamanho da entrada)
 *   Função              Operações contadas       Big O
 *   conta_constante     1                        O(1)
 *   conta_logaritmica   piso(log2 n)             O(log n)
 *   conta_linear        n                        O(n)
 *   conta_n_log_n       n · teto(log2 n)         O(n log n)
 *   conta_quadratica    n · (n - 1) / 2          O(n²)
 *   conta_exponencial   2^n                      O(2^n)
 *   Espaço: O(1) em todas (só contadores).
 *   Por quê: cada função tem a estrutura de laço típica da sua classe; o
 *   contador soma 1 a cada execução da "operação básica" do laço mais interno.
 *
 * COMO EXECUTAR
 *   ./run 01_contador_big_o               (ENTER avança cada passo)
 *   ./run 01_contador_big_o < /dev/null   (executa direto, sem pausas)
 *   Teoria: complexidade_big_o.md
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <math.h>
#include <stdio.h>
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

/* unsigned long long: O(n²) com n = 10000 já passa de 4 · 10^7, e 2^n cresce
   tão rápido que um int de 32 bits estoura em n = 31. */
typedef unsigned long long contador_t;

/* O(1): acessar a posição do meio custa sempre uma operação, qualquer que seja n. */
static contador_t conta_constante(int n) {
    (void)n;              /* n não influencia o custo: essa é a definição de O(1) */
    return 1;
}

/* O(log n): a cada volta a faixa restante cai pela metade, como na busca
   binária. Quantas vezes dá para dividir n por 2 até chegar a 1? log2(n). */
static contador_t conta_logaritmica(int n, int mostrar) {
    contador_t ops = 0;
    int faixa = n;
    while (faixa > 1) {
        if (mostrar) printf("      faixa = %4d  -> metade = %4d\n", faixa, faixa / 2);
        faixa /= 2;
        ops++;
    }
    return ops;
}

/* O(n): uma operação por elemento (varrer todas as tags uma vez). */
static contador_t conta_linear(int n) {
    contador_t ops = 0;
    for (int i = 0; i < n; i++)
        ops++;
    return ops;
}

/* O(n log n): para cada um dos n elementos, um laço que dobra (log n voltas).
   É o formato do trabalho do merge sort: log n níveis, n operações por nível. */
static contador_t conta_n_log_n(int n) {
    contador_t ops = 0;
    for (int i = 0; i < n; i++)
        for (int passo = 1; passo < n; passo *= 2)
            ops++;
    return ops;
}

/* O(n²): compara cada par (i, j) com i < j uma única vez -> n(n-1)/2 pares.
   A constante 1/2 some no Big O: continua sendo O(n²). */
static contador_t conta_quadratica(int n, int mostrar) {
    contador_t ops = 0;
    for (int i = 0; i < n; i++) {
        if (mostrar) printf("      i=%d compara com:", i);
        for (int j = i + 1; j < n; j++) {
            if (mostrar) printf(" %d", j);
            ops++;
        }
        if (mostrar) printf("%s\n", i == n - 1 ? " (nenhum: já comparado)" : "");
    }
    return ops;
}

/* O(2^n): percorre todos os estados possíveis de n válvulas (cada bit do
   inteiro 'estado' é uma válvula). Cada válvula a mais DOBRA o trabalho.
   Pré-condição: n <= 30, senão não termina em tempo útil (nem cabe na tela). */
static contador_t conta_exponencial(int n, int mostrar) {
    contador_t ops = 0;
    unsigned long total = 1UL << n;
    for (unsigned long estado = 0; estado < total; estado++) {
        if (mostrar) {
            printf("      estado %2lu: ", estado);
            for (int v = n - 1; v >= 0; v--)
                printf("V%d=%s ", v + 1, (estado >> v) & 1UL ? "ON " : "off");
            printf("\n");
        }
        ops++;
    }
    return ops;
}

/* Barra horizontal: um '#' para cada 'escala' operações, limitada a 'largura'. */
static void barra(contador_t ops, contador_t escala, int largura) {
    contador_t marcas = (ops + escala - 1) / escala;
    for (contador_t k = 0; k < marcas && k < (contador_t)largura; k++)
        putchar('#');
    if (marcas > (contador_t)largura)
        printf("... (%llu '#', não cabe na tela)", marcas);
    putchar('\n');
}

/* Parte 1: cada classe executada com n pequeno, mostrando o que é contado. */
static void demonstra_passo_a_passo(void) {
    printf("PARTE 1 — O que cada classe conta (n pequeno)\n");
    printf("================================================================\n\n");

    printf("[O(1)] Ler a amostra do instante k = 3 em um vetor de n = 8 leituras.\n");
    printf("       v[3] é calculado direto: endereço = início + 3 × tamanho.\n");
    printf("       operações = %llu (seria 1 também com n = 1 milhão)\n\n",
           conta_constante(8));
    pausar();

    printf("[O(log n)] Busca binária em tabela de n = 64 setpoints ordenados.\n");
    printf("           Cada comparação descarta metade da faixa:\n");
    contador_t ops = conta_logaritmica(64, 1);
    printf("           operações = %llu = log2(64)\n\n", ops);
    pausar();

    printf("[O(n)] Varredura de n = 8 tags procurando alarme: 1 teste por tag.\n");
    printf("       ");
    for (int i = 0; i < 8; i++) printf("[tag%d] ", i);
    printf("\n       operações = %llu = n\n\n", conta_linear(8));
    pausar();

    printf("[O(n log n)] Para cada um dos n = 8 elementos, um laço que dobra\n");
    printf("             (passo = 1, 2, 4): 3 voltas = log2(8).\n");
    printf("             operações = %llu = 8 × 3\n\n", conta_n_log_n(8));
    pausar();

    printf("[O(n²)] Comparar todos os pares de n = 5 sensores (procura duplicatas):\n");
    ops = conta_quadratica(5, 1);
    printf("        operações = %llu = 5 × 4 / 2  (metade de n², mas ainda O(n²))\n\n",
           ops);
    pausar();

    printf("[O(2^n)] Testar todas as combinações de n = 3 válvulas:\n");
    ops = conta_exponencial(3, 1);
    printf("         operações = %llu = 2^3. Com 4 válvulas seriam 16; com 30, ~1 bilhão.\n\n",
           ops);
    pausar();
}

/* Parte 2: tabela de crescimento com contagens reais (exceto 2^n grande). */
static void tabela_crescimento(void) {
    const int tamanhos[] = {10, 100, 1000, 10000};
    const int qtd = (int)(sizeof tamanhos / sizeof tamanhos[0]);

    printf("PARTE 2 — Tabela de crescimento (contagens reais dos laços)\n");
    printf("================================================================\n");
    /* Cabeçalho escrito à mão: "²" ocupa 2 bytes e desalinharia o %12s. */
    printf("      n | O(1) | O(log n) |   O(n) | O(n log n) |        O(n²) | O(2^n)\n");
    printf("--------+------+----------+--------+------------+--------------+-----------\n");
    for (int k = 0; k < qtd; k++) {
        int n = tamanhos[k];
        printf("%7d | %4llu | %8llu | %6llu | %10llu | %12llu | ",
               n, conta_constante(n), conta_logaritmica(n, 0), conta_linear(n),
               conta_n_log_n(n), conta_quadratica(n, 0));
        if (n <= 20)
            printf("%llu\n", conta_exponencial(n, 0));
        else   /* executar seria inviável: mostramos só a estimativa */
            printf("~10^%.0f (inviável)\n", n * log10(2.0));
    }
    printf("\nRepare: n cresce 10× por linha. O(n) cresce 10×, O(n²) cresce 100×,\n");
    printf("O(log n) ganha só ~3 operações, e O(2^n) deixa de ser executável.\n\n");
    pausar();
}

/* Parte 3: barras para n = 16, cada '#' = 16 operações. */
static void grafico_barras(void) {
    const int n = 16;
    const contador_t escala = 16;
    printf("PARTE 3 — Barras para n = %d (cada '#' = %llu operações)\n", n, escala);
    printf("================================================================\n");
    printf("O(1)        "); barra(conta_constante(n), escala, 60);
    printf("O(log n)    "); barra(conta_logaritmica(n, 0), escala, 60);
    printf("O(n)        "); barra(conta_linear(n), escala, 60);
    printf("O(n log n)  "); barra(conta_n_log_n(n), escala, 60);
    printf("O(n²)       "); barra(conta_quadratica(n, 0), escala, 60);
    printf("O(2^n)      "); barra(conta_exponencial(n, 0), escala, 60);

    /* Estimativa de tempo: um PC faz ~10^8 operações simples por segundo. */
    printf("\nTempo estimado para n = 1 000 000 a 10^8 operações/s:\n");
    printf("  O(log n)   %10.1e s\n", log2(1e6) / 1e8);
    printf("  O(n)       %10.1e s\n", 1e6 / 1e8);
    printf("  O(n log n) %10.1e s\n", 1e6 * log2(1e6) / 1e8);
    printf("  O(n²)      %10.1e s  (~%.1f horas)\n", 1e12 / 1e8, 1e12 / 1e8 / 3600.0);
    printf("Conclusão: a escolha do algoritmo pesa mais que a velocidade da máquina.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("Contador de operações: Big O na prática\n\n");
    demonstra_passo_a_passo();
    tabela_crescimento();
    grafico_barras();
    return 0;
}
