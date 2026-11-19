/*
 * ============================================================================
 * Arquivo : 07_topologica_partida_maquinas.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidade 6.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a ORDENAÇÃO TOPOLÓGICA pelo algoritmo de KAHN passo a passo: o grau de
 *   entrada de cada vértice, a fila dos que já podem partir e a ordem final.
 *   Mostra também as "etapas paralelas" (o que pode partir ao mesmo tempo), o
 *   que acontece quando há um CICLO de intertravamento e por que Kahn,
 *   O(V + E), é muito melhor que a varredura ingênua, O(V·(V + E)).
 *
 * EXEMPLO REAL
 *   Sequência de partida de uma linha de produção: o QGBT precisa estar
 *   energizado antes do compressor, a lubrificação antes da prensa, a esteira
 *   de SAÍDA antes da de ENTRADA (parte-se de jusante para montante para não
 *   acumular peças) e tudo antes de liberar a produção. O mesmo modelo serve
 *   para etapas de comissionamento, ordem de compilação (make) e disciplinas
 *   com pré-requisito.
 *
 * COMPLEXIDADE (V = vértices/etapas, E = arcos/dependências)
 *   Algoritmo                      Lista de adj.   Matriz     Espaço extra
 *   Kahn (fila + grau de entrada)  O(V + E)        O(V²)      O(V)
 *   varredura ingênua (abaixo)     O(V·(V + E))    O(V³)      O(V)
 *   Por quê: em Kahn cada vértice entra e sai da fila uma única vez (O(V)) e
 *   cada arco é usado uma vez para calcular o grau e uma vez para decrementar
 *   (O(E)). A varredura ingênua recalcula todos os graus a cada etapa escolhida:
 *   V rodadas de O(V + E).
 *
 * COMO EXECUTAR
 *   ./run 07_topologica_partida_maquinas               (ENTER avança cada passo)
 *   ./run 07_topologica_partida_maquinas < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
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

static void *aloca(size_t qtd, size_t tamanho) {
    void *p = calloc(qtd, tamanho);
    if (p == NULL) {
        fprintf(stderr, "Falha de alocação\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/* ==========================================================================
 * GRAFO DIRIGIDO (lista de adjacência) + lista de arcos
 * ==========================================================================
 * Guardamos também os arcos num vetor simples: a varredura ingênua precisa
 * percorrer "todos os arcos" a cada rodada, e o vetor deixa isso explícito.
 */
typedef struct No {
    int vertice;
    struct No *prox;
} No;

typedef struct {
    int n, e, cap_e;
    No **inicio, **fim;
    int (*arco)[2];   /* arco[k] = {u, v}: u antes de v */
} Grafo;

static Grafo cria(int n, int cap_e) {
    Grafo g = { n, 0, cap_e, aloca((size_t)n, sizeof(No *)), aloca((size_t)n, sizeof(No *)),
                aloca((size_t)cap_e, sizeof(int[2])) };
    return g;
}

/* O(1): u -> v ("u precisa estar pronto antes de v"). */
static void depende(Grafo *g, int u, int v) {
    No *novo = aloca(1, sizeof *novo);
    novo->vertice = v;
    if (g->fim[u] == NULL) g->inicio[u] = novo;
    else g->fim[u]->prox = novo;
    g->fim[u] = novo;
    g->arco[g->e][0] = u;
    g->arco[g->e][1] = v;
    g->e++;
}

static void libera(Grafo *g) {
    for (int v = 0; v < g->n; v++) {
        while (g->inicio[v] != NULL) {
            No *prox = g->inicio[v]->prox;
            free(g->inicio[v]);
            g->inicio[v] = prox;
        }
    }
    free(g->inicio);
    free(g->fim);
    free(g->arco);
}

/* ==========================================================================
 * A LINHA DE PRODUÇÃO
 * ==========================================================================
 */
