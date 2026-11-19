/*
 * ============================================================================
 * Arquivo : 02_pesquisa_binaria.c
 * Módulo  : 08 — Ordenação e Busca            (Ementa: Unidade 4.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a janela [ini, meio, fim] da pesquisa binária encolhendo pela metade a
 *   cada passo (versão iterativa e recursiva), medir quantas sondagens ela faz
 *   em comparação com a pesquisa sequencial para n = 10 até 1.000.000 e usar a
 *   variante "limite inferior" (primeiro elemento >= x) para interpolar uma
 *   tabela de calibração.
 *   A versão recursiva mínima já está em
 *   ../03_revisao_sistematica_c/04_recursao/04_busca_binaria.c; aqui o foco é
 *   o desenho da janela, o custo e as aplicações.
 *
 * EXEMPLO REAL
 *   - Almoxarifado: localizar uma peça pelo código num cadastro ORDENADO.
 *   - Instrumentação: um termopar/NTC é lido pelo ADC e convertido em °C por
 *     uma tabela de calibração ordenada pela leitura; o firmware acha o
 *     intervalo certo por busca binária e interpola linearmente.
 *   - Dia a dia: procurar uma palavra no dicionário abrindo no meio.
 *
 * COMPLEXIDADE (n = número de elementos)
 *   Operação              Melhor   Médio       Pior        Espaço
 *   binária iterativa     O(1)     O(log n)    O(log n)    O(1)
 *   binária recursiva     O(1)     O(log n)    O(log n)    O(log n) pilha
 *   limite inferior       O(log n) O(log n)    O(log n)    O(1)
 *   Por quê: cada sondagem descarta metade da janela; depois de k sondagens
 *   restam n / 2^k elementos, que chega a 1 quando k = log2 n. Para n = 10^6
 *   isso dá ~20 sondagens, contra ~500.000 da sequencial (média).
 *   Pré-requisito: o vetor precisa estar ORDENADO (ordenar custa O(n log n),
 *   e vale a pena quando há muitas buscas para cada inserção).
 *
 * COMO EXECUTAR
 *   ./run 02_pesquisa_binaria               (ENTER avança cada passo)
 *   ./run 02_pesquisa_binaria < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <math.h>
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

/* ------------------------------------------------------------------------- */
/* Desenho da janela                                                          */
/* ------------------------------------------------------------------------- */

/* Desenha o vetor em células de 6 colunas. Fora de [ini, fim] os valores
 * aparecem como "." (já descartados). Embaixo, marca ini (I), meio (M) e
 * fim (F); se coincidirem, as letras são juntadas (ex.: "IMF"). O(n). */
static void desenha_janela(const int v[], int n, int ini, int meio, int fim, int indent) {
    printf("%*s  idx:", indent, "");
    for (int k = 0; k < n; k++) printf("%6d", k);
    printf("\n%*s  val:", indent, "");
    for (int k = 0; k < n; k++) {
        if (k < ini || k > fim) printf("%6s", ".");
        else                    printf("%6d", v[k]);
    }
    printf("\n%*s      ", indent, "");
    for (int k = 0; k < n; k++) {
        char m[8] = "";
        if (k == ini)  strcat(m, "I");
        if (k == meio) strcat(m, "M");
        if (k == fim)  strcat(m, "F");
        printf("%6s", m);
    }
    printf("\n");
}

/* ------------------------------------------------------------------------- */
/* Algoritmos                                                                 */
/* ------------------------------------------------------------------------- */

/* Pesquisa binária ITERATIVA. Pré-condição: v ordenado crescente.
 * Devolve o índice de `chave` ou -1. *sondagens conta acessos v[meio].
 * Por que meio = ini + (fim - ini) / 2 e não (ini + fim) / 2? Com índices
 * grandes, ini + fim pode estourar o int (bug real que ficou anos na
 * biblioteca do Java). A forma usada nunca passa de fim. O(log n). */
