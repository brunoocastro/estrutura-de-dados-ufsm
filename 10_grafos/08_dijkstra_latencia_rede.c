/*
 * ============================================================================
 * Arquivo : 08_dijkstra_latencia_rede.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidade 6.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver o algoritmo de DIJKSTRA passo a passo: a cada iteração, a tabela de
 *   distâncias e predecessores, o vértice FIXADO e cada relaxamento de aresta
 *   ("melhora" ou "não melhora"). Depois, a mesma busca com um HEAP mínimo
 *   (fila de prioridade) e uma tabela que mostra quando O(V²) com vetor vence
 *   e quando O((V + E) log V) com heap vence.
 *
 * EXEMPLO REAL
 *   Latência numa rede Ethernet industrial (em microssegundos). O servidor
 *   SCADA precisa do caminho de MENOR LATÊNCIA até cada CLP e remota de I/O.
 *   A rota com menos saltos (BFS, 04_bfs_saltos_rede.c) passa por um rádio
 *   sem fio lento; a de menor latência dá mais saltos por cabos rápidos.
 *   Protocolos de roteamento como OSPF e IS-IS rodam Dijkstra em cada roteador.
 *
 * COMPLEXIDADE (V = vértices, E = arestas; pesos NÃO negativos)
 *   Implementação                  Tempo              Melhor quando
 *   vetor (busca linear do mínimo) O(V² + E) = O(V²)  grafo denso (E ~ V²)
 *   heap binário (fila prioridade) O((V + E) log V)   grafo esparso (E ~ V)
 *   Espaço extra: O(V) no vetor; O(V + E) no heap (entradas obsoletas).
 *   Por quê: com vetor, cada uma das V iterações procura o mínimo varrendo os
 *   V vértices (V·V) e cada aresta é relaxada uma vez (E). Com heap, achar o
 *   mínimo custa O(log) e cada relaxamento que melhora faz uma inserção O(log);
 *   são no máximo E inserções e E remoções, e log E <= 2 log V.
 *   No esparso, V log V ganha de V². No denso, o heap só perde quando muitos
 *   relaxamentos melhoram de fato (pior caso: E log V ~ V² log V > V²); com
 *   pesos aleatórios poucos melhoram e o heap ainda ganha. A Parte 3 mede os
 *   dois casos.
 *
 * COMO EXECUTAR
 *   ./run 08_dijkstra_latencia_rede               (ENTER avança cada passo)
 *   ./run 08_dijkstra_latencia_rede < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INF INT_MAX   /* "ainda sem caminho conhecido" */

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
 * GRAFO PONDERADO NÃO DIRIGIDO (lista de adjacência)
 * ==========================================================================
 */
typedef struct No {
    int vertice;
    int peso;          /* latência do enlace em µs */
    struct No *prox;
} No;

typedef struct {
    int n, e;
    No **inicio, **fim;
    const char *const *nome;   /* NULL nas tabelas de crescimento */
} Grafo;

static Grafo cria(int n, const char *const *nome) {
    Grafo g = { n, 0, aloca((size_t)n, sizeof(No *)), aloca((size_t)n, sizeof(No *)), nome };
    return g;
}

static void anexa(Grafo *g, int u, int v, int peso) {
    No *novo = aloca(1, sizeof *novo);
    novo->vertice = v;
    novo->peso = peso;
    if (g->fim[u] == NULL) g->inicio[u] = novo;
    else g->fim[u]->prox = novo;
    g->fim[u] = novo;
}

static void enlace(Grafo *g, int u, int v, int peso) {
    anexa(g, u, v, peso);
    anexa(g, v, u, peso);
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
}

/* ==========================================================================
 * A REDE DA PLANTA (latências em µs)
 * ==========================================================================
 */
enum { SCADA, CORE, SW_A, SW_B, SW_C, RADIO, CLP_A, CLP_B, RIO_C, NV };
static const char *const nome[NV] = {
    "SCADA", "SW-CORE", "SW-A", "SW-B", "SW-C", "RADIO", "CLP-A", "CLP-B", "RIO-C"
};

