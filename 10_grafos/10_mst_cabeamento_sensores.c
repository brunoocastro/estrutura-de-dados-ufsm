/*
 * ============================================================================
 * Arquivo : 10_mst_cabeamento_sensores.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidade 6.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a ÁRVORE GERADORA MÍNIMA (MST) sendo construída por dois algoritmos:
 *     1. KRUSKAL: arestas em ordem crescente de custo; aceita a aresta se ela
 *        une duas componentes diferentes (Union-Find), rejeita se fecharia
 *        um ciclo. As componentes são desenhadas a cada passo;
 *     2. PRIM: cresce uma única árvore a partir do gateway, sempre pelo cabo
 *        mais barato que sai dela (tabela custo/pred a cada iteração);
 *     3. rede desconexa: Kruskal devolve uma FLORESTA, uma árvore por
 *        componente conexo (Union-Find também conta componentes).
 *
 * EXEMPLO REAL
 *   Cabeamento de uma rede de sensores num galpão: cada sensor precisa chegar
 *   ao gateway, e os trajetos possíveis seguem as eletrocalhas, com o
 *   comprimento em metros como custo. A MST liga tudo com o MÍNIMO de cabo.
 *   O mesmo problema aparece no projeto de redes elétricas, de tubulações e de
 *   fibra óptica. Cuidado: a MST não tem redundância (um cabo rompido isola
 *   parte da rede); redes críticas usam anel (09_automacao_agv_profinet.c).
 *
 * COMPLEXIDADE (V = vértices, E = arestas)
 *   Algoritmo           Tempo                         Espaço extra
 *   Kruskal             O(E log E) ordenar + O(E·α(V)) O(V + E)
 *   Prim com vetor      O(V²)          (bom em denso)  O(V)
 *   Prim com heap       O(E log V)     (bom em esparso) O(V + E)
 *   Por quê: Kruskal é dominado pela ordenação das arestas; cada teste "estão
 *   na mesma componente?" é um find do Union-Find, praticamente O(1) (α(V) <= 4
 *   para qualquer V real) com compressão de caminho e união por tamanho.
 *   Prim com vetor faz V iterações e em cada uma varre os V vértices em busca
 *   do menor custo (V·V), exatamente como Dijkstra com vetor.
 *
 * COMO EXECUTAR
 *   ./run 10_mst_cabeamento_sensores               (ENTER avança cada passo)
 *   ./run 10_mst_cabeamento_sensores < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INF INT_MAX

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ==========================================================================
 * O GALPÃO: GATEWAY + SENSORES E OS TRAJETOS POSSÍVEIS DE CABO
 * ==========================================================================
 * Aqui usamos LISTA DE ARESTAS (vetor de {u, v, custo}), a representação
 * natural de Kruskal, e uma MATRIZ de custos para Prim com vetor (O(V²) já é
 * o custo do algoritmo, então a matriz não piora nada). Ver 03_formas_grafo.c.
 */
#define MAX_V 10
enum { GW, S1, S2, S3, S4, S5, S6, S7, S8, S9 };
static const char *const nome[MAX_V] = { "GW", "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9" };

typedef struct {
    int u, v, custo;
    int ordem;   /* posição na declaração: desempate estável na ordenação */
} Aresta;

static const Aresta trajetos[] = {
    {GW, S1, 12, 0}, {GW, S2, 20, 1}, {GW, S3, 25, 2}, {S1, S2, 9, 3}, {S1, S3, 10, 4},
    {S1, S4, 18, 5}, {S2, S3, 7, 6}, {S2, S5, 15, 7}, {S3, S6, 22, 8}, {S4, S5, 11, 9},
    {S4, S6, 13, 10}, {S5, S6, 10, 11}, {S5, S7, 16, 12}, {S6, S7, 8, 13}, {S4, S7, 30, 14},
};
#define N_TRAJETOS ((int)(sizeof trajetos / sizeof trajetos[0]))

