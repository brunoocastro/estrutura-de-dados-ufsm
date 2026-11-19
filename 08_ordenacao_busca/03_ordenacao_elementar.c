/*
 * ============================================================================
 * Arquivo : 03_ordenacao_elementar.c
 * Módulo  : 08 — Ordenação e Busca            (Ementa: Unidade 4.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver os três métodos elementares — BOLHA (com parada antecipada), SELEÇÃO
 *   e INSERÇÃO — trabalhando no mesmo vetor, com cada comparação narrada e o
 *   vetor desenhado em barras ao fim de cada passada. Contadores de
 *   comparações e trocas mostram o custo O(n²), o melhor caso O(n) da bolha e
 *   da inserção em vetor já ordenado, e uma demonstração de ESTABILIDADE
 *   (quem preserva a ordem de chaves iguais).
 *
 * EXEMPLO REAL
 *   - Inserção: ordenar poucas leituras (janela de um filtro, 5 a 15 valores)
 *     num microcontrolador; também é o "acabamento" usado dentro de
 *     quicksort/introsort das bibliotecas quando o pedaço fica pequeno (< ~16).
 *   - Bolha com parada: verificar/ajustar uma lista QUASE ordenada (ex.: fila
 *     de receitas da IHM em que só um item mudou de prioridade).
 *   - Seleção: faz o mínimo de trocas (n - 1), útil quando escrever é caro
 *     (memória flash/EEPROM com ciclos de escrita limitados).
 *   - Dia a dia: organizar cartas na mão (inserção).
 *
 * COMPLEXIDADE (n = número de elementos)
 *   Algoritmo   Melhor   Médio   Pior    Espaço  Estável  Trocas no pior
 *   bolha       O(n)     O(n²)   O(n²)   O(1)    sim      n(n-1)/2
 *   seleção     O(n²)    O(n²)   O(n²)   O(1)    NÃO      n - 1
 *   inserção    O(n)     O(n²)   O(n²)   O(1)    sim      n(n-1)/2 desloc.
 *   Por quê: bolha e seleção têm dois laços aninhados → ~n²/2 comparações.
 *   A bolha com flag para na primeira passada sem trocas (n - 1 comparações
 *   num vetor ordenado). A inserção só anda para trás enquanto o vizinho é
 *   maior; num vetor ordenado cada elemento faz 1 comparação. A seleção
 *   sempre procura o mínimo no resto inteiro: não tem melhor caso.
 *
 * COMO EXECUTAR
 *   ./run 03_ordenacao_elementar               (ENTER avança cada passo)
 *   ./run 03_ordenacao_elementar < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* Cada item tem uma CHAVE (o que se ordena) e um RÓTULO (só para enxergar a
 * estabilidade: dois itens com chave 3 viram 3a e 3b). */
typedef struct {
    int chave;
    char rotulo;
} Item;

typedef struct {
    long comp;     /* comparações entre chaves                                 */
    long trocas;   /* trocas (bolha/seleção) ou deslocamentos (inserção)       */
} Stats;

/* ------------------------------------------------------------------------- */
/* Desenho                                                                    */
/* ------------------------------------------------------------------------- */

/* Uma linha compacta: [x] = par sendo comparado, (x) = região já ordenada
 * [ord_ini..ord_fim]; ord_ini > ord_fim significa "nenhuma". O(n). */
static void linha(const Item v[], int n, int a, int b, int ord_ini, int ord_fim) {
    printf("      ");
    for (int k = 0; k < n; k++) {
        if (k == a || k == b)              printf("[%2d]", v[k].chave);
        else if (k >= ord_ini && k <= ord_fim) printf("(%2d)", v[k].chave);
        else                               printf(" %2d ", v[k].chave);
    }
}

/* Barras horizontais: '=' para a região ordenada, '#' para o resto. O(n · max). */
static void barras(const Item v[], int n, int ord_ini, int ord_fim) {
    for (int k = 0; k < n; k++) {
        int ordenado = k >= ord_ini && k <= ord_fim;
        printf("      v[%d] %2d |", k, v[k].chave);
        for (int t = 0; t < v[k].chave; t++) putchar(ordenado ? '=' : '#');
        printf("\n");
    }
}