static void monta_rede(Grafo *g) {
    int enlaces[][3] = {
        {SCADA, CORE, 10}, {CORE, SW_A, 15}, {CORE, SW_B, 25}, {CORE, RADIO, 12},
        {RADIO, RIO_C, 900},   /* ponte sem fio: poucos saltos, latência alta  */
        {SW_A, SW_B, 5}, {SW_A, SW_C, 120},   /* SW-A—SW-C: enlace congestionado */
        {SW_B, SW_C, 40}, {SW_C, RIO_C, 30}, {SW_A, CLP_A, 25}, {SW_B, CLP_B, 25},
    };
    for (size_t k = 0; k < sizeof enlaces / sizeof enlaces[0]; k++) {
        enlace(g, enlaces[k][0], enlaces[k][1], enlaces[k][2]);
    }
}

static void desenha_rede(void) {
    printf("                  SCADA\n");
    printf("                    | 10\n");
    printf("     +---------- SW-CORE ----------+\n");
    printf("     | 15           | 25           | 12\n");
    printf("   SW-A ----5---- SW-B           RADIO\n");
    printf("   |  \\            |  \\             |\n");
    printf("   25  \\---120---  40  25           900 (sem fio)\n");
    printf("   |            \\  |    \\           |\n");
    printf("  CLP-A          SW-C   CLP-B       |\n");
    printf("                  |  30             |\n");
    printf("                RIO-C --------------+\n\n");
}

/* ==========================================================================
 * PARTE 1 — DIJKSTRA COM VETOR, PASSO A PASSO
 * ==========================================================================
 */
static void desenha_tabela(const Grafo *g, const int dist[], const int pred[],
                           const int fixado[], int escolhido) {
    printf("      vértice   dist(µs)  pred     estado\n");   /* literal: acentos têm 2 bytes */
    for (int v = 0; v < g->n; v++) {
        printf("      %-8s  ", g->nome[v]);
        if (dist[v] == INF) printf("%7s∞  ", "");             /* "∞": 3 bytes, 1 coluna */
        else printf("%8d  ", dist[v]);
        printf("%-8s %s\n", pred[v] >= 0 ? g->nome[pred[v]] : "-",
               v == escolhido ? "FIXADO AGORA  <==" : fixado[v] ? "fixado" : "");
    }
}

/*
 * Dijkstra "de livro" com vetor.
 * Invariante: quando u é fixado, dist[u] é definitivo. Vale porque todos os
 * pesos são >= 0: qualquer outro caminho até u passaria por um vértice não
 * fixado com distância >= dist[u] e só poderia ficar mais caro. Com pesos
 * negativos essa garantia cai (use Bellman-Ford).
 * Contadores: *comp = comparações para achar o mínimo; *relax = arestas testadas.
 */
static void dijkstra_vetor(const Grafo *g, int origem, int dist[], int pred[], int narrar,
                           long *comp, long *relax) {
    int *fixado = aloca((size_t)g->n, sizeof(int));
    *comp = *relax = 0;
    for (int v = 0; v < g->n; v++) { dist[v] = INF; pred[v] = -1; }
    dist[origem] = 0;

    for (int it = 1; it <= g->n; it++) {
        int u = -1;
        for (int v = 0; v < g->n; v++) {          /* O(V): varre o vetor inteiro */
            (*comp)++;
            if (!fixado[v] && dist[v] != INF && (u == -1 || dist[v] < dist[u])) u = v;
        }
        if (u == -1) break;                       /* o resto é inalcançável */
        fixado[u] = 1;
        if (narrar) {
            printf("\n--- Iteração %d: menor distância ainda não fixada = %s (%d µs) ---\n",
                   it, g->nome[u], dist[u]);
        }
        for (const No *a = g->inicio[u]; a != NULL; a = a->prox) {
            int v = a->vertice;
            (*relax)++;
            if (fixado[v]) {
                if (narrar) printf("      %s—%s: %s já fixado -> ignora\n", g->nome[u], g->nome[v], g->nome[v]);
                continue;
            }
            int nova = dist[u] + a->peso;
            if (nova < dist[v]) {
                if (narrar) {
                    printf("      %s—%s (%d): %d + %d = %d < ", g->nome[u], g->nome[v], a->peso,
                           dist[u], a->peso, nova);
                    if (dist[v] == INF) printf("∞");
                    else printf("%d", dist[v]);
                    printf(" -> MELHORA: dist[%s] = %d, pred = %s\n", g->nome[v], nova, g->nome[u]);
                }
                dist[v] = nova;
                pred[v] = u;
            } else if (narrar) {
                printf("      %s—%s (%d): %d + %d = %d >= %d -> não melhora\n", g->nome[u],
                       g->nome[v], a->peso, dist[u], a->peso, nova, dist[v]);
            }
        }
        if (narrar) {
            desenha_tabela(g, dist, pred, fixado, u);
            pausar();
        }
    }
    free(fixado);
}

