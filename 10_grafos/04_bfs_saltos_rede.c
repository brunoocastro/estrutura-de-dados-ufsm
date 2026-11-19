/*
 * ============================================================================
 * Arquivo : 04_bfs_saltos_rede.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidade 6.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a BUSCA EM LARGURA (BFS) acontecendo: a cada passo o programa desenha
 *   a FILA (frente/fim), o vetor de VISITADOS, a distância em saltos e o pai
 *   de cada vértice. No fim, reconstrói o caminho com MENOS SALTOS do servidor
 *   SCADA até cada equipamento e mostra as "camadas" da busca.
 *
 * EXEMPLO REAL
 *   Rede Ethernet industrial de uma planta: servidor SCADA, switch de núcleo,
 *   switches de linha (com enlaces redundantes formando ciclos), CLPs, IHM,
 *   inversor e remota de I/O. Cada switch atravessado soma atraso e é um
 *   ponto de falha; o número mínimo de saltos é o que a BFS calcula. O mesmo
 *   código serve para "graus de separação" numa rede social (06_rede_social.c).
 *
 * COMPLEXIDADE (V = vértices, E = arestas)
 *   Representação        Tempo da BFS   Espaço extra
 *   lista de adjacência  O(V + E)       O(V)  (fila + visitados + dist + pai)
 *   matriz de adjacência O(V²)          O(V)
 *   Por quê: cada vértice entra na fila no máximo UMA vez (O(V)) e, quando
 *   sai, sua lista de vizinhos é percorrida uma vez; somando todas as listas
 *   dá 2E no grafo não dirigido (O(E)). Na matriz, achar os vizinhos de cada
 *   vértice exige varrer uma linha inteira de V células: V·V.
 *
 * COMO EXECUTAR
 *   ./run 04_bfs_saltos_rede               (ENTER avança cada passo)
 *   ./run 04_bfs_saltos_rede < /dev/null   (executa direto, sem pausas)
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
 * GRAFO NÃO DIRIGIDO COM LISTA DE ADJACÊNCIA
 * ==========================================================================
 * Inserimos no FIM de cada lista (guardando um ponteiro para o último nó)
 * para que os vizinhos sejam visitados na ordem em que os cabos foram
 * declarados. Isso deixa a narração previsível; a BFS funciona em qualquer
 * ordem, só muda qual caminho é escolhido quando há empate de saltos.
 */
typedef struct No {
    int vertice;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **inicio;   /* inicio[v] = primeiro vizinho de v */
    No **fim;      /* fim[v]    = último vizinho (inserção em O(1)) */
} Grafo;

static Grafo cria(int n) {
    Grafo g = { n, aloca((size_t)n, sizeof(No *)), aloca((size_t)n, sizeof(No *)) };
    return g;
}

/* O(1): acrescenta v no fim da lista de u. */
static void anexa(Grafo *g, int u, int v) {
    No *novo = aloca(1, sizeof *novo);
    novo->vertice = v;
    if (g->fim[u] == NULL) g->inicio[u] = novo;
    else g->fim[u]->prox = novo;
    g->fim[u] = novo;
}

/* Cabo = aresta não dirigida: guardada nas duas listas. */
static void cabo(Grafo *g, int u, int v) {
    anexa(g, u, v);
    anexa(g, v, u);
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
}

/* ==========================================================================
 * A REDE DA PLANTA
 * ==========================================================================
 */
enum { SCADA, CORE, SW_A, SW_B, SW_C, CLP_A, CLP_B, IHM_A, INV_B, RIO_C, NV };
static const char *nome[NV] = {
    "SCADA", "SW-CORE", "SW-A", "SW-B", "SW-C", "CLP-A", "CLP-B", "IHM-A", "INV-B", "RIO-C"
};

static void desenha_rede(void) {
    printf("                         SCADA\n");
    printf("                           |\n");
    printf("                        SW-CORE\n");
    printf("                     /     |     \\\n");
    printf("                 SW-A --- SW-B --- SW-C     (SW-A—SW-B e SW-B—SW-C: enlaces\n");
    printf("                 /  \\       |        |       redundantes -> o grafo tem ciclos)\n");
    printf("             CLP-A--IHM-A  CLP-B    RIO-C\n");
    printf("                            |        |\n");
    printf("                          INV-B -----+\n\n");
}