enum { QGBT, COMPRESSOR, CHILLER, LUBRIF, CLP, EST_ENT, PRENSA, ROBO, EST_SAI, PRODUCAO, NV };
static const char *const nome[NV] = {
    "Energizar QGBT", "Compressor de ar", "Chiller", "Bomba de lubrificação", "CLP e rede",
    "Esteira de entrada", "Prensa hidráulica", "Robô de solda", "Esteira de saída",
    "Liberar produção"
};
static const char *const sigla[NV] = {
    "QGBT", "AR", "CHIL", "LUB", "CLP", "EST-E", "PRENS", "ROBO", "EST-S", "PROD"
};

static void monta_linha(Grafo *g) {
    int dep[][2] = {
        {QGBT, COMPRESSOR}, {QGBT, CHILLER}, {QGBT, LUBRIF}, {QGBT, CLP},
        {COMPRESSOR, ROBO},                 /* garra pneumática precisa de ar        */
        {CHILLER, PRENSA}, {LUBRIF, PRENSA}, /* óleo resfriado e guias lubrificadas   */
        {CLP, EST_ENT}, {CLP, PRENSA}, {CLP, ROBO}, {CLP, EST_SAI},
        {EST_SAI, EST_ENT},                 /* parte de jusante para montante         */
        {EST_ENT, PRODUCAO}, {PRENSA, PRODUCAO}, {ROBO, PRODUCAO},
    };
    for (size_t k = 0; k < sizeof dep / sizeof dep[0]; k++) depende(g, dep[k][0], dep[k][1]);
}

static void lista_dependencias(const Grafo *g) {
    printf("  Dependências (u -> v: u precisa estar pronto antes de v):\n");
    for (int k = 0; k < g->e; k++) {
        printf("    %-6s -> %-6s  (%s antes de %s)\n", sigla[g->arco[k][0]], sigla[g->arco[k][1]],
               nome[g->arco[k][0]], nome[g->arco[k][1]]);
    }
    printf("\n");
}

static void desenha_estado(const int grau[], const int feito[], const int fila[], int ini, int fim) {
    printf("      %-11s", "etapa");
    for (int v = 0; v < NV; v++) printf("%7s", sigla[v]);
    printf("\n      %-11s", "grau entr.");
    for (int v = 0; v < NV; v++) {
        if (feito[v]) printf("%7s", "ok");
        else printf("%7d", grau[v]);
    }
    printf("\n      fila: frente -> [");
    if (ini == fim) printf(" vazia ");
    for (int i = ini; i < fim; i++) printf(" %s%s", sigla[fila[i]], i + 1 < fim ? " |" : " ");
    printf("] <- fim\n");
}

/* ==========================================================================
 * KAHN
 * ==========================================================================
 * 1. grau[v] = número de pré-requisitos de v ainda não cumpridos;  O(V + E)
 * 2. enfileira todos com grau 0 (podem partir já);                  O(V)
 * 3. retira u da fila, põe na ordem e "cumpre" u: decrementa o grau
 *    de cada v que dependia de u; quem chegar a 0 entra na fila.     O(V + E)
 * Se a ordem terminar com menos de V vértices, sobrou um CICLO: os que
 * restaram nunca chegam a grau 0, porque um espera pelo outro.
 * Retorna quantos vértices foram ordenados; nivel[v] = etapa paralela de v.
 */