/* Lista os trajetos candidatos em 3 colunas (um desenho com 15 cabos ficaria ilegível). */
static void desenha_galpao(void) {
    printf("  Gateway GW e sensores S1..S7. Trajetos possíveis pelas eletrocalhas (m):\n");
    for (int k = 0; k < N_TRAJETOS; k++) {
        printf("%s%-3s — %-3s %3d m", k % 3 == 0 ? "    " : "      ", nome[trajetos[k].u],
               nome[trajetos[k].v], trajetos[k].custo);
        if (k % 3 == 2 || k == N_TRAJETOS - 1) printf("\n");
    }
    printf("  O grafo tem %d arestas para 8 vértices; a árvore precisa de só 7.\n\n", N_TRAJETOS);
}

/* ==========================================================================
 * UNION-FIND (conjuntos disjuntos)
 * ==========================================================================
 * pai[x] aponta para outro elemento do mesmo conjunto; a raiz (pai[x] == x)
 * é o "representante". Duas otimizações deixam cada operação quase O(1):
 *   - compressão de caminho: o find pendura todos no representante;
 *   - união por tamanho: a árvore menor é pendurada na maior.
 * Sem elas, uniões em sequência formam uma "lista" e o find vira O(V).
 */
typedef struct {
    int pai[MAX_V], tam[MAX_V], n;
    long finds;
} UnionFind;

static void uf_inicia(UnionFind *uf, int n) {
    uf->n = n;
    uf->finds = 0;
    for (int i = 0; i < n; i++) { uf->pai[i] = i; uf->tam[i] = 1; }
}

static int uf_find(UnionFind *uf, int x) {
    uf->finds++;
    if (uf->pai[x] != x) uf->pai[x] = uf_find(uf, uf->pai[x]);   /* compressão */
    return uf->pai[x];
}

/* Retorna 0 se a e b já estavam juntos (a aresta fecharia um ciclo). */
static int uf_une(UnionFind *uf, int a, int b) {
    int ra = uf_find(uf, a), rb = uf_find(uf, b);
    if (ra == rb) return 0;
    if (uf->tam[ra] < uf->tam[rb]) { int t = ra; ra = rb; rb = t; }
    uf->pai[rb] = ra;
    uf->tam[ra] += uf->tam[rb];
    return 1;
}

/* O(V²) só para desenhar: agrupa os vértices pelo representante. */
static int desenha_componentes(UnionFind *uf) {
    int qtd = 0;
    long salva = uf->finds;   /* os finds do desenho não entram na contagem */
    printf("      componentes:");
    for (int r = 0; r < uf->n; r++) {
        if (uf_find(uf, r) != r) continue;
        qtd++;
        printf(" {");
        int primeiro = 1;
        for (int v = 0; v < uf->n; v++) {
            if (uf_find(uf, v) == r) { printf(primeiro ? "%s" : " %s", nome[v]); primeiro = 0; }
        }
        printf("}");
    }
    printf("   (%d)\n", qtd);
    uf->finds = salva;
    return qtd;
}

/* ==========================================================================
 * KRUSKAL
 * ==========================================================================
 */
static long comparacoes_ordenacao = 0;

static int compara_custo(const void *a, const void *b) {
    const Aresta *x = a, *y = b;
    comparacoes_ordenacao++;
    if (x->custo != y->custo) return x->custo < y->custo ? -1 : 1;
    return x->ordem - y->ordem;   /* desempate: deixa a saída igual em toda máquina */
}