static void monta_rede(Grafo *g) {
    int cabos[][2] = {
        {SCADA, CORE}, {CORE, SW_A}, {CORE, SW_B}, {CORE, SW_C}, {SW_A, SW_B}, {SW_B, SW_C},
        {SW_A, CLP_A}, {SW_A, IHM_A}, {CLP_A, IHM_A}, {SW_B, CLP_B}, {CLP_B, INV_B},
        {SW_C, RIO_C}, {RIO_C, INV_B},
    };
    for (size_t k = 0; k < sizeof cabos / sizeof cabos[0]; k++) cabo(g, cabos[k][0], cabos[k][1]);
}

/* ==========================================================================
 * DESENHO DO ESTADO DA BFS
 * ==========================================================================
 */
static void desenha_fila(const int fila[], int ini, int fim) {
    printf("      fila: frente -> [");
    if (ini == fim) printf(" vazia ");
    for (int i = ini; i < fim; i++) printf(" %s%s", nome[fila[i]], i + 1 < fim ? " |" : " ");
    printf("] <- fim\n");
}

/* Tabela com uma coluna por vértice: visitado, dist (saltos) e pai. */
static void desenha_tabela(const int dist[], const int pai[]) {
    printf("      %-7s", "vértice");
    for (int v = 0; v < NV; v++) printf("%8s", nome[v]);
    printf("\n      %-7s", "visit.");
    for (int v = 0; v < NV; v++) printf("%8s", dist[v] >= 0 ? "X" : ".");
    printf("\n      %-7s", "saltos");
    for (int v = 0; v < NV; v++) {
        if (dist[v] >= 0) printf("%8d", dist[v]);
        else printf("%10s", "∞");   /* "∞" ocupa 3 bytes e 1 coluna: largura 8 + 2 */
    }
    printf("\n      %-7s", "pai");
    for (int v = 0; v < NV; v++) printf("%8s", pai[v] >= 0 ? nome[pai[v]] : "-");
    printf("\n");
}

/* ==========================================================================
 * BFS
 * ==========================================================================
 * Pré : dist[] e pai[] têm g->n posições; origem é válida.
 * Pós : dist[v] = menor número de arestas origem->v (-1 se inalcançável);
 *       pai[v]  = vértice anterior nesse caminho (-1 na origem).
 * Contadores: *saidas = vértices desenfileirados, *arestas = vizinhos examinados.
 *
 * Por que marcar como visitado ao ENFILEIRAR (e não ao desenfileirar)?
 *   Se marcássemos só na saída, o mesmo vértice poderia entrar várias vezes na
 *   fila (uma por vizinho já processado), a fila passaria de V posições e a
 *   contagem deixaria de ser O(V + E). Armadilha clássica de prova.
 */
static void bfs(const Grafo *g, int origem, int dist[], int pai[], int narrar,
                long *saidas, long *arestas) {
    int *fila = aloca((size_t)g->n, sizeof(int));   /* cabe: cada vértice entra 1 vez */
    int ini = 0, fim = 0;
    *saidas = *arestas = 0;
    for (int v = 0; v < g->n; v++) { dist[v] = -1; pai[v] = -1; }

    dist[origem] = 0;
    fila[fim++] = origem;
    if (narrar) {
        printf("--- Início: origem %s marcada (0 saltos) e enfileirada ---\n", nome[origem]);
        desenha_fila(fila, ini, fim);
        desenha_tabela(dist, pai);
        pausar();
    }

    int passo = 0;
    while (ini < fim) {
        int u = fila[ini++];                              /* FIFO: sai o mais antigo */
        (*saidas)++;
        if (narrar) printf("\n--- Passo %d: desenfileira %s (%d salto(s) da origem) ---\n",
                           ++passo, nome[u], dist[u]);
        for (const No *a = g->inicio[u]; a != NULL; a = a->prox) {
            int v = a->vertice;
            (*arestas)++;
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                pai[v] = u;
                fila[fim++] = v;
                if (narrar) printf("      vizinho %-7s: NOVO -> saltos = %d, pai = %s, enfileira\n",
                                   nome[v], dist[v], nome[u]);
            } else if (narrar) {
                printf("      vizinho %-7s: já visitado (saltos %d) -> ignora\n", nome[v], dist[v]);
            }
        }
        if (narrar) {
            desenha_fila(fila, ini, fim);
            desenha_tabela(dist, pai);
            pausar();
        }
    }
    free(fila);
}