static int binaria_iterativa(const int v[], int n, int chave, long *sondagens, int mostrar) {
    int ini = 0, fim = n - 1, passo = 1;
    while (ini <= fim) {                      /* janela não vazia */
        int meio = ini + (fim - ini) / 2;
        (*sondagens)++;
        if (mostrar) {
            desenha_janela(v, n, ini, meio, fim, 4);
            printf("      passo %d: janela [%d..%d] (%d elem.), v[%d] = %d ", passo++, ini, fim,
                   fim - ini + 1, meio, v[meio]);
        }
        if (v[meio] == chave) {
            if (mostrar) printf("== %d -> ACHOU\n", chave);
            return meio;
        }
        if (chave < v[meio]) {                /* chave está à esquerda */
            if (mostrar) { printf("> %d -> descarta a DIREITA (fim = %d)\n", chave, meio - 1); pausar(); }
            fim = meio - 1;
        } else {                              /* chave está à direita */
            if (mostrar) { printf("< %d -> descarta a ESQUERDA (ini = %d)\n", chave, meio + 1); pausar(); }
            ini = meio + 1;
        }
    }
    if (mostrar) printf("      janela vazia (ini = %d > fim = %d): chave %d não existe\n", ini, fim, chave);
    return -1;
}

/* Pesquisa binária RECURSIVA: mesma lógica, mas a janela vai nos parâmetros.
 * A indentação mostra a profundidade da recursão (= número de sondagens).
 * Espaço O(log n) por causa dos quadros de pilha; como a chamada recursiva é a
 * última ação, um compilador com otimização pode transformá-la em laço. */
static int binaria_recursiva(const int v[], int ini, int fim, int chave, int nivel) {
    printf("%*sbusca(ini=%d, fim=%d)", 4 + 4 * nivel, "", ini, fim);
    if (ini > fim) { printf(" -> janela vazia, devolve -1\n"); return -1; }
    int meio = ini + (fim - ini) / 2;
    printf(" meio=%d v[meio]=%d", meio, v[meio]);
    if (v[meio] == chave) { printf(" -> ACHOU, devolve %d\n", meio); return meio; }
    printf(chave < v[meio] ? " -> desce pela esquerda\n" : " -> desce pela direita\n");
    int r = chave < v[meio] ? binaria_recursiva(v, ini, meio - 1, chave, nivel + 1)
                            : binaria_recursiva(v, meio + 1, fim, chave, nivel + 1);
    printf("%*s<- retorna %d\n", 4 + 4 * nivel, "", r);
    return r;
}

/* Limite inferior (lower bound): menor índice i com v[i] >= x; devolve n se
 * todos forem menores. Útil quando a chave exata NÃO existe e queremos o
 * intervalo onde ela cairia (tabelas, inserção ordenada). Janela semiaberta
 * [ini, fim): o laço termina com ini == fim. O(log n). */
static int limite_inferior(const double v[], int n, double x) {
    int ini = 0, fim = n;
    while (ini < fim) {
        int meio = ini + (fim - ini) / 2;
        if (v[meio] < x) ini = meio + 1;
        else             fim = meio;
    }
    return ini;
}

/* Sequencial em vetor ordenado (para comparação justa): conta sondagens. O(n). */
static int sequencial(const int v[], int n, int chave, long *sondagens) {
    for (int i = 0; i < n; i++) {
        (*sondagens)++;
        if (v[i] == chave) return i;
    }
    return -1;
}

/* ------------------------------------------------------------------------- */
/* Demonstrações                                                              */
/* ------------------------------------------------------------------------- */

static void demo_janela(void) {
    const int v[] = {3, 8, 12, 17, 25, 31, 42, 47, 53, 60, 68, 74, 81, 89, 95, 99};
    const int n = (int)(sizeof v / sizeof v[0]);
    long s = 0;

    printf("=== 1) Binária iterativa: a janela [I..F] encolhe pela metade ===\n\n");
    printf("   Procurando 68 em %d elementos ordenados:\n", n);
    int r = binaria_iterativa(v, n, 68, &s, 1);
    printf("   -> índice %d em %ld sondagens (a sequencial faria %d)\n\n", r, s, r + 1);
    pausar();

    s = 0;
    printf("   Procurando 50 (não existe):\n");
    r = binaria_iterativa(v, n, 50, &s, 1);
    printf("   -> %d após %ld sondagens; log2(%d) = %.0f. A sequencial olharia os %d.\n\n", r, s, n,
           log2(n), n);
    pausar();

    printf("=== 2) Binária recursiva: cada chamada é um nível de indentação ===\n\n");
    printf("   Procurando 17:\n");
    binaria_recursiva(v, 0, n - 1, 17, 0);
    printf("\n");
    pausar();
}