static void imprime_rota(const Grafo *g, const int pred[], int destino) {
    int rota[NV], k = 0;
    for (int v = destino; v != -1; v = pred[v]) rota[k++] = v;
    int saltos = k - 1;
    while (k > 0) {
        k--;
        printf("%s%s", g->nome[rota[k]], k > 0 ? " -> " : "");
    }
    printf("  (%d salto%s)", saltos, saltos == 1 ? "" : "s");
}

static void parte1_vetor(void) {
    Grafo g = cria(NV, nome);
    monta_rede(&g);
    printf("=== PARTE 1: Dijkstra com vetor — menor latência a partir do SCADA ===\n\n");
    desenha_rede();
    printf("  Início: dist[SCADA] = 0, todos os outros = ∞, nenhum fixado.\n");
    pausar();

    int dist[NV], pred[NV];
    long comp, relax;
    dijkstra_vetor(&g, SCADA, dist, pred, 1, &comp, &relax);

    printf("\n=== Rotas de menor latência ===\n");
    for (int v = 1; v < NV; v++) {
        printf("  %-7s %4d µs: ", nome[v], dist[v]);
        imprime_rota(&g, pred, v);
        printf("\n");
    }
    printf("\n  Contadores: %ld comparações para achar mínimos (= V·V = %d·%d) e %ld\n",
           comp, NV, NV, relax);
    printf("  arestas relaxadas (= 2E = 2·%d). Veja SW-B: começou com 35 µs via SW-CORE e\n", g.e);
    printf("  caiu para 30 µs via SW-A; SW-C caiu de 145 para 70 e RIO-C de 922 para 100.\n\n");

    /* Contraste com a BFS: menos saltos não é menos latência. */
    int saltos[NV], pai[NV], fila[NV], ini = 0, fim = 0;
    for (int v = 0; v < NV; v++) { saltos[v] = -1; pai[v] = -1; }
    saltos[SCADA] = 0;
    fila[fim++] = SCADA;
    while (ini < fim) {
        int u = fila[ini++];
        for (const No *a = g.inicio[u]; a != NULL; a = a->prox) {
            if (saltos[a->vertice] < 0) {
                saltos[a->vertice] = saltos[u] + 1;
                pai[a->vertice] = u;
                fila[fim++] = a->vertice;
            }
        }
    }
    int lat = 0;
    for (int v = RIO_C; pai[v] != -1; v = pai[v]) {
        for (const No *a = g.inicio[v]; a != NULL; a = a->prox) {
            if (a->vertice == pai[v]) { lat = lat + a->peso; break; }
        }
    }
    printf("  BFS (menos saltos) até RIO-C: ");
    imprime_rota(&g, pai, RIO_C);
    printf(" = %d µs\n", lat);
    printf("  Dijkstra (menor soma)       : ");
    imprime_rota(&g, pred, RIO_C);
    printf(" = %d µs\n", dist[RIO_C]);
    printf("  A BFS conta arestas como se todas custassem 1; o rádio de 900 µs some da conta.\n\n");
    libera(&g);
    pausar();
}

/* ==========================================================================
 * PARTE 2 — A MESMA BUSCA COM HEAP MÍNIMO
 * ==========================================================================
 * Heap binário guardado em vetor (como em ../09_arvores/): filhos de i em
 * 2i+1 e 2i+2; o menor fica na raiz (posição 0).
 *
 * "Inserção preguiçosa": quando dist[v] melhora, inserimos um par novo
 * (dist, v) em vez de procurar e alterar o antigo dentro do heap. O par antigo
 * vira OBSOLETO e é descartado quando sai (dist do par > dist[v]). É a forma
 * mais simples de implementar e mantém O((V + E) log V).
 */
typedef struct { int d, v; } Par;

typedef struct {
    Par *item;
    int qtd;
    long comp;   /* comparações entre pares: o custo log do heap */
} Heap;

static void troca(Par *a, Par *b) { Par t = *a; *a = *b; *b = t; }

/* O(log n): sobe o novo item enquanto for menor que o pai. */
static void heap_insere(Heap *h, int d, int v) {
    int i = h->qtd++;
    h->item[i] = (Par){ d, v };
    while (i > 0) {
        int pai = (i - 1) / 2;
        h->comp++;
        if (h->item[pai].d <= h->item[i].d) break;
        troca(&h->item[pai], &h->item[i]);
        i = pai;
    }
}