static void troca(Item *x, Item *y) { Item t = *x; *x = *y; *y = t; }

/* Imprime s alinhado à esquerda em `largura` colunas. printf("%-10s") conta
 * BYTES, e em UTF-8 "ç"/"ã" ocupam 2: contamos só os bytes que iniciam um
 * caractere para não desalinhar as tabelas. */
static void col(const char *s, int largura) {
    int visiveis = 0;
    for (const char *p = s; *p; p++)
        if (((unsigned char)*p & 0xC0) != 0x80) visiveis++;
    printf("%s%*s", s, largura > visiveis ? largura - visiveis : 0, "");
}

/* ------------------------------------------------------------------------- */
/* Algoritmos (mostrar = 1 desenha; 0 só conta)                               */
/* ------------------------------------------------------------------------- */

/* BOLHA com parada antecipada. Cada passada leva o maior elemento restante
 * para o fim ("borbulha"), então o limite `fim` diminui a cada passada.
 * Se uma passada inteira não troca nada, o vetor já está ordenado: para.
 * Estável porque só troca quando v[j] > v[j+1] (estritamente): iguais nunca
 * passam um pelo outro. Trocar `>` por `>=` quebraria a estabilidade. */
static void bolha(Item v[], int n, Stats *s, int mostrar) {
    for (int fim = n - 1; fim > 0; fim--) {
        int trocou = 0;
        if (mostrar) printf("    Passada %d (leva o maior de v[0..%d] para v[%d]):\n", n - fim, fim, fim);
        for (int j = 0; j < fim; j++) {
            s->comp++;
            int fora = v[j].chave > v[j + 1].chave;
            if (mostrar) {
                linha(v, n, j, j + 1, fim + 1, n - 1);
                printf("  %d > %d ? %s\n", v[j].chave, v[j + 1].chave, fora ? "sim -> troca" : "não");
            }
            if (fora) { troca(&v[j], &v[j + 1]); s->trocas++; trocou = 1; }
        }
        if (mostrar) { barras(v, n, fim, n - 1); pausar(); }
        if (!trocou) {
            if (mostrar) printf("    Nenhuma troca nesta passada -> já está ordenado, PARA antes.\n");
            break;
        }
    }
}

/* SELEÇÃO: na passada i, procura o MENOR de v[i..n-1] e troca com v[i].
 * Faz no máximo n - 1 trocas, mas SEMPRE n(n-1)/2 comparações.
 * Não é estável: a troca à distância pode pular um item por cima de outro
 * de mesma chave (ex.: [2a 2b 1] -> troca 2a com 1 -> [1 2b 2a]). */
static void selecao(Item v[], int n, Stats *s, int mostrar) {
    for (int i = 0; i < n - 1; i++) {
        int menor = i;
        if (mostrar) printf("    Passada %d (procura o menor de v[%d..%d]):\n", i + 1, i, n - 1);
        for (int j = i + 1; j < n; j++) {
            s->comp++;
            if (mostrar) {
                linha(v, n, menor, j, 0, i - 1);
                printf("  %d < menor (%d) ? %s\n", v[j].chave, v[menor].chave,
                       v[j].chave < v[menor].chave ? "sim -> novo menor" : "não");
            }
            if (v[j].chave < v[menor].chave) menor = j;
        }
        if (menor != i) { troca(&v[i], &v[menor]); s->trocas++; }
        if (mostrar) {
            printf("      troca v[%d] com v[%d]%s\n", i, menor, menor == i ? " (já está no lugar: sem troca)" : "");
            barras(v, n, 0, i);
            pausar();
        }
    }
}

/* INSERÇÃO: v[0..i-1] já está ordenado; retira x = v[i] e desloca para a
 * direita todos os maiores que x, abrindo um buraco onde x entra.
 * Deslocar (1 atribuição) é mais barato que trocar (3 atribuições).
 * Estável: para no primeiro elemento <= x, sem passar por cima dos iguais. */