/* Tabela de crescimento: sondagens da sequencial x binária.
 * Binária: média medida procurando TODAS as chaves presentes, e pior caso
 * medido (chave ausente maior que todas). Sequencial: média medida com 101
 * chaves espaçadas uniformemente (procurar todas para n = 10^6 seriam ~5·10^11
 * passos) e pior caso = n. */
static void demo_tabela(void) {
    printf("=== 3) Sequencial x binária: sondagens para encontrar uma chave ===\n\n");
    const int maxn = 1000000;
    int *v = malloc((size_t)maxn * sizeof *v);
    if (v == NULL) { printf("   sem memória\n"); return; }
    for (int i = 0; i < maxn; i++) v[i] = 2 * i;           /* ordenado, só pares */

    printf("   %9s | %14s %12s | %12s %12s %8s | %9s\n", "n", "seq. média", "seq. pior", "bin. média",
           "bin. pior", "log2 n", "ganho");
    printf("   ----------+-----------------------------+---------------------------------+----------\n");
    for (int n = 10; n <= maxn; n *= 10) {
        long seq = 0, bin = 0, pior_bin = 0, pior_seq = 0;
        for (int k = 0; k <= 100; k++) {
            int idx = (int)((long)(n - 1) * k / 100);
            sequencial(v, n, v[idx], &seq);
        }
        for (int i = 0; i < n; i++) binaria_iterativa(v, n, v[i], &bin, 0);
        binaria_iterativa(v, n, 2 * n + 1, &pior_bin, 0); /* ausente, maior que todos */
        sequencial(v, n, -1, &pior_seq);                    /* ausente: varre tudo      */
        double media_seq = seq / 101.0, media_bin = (double)bin / n;
        printf("   %9d | %14.1f %12ld | %12.2f %12ld %8.1f | %8.0fx\n", n, media_seq, pior_seq,
               media_bin, pior_bin, log2(n), media_seq / media_bin);
    }
    free(v);
    printf("\n   Leitura: multiplicar n por 10 multiplica a sequencial por 10, mas soma só ~3,3\n");
    printf("   sondagens à binária (log2 10 ≈ 3,3). Com 1 milhão de peças: ~20 contra ~500 mil.\n\n");
    pausar();
}

/* --- Exemplo real 1: estoque de peças ordenado por código ----------------- */

typedef struct {
    int codigo;
    const char *descricao;
    int quantidade;
} Peca;

static const Peca estoque[] = {   /* ORDENADO por código: pré-condição da binária */
    {1020, "rolamento 6204-2RS", 37},   {1185, "correia em V A-42", 12},
    {1310, "contator 3RT 9A 24Vcc", 8}, {2044, "sensor indutivo M12 PNP", 25},
    {2210, "relé de segurança", 3},     {2950, "inversor 1,5 kW", 2},
    {3101, "fusível NH 63A", 40},       {3340, "botoeira de emergência", 15},
    {4012, "CLP CPU 1214C", 1},         {4500, "fonte 24Vcc 5A", 6},
    {5123, "cabo PROFINET 5m", 18},     {6000, "encoder incremental 1024ppr", 4},
};
static const int n_estoque = (int)(sizeof estoque / sizeof estoque[0]);

/* Comparador para bsearch: devolve <0, 0 ou >0 (sem subtração, que pode
 * estourar com valores extremos). */
static int compara_peca(const void *a, const void *b) {
    const Peca *x = a, *y = b;
    return (x->codigo > y->codigo) - (x->codigo < y->codigo);
}