/* O(log n): tira a raiz, põe o último no lugar e desce até a posição certa. */
static Par heap_remove_min(Heap *h) {
    Par min = h->item[0];
    h->item[0] = h->item[--h->qtd];
    int i = 0;
    for (;;) {
        int menor = i, esq = 2 * i + 1, dir = 2 * i + 2;
        if (esq < h->qtd) { h->comp++; if (h->item[esq].d < h->item[menor].d) menor = esq; }
        if (dir < h->qtd) { h->comp++; if (h->item[dir].d < h->item[menor].d) menor = dir; }
        if (menor == i) break;
        troca(&h->item[i], &h->item[menor]);
        i = menor;
    }
    return min;
}

static void desenha_heap(const Grafo *g, const Heap *h) {
    printf("      heap (vetor; raiz = menor):");
    if (h->qtd == 0) printf(" vazio");
    for (int i = 0; i < h->qtd; i++) printf(" (%d,%s)", h->item[i].d, g->nome[h->item[i].v]);
    printf("\n");
}

/* Contadores: *comp = comparações dentro do heap; *relax = arestas testadas. */
static void dijkstra_heap(const Grafo *g, int origem, int dist[], int pred[], int narrar,
                          long *comp, long *relax) {
    int *fixado = aloca((size_t)g->n, sizeof(int));
    Heap h = { aloca((size_t)(2 * g->e + 1), sizeof(Par)), 0, 0 };   /* <= 1 inserção por relaxamento */
    *relax = 0;
    for (int v = 0; v < g->n; v++) { dist[v] = INF; pred[v] = -1; }
    dist[origem] = 0;
    heap_insere(&h, 0, origem);
    if (narrar) desenha_heap(g, &h);

    while (h.qtd > 0) {
        Par p = heap_remove_min(&h);
        int u = p.v;
        if (fixado[u]) {   /* par obsoleto: já saiu antes com distância menor */
            if (narrar) printf("    remove (%d,%s): obsoleto, %s já fixado com %d -> descarta\n",
                               p.d, g->nome[u], g->nome[u], dist[u]);
            continue;
        }
        fixado[u] = 1;
        if (narrar) printf("    remove (%d,%s): FIXA %s;", p.d, g->nome[u], g->nome[u]);
        int inseridos = 0;
        for (const No *a = g->inicio[u]; a != NULL; a = a->prox) {
            int v = a->vertice;
            (*relax)++;
            if (!fixado[v] && dist[u] + a->peso < dist[v]) {
                dist[v] = dist[u] + a->peso;
                pred[v] = u;
                heap_insere(&h, dist[v], v);
                if (narrar) printf(" insere (%d,%s)", dist[v], g->nome[v]);
                inseridos++;
            }
        }
        if (narrar) {
            if (!inseridos) printf(" nenhum vizinho melhora");
            printf("\n");
            desenha_heap(g, &h);
        }
    }
    *comp = h.comp;
    free(h.item);
    free(fixado);
}

static void parte2_heap(void) {
    Grafo g = cria(NV, nome);
    monta_rede(&g);
    printf("=== PARTE 2: a mesma busca com HEAP mínimo (fila de prioridade) ===\n\n");
    int dv[NV], pv[NV], dh[NV], ph[NV];
    long cv, rv, ch, rh;
    dijkstra_vetor(&g, SCADA, dv, pv, 0, &cv, &rv);
    dijkstra_heap(&g, SCADA, dh, ph, 1, &ch, &rh);

    int iguais = 1;
    for (int v = 0; v < NV; v++) if (dv[v] != dh[v]) iguais = 0;
    printf("\n  Distâncias iguais às do vetor? %s\n", iguais ? "sim" : "NÃO (erro!)");
    printf("  Vetor: %ld comparações para mínimos; heap: %ld comparações internas.\n", cv, ch);
    printf("  Com só 9 vértices a diferença é pequena; a tabela a seguir mostra o crescimento.\n\n");
    libera(&g);
    pausar();
}

/* ==========================================================================
 * PARTE 3 — VETOR x HEAP EM GRAFOS ESPARSOS E DENSOS
 * ==========================================================================
 */
static unsigned long semente = 42;
static unsigned long sorteio(void) {
    semente = semente * 1103515245UL + 12345UL;
    return (semente >> 16) & 0x7fffUL;
}