/* O(V): segue pai[] do destino até a origem e imprime na ordem certa. */
static void imprime_rota(const int pai[], const int dist[], int destino) {
    int rota[NV], k = 0;
    for (int v = destino; v != -1; v = pai[v]) rota[k++] = v;
    printf("    %-7s %d salto(s): ", nome[destino], dist[destino]);
    while (k > 0) {
        k--;
        printf("%s%s", nome[rota[k]], k > 0 ? " -> " : "\n");
    }
}

static void demonstra_planta(void) {
    Grafo g = cria(NV);
    monta_rede(&g);
    int dist[NV], pai[NV];
    long saidas, arestas;

    printf("=== BFS a partir do servidor SCADA: quantos switches até cada equipamento? ===\n\n");
    desenha_rede();
    pausar();
    bfs(&g, SCADA, dist, pai, 1, &saidas, &arestas);

    printf("\n=== Resultado ===\n\n  Camadas (a BFS anda como uma onda, nível por nível):\n");
    for (int nivel = 0; nivel < NV; nivel++) {
        int algum = 0;
        for (int v = 0; v < NV; v++) {
            if (dist[v] == nivel) {
                if (!algum) printf("    nível %d:", nivel);
                printf(" %s", nome[v]);
                algum = 1;
            }
        }
        if (algum) printf("\n");
    }
    printf("\n  Caminho com menos saltos (seguindo pai[] de trás para frente):\n");
    for (int v = 0; v < NV; v++) imprime_rota(pai, dist, v);

    printf("\n  Contadores: %ld vértices desenfileirados (= V = %d), %ld vizinhos\n",
           saidas, NV, arestas);
    printf("  examinados (= 2E = 2·13, cada cabo visto pelos dois lados). Total O(V + E).\n");
    printf("  Repare em INV-B: há dois caminhos de 4 saltos (via CLP-B ou via RIO-C);\n");
    printf("  a BFS fica com o primeiro que descobre. Saltos não são latência: para somar\n");
    printf("  atrasos diferentes em cada enlace use Dijkstra (08_dijkstra_latencia_rede.c).\n\n");
    libera(&g);
    pausar();
}

/* ==========================================================================
 * TABELA DE CRESCIMENTO
 * ==========================================================================
 * Rede gerada com V switches: anel (i, i+1) mais um enlace redundante
 * (i, i+V/3) -> cada switch tem ~4 vizinhos, E ~ 2V (grafo esparso).
 */
static void tabela_crescimento(void) {
    const int tamanhos[] = { 10, 100, 1000, 10000 };
    printf("=== Crescimento: BFS em redes geradas com V switches (~4 enlaces cada) ===\n\n");
    printf("  %7s %7s | %12s %12s | %14s | %s\n", "V", "E", "desenfil.", "vizinhos",
           "lista: total", "matriz: V·V");
    printf("  ---------------------------------------------------------------------------\n");
    for (int i = 0; i < 4; i++) {
        int n = tamanhos[i];
        Grafo g = cria(n);
        long e = 0;
        for (int v = 0; v < n; v++) {
            cabo(&g, v, (v + 1) % n);
            cabo(&g, v, (v + n / 3) % n);
            e += 2;
        }
        int *dist = aloca((size_t)n, sizeof(int));
        int *pai = aloca((size_t)n, sizeof(int));
        long saidas, arestas;
        bfs(&g, 0, dist, pai, 0, &saidas, &arestas);
        printf("  %7d %7ld | %12ld %12ld | %14ld | %ld\n", n, e, saidas, arestas,
               saidas + arestas, (long)n * n);
        free(dist);
        free(pai);
        libera(&g);
    }
    printf("\n  Com lista o total cresce como V + 2E (10x mais switches = 10x mais trabalho).\n");
    printf("  Com matriz cada vértice desenfileirado varre uma linha de V células: V·V\n");
    printf("  (10x mais switches = 100x mais trabalho). Com V = 10000: 50 mil contra 100 milhões.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    demonstra_planta();
    tabela_crescimento();
    return 0;
}