static int kahn(const Grafo *g, int ordem[], int nivel[], int narrar, long *ops) {
    int *grau = aloca((size_t)g->n, sizeof(int));
    int *feito = aloca((size_t)g->n, sizeof(int));
    int *fila = aloca((size_t)g->n, sizeof(int));
    int ini = 0, fim = 0, qtd = 0;
    *ops = 0;

    for (int k = 0; k < g->e; k++) { grau[g->arco[k][1]]++; (*ops)++; }
    for (int v = 0; v < g->n; v++) {
        (*ops)++;
        nivel[v] = 0;
        if (grau[v] == 0) fila[fim++] = v;
    }
    if (narrar) {
        printf("--- Início: graus de entrada calculados; quem tem grau 0 vai para a fila ---\n");
        desenha_estado(grau, feito, fila, ini, fim);
        pausar();
    }

    while (ini < fim) {
        int u = fila[ini++];
        (*ops)++;
        feito[u] = 1;
        ordem[qtd++] = u;
        if (narrar) printf("\n--- Passo %d: parte %s (etapa paralela %d) ---\n", qtd, nome[u], nivel[u]);
        for (const No *a = g->inicio[u]; a != NULL; a = a->prox) {
            int v = a->vertice;
            (*ops)++;
            grau[v]--;
            if (nivel[u] + 1 > nivel[v]) nivel[v] = nivel[u] + 1;   /* depois do mais lento */
            if (narrar) {
                printf("      %s cumprido: grau de %s cai para %d", sigla[u], sigla[v], grau[v]);
                printf("%s\n", grau[v] == 0 ? " -> todos os pré-requisitos ok, enfileira" : "");
            }
            if (grau[v] == 0) fila[fim++] = v;
        }
        if (narrar) {
            if (g->inicio[u] == NULL) printf("      ninguém depende de %s\n", sigla[u]);
            desenha_estado(grau, feito, fila, ini, fim);
            pausar();
        }
    }
    free(grau);
    free(feito);
    free(fila);
    return qtd;
}

static void parte1_kahn(void) {
    Grafo g = cria(NV, 32);
    monta_linha(&g);
    printf("=== PARTE 1: ordem de partida da linha (algoritmo de Kahn) ===\n\n");
    lista_dependencias(&g);
    pausar();

    int ordem[NV], nivel[NV];
    long ops;
    int qtd = kahn(&g, ordem, nivel, 1, &ops);

    printf("\n=== Resultado ===\n\n  Ordem de partida (%d de %d etapas):\n", qtd, NV);
    for (int i = 0; i < qtd; i++) printf("    %2d. %s\n", i + 1, nome[ordem[i]]);
    printf("\n  Etapas paralelas (podem ser disparadas juntas pelo CLP):\n");
    for (int etapa = 0; etapa < NV; etapa++) {
        int algum = 0;
        for (int v = 0; v < NV; v++) {
            if (nivel[v] != etapa) continue;
            if (!algum) printf("    etapa %d:", etapa);
            printf(" %s", sigla[v]);
            algum = 1;
        }
        if (algum) printf("\n");
    }
    printf("\n  Operações contadas: %ld  (V + E para os graus + V saídas da fila + E\n", ops);
    printf("  decrementos = %d + %d + %d + %d). A ordem não é única: com outra ordem\n",
           NV, g.e, NV, g.e);
    printf("  na fila (ou com pilha) sairia outra sequência igualmente válida.\n\n");
    libera(&g);
    pausar();
}

/* ==========================================================================
 * PARTE 2 — UM INTERTRAVAMENTO ERRADO CRIA UM CICLO
 * ==========================================================================
 */
static void parte2_ciclo(void) {
    Grafo g = cria(NV, 32);
    monta_linha(&g);
    depende(&g, ROBO, COMPRESSOR);   /* ERRO: "compressor só parte com o robô pronto" */
    printf("=== PARTE 2: acrescentando o intertravamento errado ROBO -> AR ===\n");
    printf("  (AR -> ROBO já existia: o robô espera o ar e o ar espera o robô)\n\n");
    pausar();

    int ordem[NV], nivel[NV];
    long ops;
    int qtd = kahn(&g, ordem, nivel, 0, &ops);
    int feito[NV] = { 0 };
    for (int i = 0; i < qtd; i++) feito[ordem[i]] = 1;

    printf("  Kahn ordenou %d de %d etapas:", qtd, NV);
    for (int i = 0; i < qtd; i++) printf(" %s", sigla[ordem[i]]);
    printf("\n  Nunca chegaram a grau 0:");
    for (int v = 0; v < NV; v++) if (!feito[v]) printf(" %s", sigla[v]);
    printf("\n  AR e ROBO formam o ciclo; PROD depende de ROBO e também fica travada.\n");
    printf("  Detectar 'ordem com menos de V vértices' é o teste de ciclo de graça do Kahn.\n\n");
    libera(&g);
    pausar();
}