static int peso_aleatorio(void) { return 1 + (int)(sorteio() % 100); }

/* Esparso: anel + corda (i, i+V/3): ~4 vizinhos por vértice, E = 2V. */
static void gera_esparso(Grafo *g) {
    for (int v = 0; v < g->n; v++) {
        enlace(g, v, (v + 1) % g->n, peso_aleatorio());
        enlace(g, v, (v + g->n / 3) % g->n, peso_aleatorio());
    }
}

/* Denso aleatório: cada par existe com 50% de chance, E ~ V²/4. */
static void gera_denso(Grafo *g) {
    for (int u = 0; u < g->n; u++) {
        for (int v = u + 1; v < g->n; v++) {
            if (sorteio() % 2) enlace(g, u, v, peso_aleatorio());
        }
    }
}

/*
 * Denso no PIOR CASO para o heap: grafo completo em que TODO relaxamento
 * melhora. Os vértices são fixados na ordem 0, 1, 2, ... (dist[i] = i pela
 * cadeia de peso 1) e o atalho i—j (j > i+1) custa 2V+1-2i. Passando por i,
 * o candidato para j vale i + (2V+1-2i) = 2V+1-i, que diminui a cada novo i
 * fixado: toda aresta examinada melhora dist[j] e gera uma inserção no heap.
 * Resultado: ~V²/2 inserções, cada uma O(log V).
 */
static void gera_denso_pior(Grafo *g) {
    for (int i = 0; i < g->n; i++) {
        for (int j = i + 1; j < g->n; j++) {
            enlace(g, i, j, j == i + 1 ? 1 : 2 * g->n + 1 - 2 * i);
        }
    }
}

static void parte3_crescimento(void) {
    const int esparsos[] = { 10, 100, 1000, 10000 };
    const int densos[] = { 10, 100, 1000 };
    static const char *const rotulo[] = { "esparso", "denso", "pior" };
    printf("=== PARTE 3: operações contadas (comparações + relaxamentos) ===\n\n");
    printf("  esparso = ~4 vizinhos por vértice; denso = 50%% dos pares, pesos aleatórios;\n");
    printf("  pior    = grafo completo em que todo relaxamento melhora (pior caso do heap)\n\n");
    printf("  %-8s %6s %8s | %15s %14s | %s\n", "tipo", "V", "E", "vetor O(V²)", "heap", "vence");
    printf("  ----------------------------------------------------------------------\n");
    for (int tipo = 0; tipo < 3; tipo++) {
        int qtd = tipo == 0 ? 4 : 3;
        for (int i = 0; i < qtd; i++) {
            int n = tipo == 0 ? esparsos[i] : densos[i];
            Grafo g = cria(n, NULL);
            if (tipo == 0) gera_esparso(&g);
            else if (tipo == 1) gera_denso(&g);
            else gera_denso_pior(&g);
            int *dv = aloca((size_t)n, sizeof(int)), *pv = aloca((size_t)n, sizeof(int));
            int *dh = aloca((size_t)n, sizeof(int)), *ph = aloca((size_t)n, sizeof(int));
            long cv, rv, ch, rh;
            dijkstra_vetor(&g, 0, dv, pv, 0, &cv, &rv);
            dijkstra_heap(&g, 0, dh, ph, 0, &ch, &rh);
            for (int v = 0; v < n; v++) {
                if (dv[v] != dh[v]) { fprintf(stderr, "divergência em %d\n", v); exit(EXIT_FAILURE); }
            }
            long tv = cv + rv, th = ch + rh;
            printf("  %-8s %6d %8d | %14ld %14ld | %s\n", rotulo[tipo], n, g.e,
                   tv, th, tv < th ? "vetor" : "heap");
            free(dv); free(pv); free(dh); free(ph);
            libera(&g);
        }
    }
    printf("\n  Esparso: o vetor paga V² só para achar mínimos (10 mil vértices = 100 milhões);\n");
    printf("  o heap paga ~E·log V. Denso aleatório: poucas distâncias melhoram, então o\n");
    printf("  heap faz poucas inserções e ainda ganha. Pior caso: ~V²/2 inserções de custo\n");
    printf("  log V, e a busca linear simples vence. O(V²) do vetor vale em qualquer caso;\n");
    printf("  O((V + E) log V) do heap é o limite do pior caso. Resultados sempre iguais.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    parte1_vetor();
    parte2_heap();
    parte3_crescimento();
    return 0;
}