/* Retorna o custo total; aceitas[] recebe as arestas da floresta. */
static int kruskal(const Aresta *candidatas, int m, int n, Aresta aceitas[], int *qtd_aceitas,
                   int narrar, long *finds) {
    Aresta ordenadas[32];
    for (int k = 0; k < m; k++) ordenadas[k] = candidatas[k];
    comparacoes_ordenacao = 0;
    qsort(ordenadas, (size_t)m, sizeof ordenadas[0], compara_custo);   /* O(E log E) */

    if (narrar) {
        printf("  Arestas ordenadas por custo (%ld comparações no qsort):\n   ", comparacoes_ordenacao);
        for (int k = 0; k < m; k++) {
            printf(" %s—%s:%d", nome[ordenadas[k].u], nome[ordenadas[k].v], ordenadas[k].custo);
            if (k % 8 == 7 && k + 1 < m) printf("\n   ");   /* quebra para caber em ~100 colunas */
        }
        printf("\n\n");
    }

    UnionFind uf;
    uf_inicia(&uf, n);
    if (narrar) { printf("  Início: cada sensor é uma componente sozinho.\n"); desenha_componentes(&uf); pausar(); }

    int total = 0;
    *qtd_aceitas = 0;
    for (int k = 0; k < m && *qtd_aceitas < n - 1; k++) {
        const Aresta *a = &ordenadas[k];
        int ok = uf_une(&uf, a->u, a->v);
        if (narrar) {
            printf("\n--- Passo %d: %s—%s (%d m) ---\n", k + 1, nome[a->u], nome[a->v], a->custo);
            if (ok) printf("      %s e %s estavam em componentes diferentes -> ACEITA, une\n",
                           nome[a->u], nome[a->v]);
            else printf("      %s e %s já estão na mesma componente -> REJEITA (fecharia ciclo)\n",
                        nome[a->u], nome[a->v]);
        }
        if (ok) {
            aceitas[(*qtd_aceitas)++] = *a;
            total += a->custo;
        }
        if (narrar) {
            desenha_componentes(&uf);
            printf("      cabo acumulado: %d m em %d trecho(s)\n", total, *qtd_aceitas);
            pausar();
        }
    }
    *finds = uf.finds;
    return total;
}

static void parte1_kruskal(void) {
    const int n = 8;   /* GW..S7 */
    printf("=== PARTE 1: KRUSKAL — cabos mais curtos primeiro, sem fechar ciclos ===\n\n");
    desenha_galpao();
    pausar();

    Aresta aceitas[MAX_V];
    int qtd;
    long finds;
    int total = kruskal(trajetos, N_TRAJETOS, n, aceitas, &qtd, 1, &finds);

    int todos = 0;
    for (int k = 0; k < N_TRAJETOS; k++) todos += trajetos[k].custo;
    printf("\n  MST: %d trechos (= V - 1 = %d):", qtd, n - 1);
    for (int k = 0; k < qtd; k++) printf(" %s—%s", nome[aceitas[k].u], nome[aceitas[k].v]);
    printf("\n  Total de cabo: %d m. Usar todos os %d trajetos custaria %d m.\n", total, N_TRAJETOS, todos);
    printf("  Assim que %d trechos foram aceitos o laço parou: as arestas mais caras nem\n", n - 1);
    printf("  precisaram ser examinadas. Finds no Union-Find: %ld.\n\n", finds);
    pausar();
}

/* ==========================================================================
 * PRIM COM VETOR
 * ==========================================================================
 * Parecido com Dijkstra, com UMA diferença: custo[v] é o peso do cabo mais
 * barato que liga v à árvore, e não a soma desde a origem. Armadilha comum:
 * escrever custo[u] + peso (isso é Dijkstra e dá outra árvore).
 */
static void desenha_prim(int n, const int custo[], const int pred[], const int na_arvore[], int escolhido) {
    printf("      sensor  custo  pred  estado\n");
    for (int v = 0; v < n; v++) {
        printf("      %-6s  ", nome[v]);
        if (custo[v] == INF) printf("%4s∞  ", "");   /* "∞": 3 bytes, 1 coluna */
        else printf("%5d  ", custo[v]);
        printf("%-4s  %s\n", pred[v] >= 0 ? nome[pred[v]] : "-",
               v == escolhido ? "ENTRA NA ÁRVORE  <==" : na_arvore[v] ? "na árvore" : "");
    }
}