/* ==========================================================================
 * PARTE 3 — KAHN x VARREDURA INGÊNUA
 * ==========================================================================
 * Ingênua (o que se faria sem guardar os graus): a cada rodada, recalcula
 * os pré-requisitos pendentes de TODOS os vértices percorrendo todos os
 * arcos (O(E)) e escolhe o primeiro vértice pronto (O(V)). São V rodadas.
 */
static long varredura_ingenua(const Grafo *g) {
    int *feito = aloca((size_t)g->n, sizeof(int));
    int *pend = aloca((size_t)g->n, sizeof(int));
    long ops = 0;
    for (int rodada = 0; rodada < g->n; rodada++) {
        for (int v = 0; v < g->n; v++) { pend[v] = 0; ops++; }
        for (int k = 0; k < g->e; k++) {
            ops++;
            if (!feito[g->arco[k][0]]) pend[g->arco[k][1]]++;
        }
        int escolhido = -1;
        for (int v = 0; v < g->n && escolhido < 0; v++) {
            ops++;
            if (!feito[v] && pend[v] == 0) escolhido = v;
        }
        if (escolhido < 0) break;   /* ciclo: ninguém pronto */
        feito[escolhido] = 1;
    }
    free(feito);
    free(pend);
    return ops;
}

static unsigned long semente = 7;
static unsigned long sorteio(void) {
    semente = semente * 1103515245UL + 12345UL;
    return (semente >> 16) & 0x7fffUL;
}

/* DAG gerado: cada etapa v > 0 depende de até 2 etapas anteriores próximas. */
static void gera_dag(Grafo *g) {
    for (int v = 1; v < g->n; v++) {
        int janela = v < 20 ? v : 20;
        int p1 = v - 1 - (int)(sorteio() % (unsigned long)janela);
        int p2 = v - 1 - (int)(sorteio() % (unsigned long)janela);
        depende(g, p1, v);
        if (p2 != p1) depende(g, p2, v);
    }
}

static void parte3_crescimento(void) {
    const int tamanhos[] = { 10, 100, 1000, 10000 };
    printf("=== PARTE 3: Kahn O(V + E) x varredura ingênua O(V·(V + E)) ===\n\n");
    printf("  %7s %7s | %14s %17s | %s\n", "V", "E", "Kahn (ops)", "ingênua (ops)", "razão");  /* ê = 2 bytes */
    printf("  ----------------------------------------------------------------\n");
    for (int i = 0; i < 4; i++) {
        int n = tamanhos[i];
        Grafo g = cria(n, 2 * n);
        gera_dag(&g);
        int *ordem = aloca((size_t)n, sizeof(int));
        int *nivel = aloca((size_t)n, sizeof(int));
        long k_ops;
        kahn(&g, ordem, nivel, 0, &k_ops);
        long i_ops = varredura_ingenua(&g);
        printf("  %7d %7d | %14ld %16ld | %6.0fx\n", n, g.e, k_ops, i_ops, (double)i_ops / (double)k_ops);
        free(ordem);
        free(nivel);
        libera(&g);
    }
    printf("\n  10x mais etapas: Kahn faz ~10x mais operações; a ingênua, ~100x.\n");
    printf("  Guardar o grau de entrada e atualizar só os vizinhos é o que faz a diferença.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    parte1_kahn();
    parte2_ciclo();
    parte3_crescimento();
    return 0;
}