static void demo_estoque(void) {
    printf("=== 4) Exemplo real: peça por código no estoque ordenado ===\n\n");
    int codigos[16];
    for (int i = 0; i < n_estoque; i++) codigos[i] = estoque[i].codigo;

    int pedido = 4012;
    long s = 0;
    printf("   Ordem de manutenção pede a peça %d:\n", pedido);
    int i = binaria_iterativa(codigos, n_estoque, pedido, &s, 1);
    if (i >= 0)
        printf("   -> %s, %d em estoque (%ld sondagens)\n\n", estoque[i].descricao,
               estoque[i].quantidade, s);
    pausar();

    /* A biblioteca padrão já tem pesquisa binária: bsearch (stdlib.h). */
    Peca chave = {2210, NULL, 0};
    const Peca *p = bsearch(&chave, estoque, (size_t)n_estoque, sizeof estoque[0], compara_peca);
    printf("   Com bsearch() da stdlib: código %d -> %s\n\n", chave.codigo,
           p ? p->descricao : "não encontrado");
    pausar();
}

/* --- Exemplo real 2: tabela de calibração de um NTC ----------------------- */

static void demo_calibracao(void) {
    printf("=== 5) Exemplo real: tabela de calibração (leitura ADC -> °C) ===\n\n");
    /* Pontos levantados em bancada, ORDENADOS pela leitura do ADC de 12 bits. */
    const double adc[]  = {310, 520, 805, 1160, 1580, 2040, 2510, 2950, 3330, 3640};
    const double temp[] = {120, 100,  80,   60,   45,   30,   18,    8,   -2,  -12};
    const int n = (int)(sizeof adc / sizeof adc[0]);

    printf("   ADC : ");
    for (int i = 0; i < n; i++) printf("%6.0f", adc[i]);
    printf("\n   °C  : ");
    for (int i = 0; i < n; i++) printf("%6.0f", temp[i]);
    printf("\n\n");

    const double leituras[] = {1900, 600, 3500, 200};
    for (int k = 0; k < 4; k++) {
        double x = leituras[k];
        int i = limite_inferior(adc, n, x);   /* primeiro ponto com adc >= x */
        if (i == 0 || i == n) {
            printf("   ADC = %4.0f: fora da faixa calibrada -> alarme de sensor (aberto/curto)\n", x);
            continue;
        }
        /* x está entre adc[i-1] e adc[i]: interpolação linear. */
        double frac = (x - adc[i - 1]) / (adc[i] - adc[i - 1]);
        double t = temp[i - 1] + frac * (temp[i] - temp[i - 1]);
        printf("   ADC = %4.0f: limite_inferior -> i = %d, intervalo [%4.0f, %4.0f] -> %.1f °C\n", x, i,
               adc[i - 1], adc[i], t);
    }
    printf("\n   Com 10 pontos a diferença é pequena, mas tabelas de linearização com 4096\n");
    printf("   pontos (uma por código do ADC) precisam de 12 sondagens em vez de ~2048.\n\n");
    pausar();
}

/* Exploração livre: o aluno digita códigos de peça; EOF encerra. */
static void exploracao_livre(void) {
    int codigos[16];
    for (int i = 0; i < n_estoque; i++) codigos[i] = estoque[i].codigo;
    printf("=== Exploração: digite um código de peça (ex.: 3101). Ctrl+D encerra ===\n");
    int c;
    while (printf("   código> "), fflush(stdout), scanf("%d", &c) == 1) {
        int resto;
        while ((resto = getchar()) != '\n' && resto != EOF) { }   /* descarta o ENTER */
        long s = 0;
        int i = binaria_iterativa(codigos, n_estoque, c, &s, 1);
        if (i >= 0) printf("   -> %s (%ld sondagens)\n", estoque[i].descricao, s);
        else        printf("   -> inexistente (%ld sondagens)\n", s);
    }
    printf("\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    printf("PESQUISA BINÁRIA\n\n");
    demo_janela();
    demo_tabela();
    demo_estoque();
    demo_calibracao();
    exploracao_livre();
    return 0;
}