static void parte2_prim(void) {
    const int n = 8;
    int peso[MAX_V][MAX_V] = { { 0 } };   /* 0 = sem trajeto */
    for (int k = 0; k < N_TRAJETOS; k++) {
        peso[trajetos[k].u][trajetos[k].v] = trajetos[k].custo;
        peso[trajetos[k].v][trajetos[k].u] = trajetos[k].custo;
    }
    int custo[MAX_V], pred[MAX_V], na_arvore[MAX_V] = { 0 };
    for (int v = 0; v < n; v++) { custo[v] = INF; pred[v] = -1; }
    custo[GW] = 0;
    long comparacoes = 0;
    int total = 0;

    printf("=== PARTE 2: PRIM — a árvore cresce a partir do gateway ===\n");
    for (int it = 1; it <= n; it++) {
        int u = -1;
        for (int v = 0; v < n; v++) {   /* O(V): menor custo fora da árvore */
            comparacoes++;
            if (!na_arvore[v] && custo[v] != INF && (u == -1 || custo[v] < custo[u])) u = v;
        }
        if (u == -1) break;
        na_arvore[u] = 1;
        total += custo[u];
        printf("\n--- Iteração %d: cabo mais barato saindo da árvore leva a %s", it, nome[u]);
        if (pred[u] >= 0) printf(" (%s—%s, %d m)", nome[pred[u]], nome[u], custo[u]);
        printf(" ---\n");
        for (int v = 0; v < n; v++) {   /* O(V): linha u da matriz */
            comparacoes++;
            if (peso[u][v] == 0 || na_arvore[v]) continue;
            if (peso[u][v] < custo[v]) {
                printf("      %s—%s = %d m < ", nome[u], nome[v], peso[u][v]);
                if (custo[v] == INF) printf("∞"); else printf("%d", custo[v]);
                printf(" -> custo[%s] = %d, pred = %s\n", nome[v], peso[u][v], nome[u]);
                custo[v] = peso[u][v];
                pred[v] = u;
            }
        }
        desenha_prim(n, custo, pred, na_arvore, u);
        pausar();
    }
    printf("\n  Total pelo Prim: %d m (o mesmo do Kruskal; se houver empates de custo, as\n", total);
    printf("  árvores podem diferir, mas o total mínimo é sempre o mesmo).\n");
    printf("  %ld comparações = 2·V·V = 2·%d·%d: O(V²), ótimo para grafos densos.\n\n", comparacoes, n, n);
    pausar();
}

/* ==========================================================================
 * PARTE 3 — REDE DESCONEXA: FLORESTA GERADORA E COMPONENTES
 * ==========================================================================
 */
static void parte3_floresta(void) {
    Aresta candidatas[32];
    int m = 0;
    for (int k = 0; k < N_TRAJETOS; k++) candidatas[m++] = trajetos[k];
    candidatas[m++] = (Aresta){ S8, S9, 6, N_TRAJETOS };   /* mezanino: sem trajeto até o resto */

    printf("=== PARTE 3: S8 e S9 no mezanino, sem eletrocalha até o resto do galpão ===\n\n");
    Aresta aceitas[MAX_V];
    int qtd;
    long finds;
    int total = kruskal(candidatas, m, MAX_V, aceitas, &qtd, 0, &finds);

    UnionFind uf;
    uf_inicia(&uf, MAX_V);
    for (int k = 0; k < qtd; k++) uf_une(&uf, aceitas[k].u, aceitas[k].v);
    printf("  Kruskal aceitou %d trechos (%d m) para %d vértices.\n", qtd, total, MAX_V);
    int comp = desenha_componentes(&uf);
    printf("  Floresta: componentes = V - trechos = %d - %d = %d. Cada componente recebeu\n",
           MAX_V, qtd, comp);
    printf("  sua própria árvore mínima; é preciso um novo trajeto (ou um segundo gateway)\n");
    printf("  para S8 e S9 chegarem ao supervisório. Union-Find conta componentes em\n");
    printf("  O(E·α(V)), alternativa à DFS de 05_dfs_ciclos.c.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    parte1_kruskal();
    parte2_prim();
    parte3_floresta();
    return 0;
}