static void insercao(Item v[], int n, Stats *s, int mostrar) {
    for (int i = 1; i < n; i++) {
        Item x = v[i];
        int j = i - 1;
        if (mostrar) printf("    Passo %d: insere %d na parte ordenada v[0..%d]\n", i, x.chave, i - 1);
        while (j >= 0) {
            s->comp++;
            int maior = v[j].chave > x.chave;
            if (mostrar) {
                linha(v, n, j, -1, 0, i - 1);
                printf("  %d > %d ? %s\n", v[j].chave, x.chave, maior ? "sim -> desloca para a direita" : "não -> para");
            }
            if (!maior) break;
            v[j + 1] = v[j];
            s->trocas++;
            j--;
        }
        v[j + 1] = x;
        if (mostrar) {
            printf("      %d entra em v[%d]\n", x.chave, j + 1);
            barras(v, n, 0, i);
            pausar();
        }
    }
}

typedef void (*Ordenador)(Item[], int, Stats *, int);

/* ------------------------------------------------------------------------- */
/* Demonstrações                                                              */
/* ------------------------------------------------------------------------- */

/* Lê até `max` inteiros (1..30) numa linha; devolve quantos leu, 0 se ENTER
 * vazio ou EOF (aí o programa usa o exemplo padrão). */
static int le_vetor(Item v[], int max) {
    char buf[256];
    printf("Digite até %d inteiros entre 1 e 30 separados por espaço (ENTER = exemplo): ", max);
    fflush(stdout);
    if (fgets(buf, sizeof buf, stdin) == NULL) { printf("\n"); return 0; }
    int n = 0;
    char *p = buf, *fim;
    while (n < max) {
        long x = strtol(p, &fim, 10);
        if (fim == p) break;                 /* não há mais número */
        if (x < 1 || x > 30) { printf("  (ignorando %ld: fora de 1..30)\n", x); p = fim; continue; }
        v[n].chave = (int)x;
        v[n].rotulo = ' ';
        n++;
        p = fim;
    }
    return n;
}

static void demo_visual(const Item base[], int n) {
    const char *nomes[] = {"BOLHA (com parada antecipada)", "SELEÇÃO", "INSERÇÃO"};
    Ordenador algs[] = {bolha, selecao, insercao};
    Stats res[3];
    for (int a = 0; a < 3; a++) {
        Item v[16];
        memcpy(v, base, (size_t)n * sizeof *v);
        Stats s = {0, 0};
        printf("\n=== %d) %s ===\n  Vetor inicial:\n", a + 1, nomes[a]);
        barras(v, n, 1, 0);
        pausar();
        algs[a](v, n, &s, 1);
        printf("  -> %s: %ld comparações, %ld %s\n", nomes[a], s.comp, s.trocas,
               a == 2 ? "deslocamentos" : "trocas");
        res[a] = s;
        pausar();
    }
    printf("\n  Resumo para este vetor (n = %d, n(n-1)/2 = %d):\n", n, n * (n - 1) / 2);
    for (int a = 0; a < 3; a++) {
        printf("    ");
        col(nomes[a], 30);
        printf(" comparações = %3ld   trocas/desloc. = %3ld\n", res[a].comp, res[a].trocas);
    }
    pausar();
}

/* Melhor/pior caso medidos: vetor ordenado, aleatório e invertido. */
static void demo_casos(void) {
    printf("\n=== 4) Melhor x pior caso (contagens, sem desenho) ===\n\n");
    const char *nomes[] = {"bolha", "seleção", "inserção"};
    Ordenador algs[] = {bolha, selecao, insercao};
    const char *entradas[] = {"ordenado", "aleatório", "invertido"};
    const int tamanhos[] = {100, 1000};
    unsigned semente = 12345u;                 /* gerador próprio: saída reprodutível */

    printf("       n entrada    | método         comp.     trocas | referência\n");
    printf("   -----------------+--------------------------------+-------------------------\n");
    for (int t = 0; t < 2; t++) {
        int n = tamanhos[t];
        Item *base = malloc((size_t)n * sizeof *base), *v = malloc((size_t)n * sizeof *v);
        if (base == NULL || v == NULL) { free(base); free(v); printf("   sem memória\n"); return; }
        for (int e = 0; e < 3; e++) {
            for (int i = 0; i < n; i++) {
                semente = semente * 1103515245u + 12345u;
                base[i].chave = e == 0 ? i : e == 2 ? n - i : (int)((semente >> 16) % 10000u);
                base[i].rotulo = ' ';
            }
            for (int a = 0; a < 3; a++) {
                memcpy(v, base, (size_t)n * sizeof *v);
                Stats s = {0, 0};
                algs[a](v, n, &s, 0);
                const char *ref = "";
                if (a == 1)                 ref = "sempre n(n-1)/2 comp.";
                else if (e == 0)            ref = "MELHOR caso: n-1 comp.";
                else if (e == 2)            ref = "PIOR caso: n(n-1)/2";
                printf("   %5d ", n);
                col(entradas[e], 10);
                printf(" | ");
                col(nomes[a], 9);
                printf(" %10ld %10ld | %s\n", s.comp, s.trocas, ref);
            }
        }
        printf("   -----------------+--------------------------------+-------------------------\n");
        free(base);
        free(v);
    }
    printf("   Leitura: com n 10x maior, o aleatório/invertido custa ~100x (quadrático); a bolha e a\n");
    printf("   inserção no vetor ordenado custam só ~10x (linear). Seleção: sempre ~n²/2.\n");
    printf("   Note que bolha e inserção movem EXATAMENTE o mesmo número de vezes: cada troca ou\n");
    printf("   deslocamento desfaz uma INVERSÃO (par fora de ordem), e o vetor tem um número fixo delas.\n");
    pausar();
}

/* Estabilidade: itens com a mesma chave devem manter a ordem original. */
static void demo_estabilidade(void) {
    printf("\n=== 5) Estabilidade: chaves iguais mantêm a ordem de chegada? ===\n\n");
    const Item base[] = {{2, 'a'}, {3, 'a'}, {2, 'b'}, {1, 'a'}, {3, 'b'}, {2, 'c'}};
    const int n = (int)(sizeof base / sizeof base[0]);
    const char *nomes[] = {"bolha", "seleção", "inserção"};
    Ordenador algs[] = {bolha, selecao, insercao};

    printf("   original:  ");
    for (int i = 0; i < n; i++) printf("%d%c ", base[i].chave, base[i].rotulo);
    printf("   (a chegou antes de b, que chegou antes de c)\n");
    for (int a = 0; a < 3; a++) {
        Item v[8];
        memcpy(v, base, sizeof base);
        Stats s = {0, 0};
        algs[a](v, n, &s, 0);
        int estavel = 1;
        for (int i = 1; i < n; i++)
            if (v[i].chave == v[i - 1].chave && v[i].rotulo < v[i - 1].rotulo) estavel = 0;
        printf("   ");
        col(nomes[a], 10);
        printf(" ");
        for (int i = 0; i < n; i++) printf("%d%c ", v[i].chave, v[i].rotulo);
        printf("   -> %s\n", estavel ? "ESTÁVEL" : "NÃO estável (a ordem entre iguais mudou)");
    }
    printf("\n   Por que importa: ao ordenar alarmes por prioridade, um método estável mantém os de\n");
    printf("   mesma prioridade em ordem cronológica (ver 08_ranking_alarmes_estavel).\n");
    pausar();
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("ORDENAÇÃO ELEMENTAR: BOLHA, SELEÇÃO E INSERÇÃO\n\n");

    Item v[10];
    int n = le_vetor(v, 10);
    if (n < 2) {
        const int exemplo[] = {9, 4, 12, 2, 7, 5};
        n = (int)(sizeof exemplo / sizeof exemplo[0]);
        for (int i = 0; i < n; i++) { v[i].chave = exemplo[i]; v[i].rotulo = ' '; }
        printf("Usando o exemplo padrão.\n");
    }
    printf("Legenda: [x] par comparado   (x) região já ordenada   '=' barra ordenada   '#' barra a ordenar\n");

    demo_visual(v, n);
    demo_casos();
    demo_estabilidade();
    return 0;
}
