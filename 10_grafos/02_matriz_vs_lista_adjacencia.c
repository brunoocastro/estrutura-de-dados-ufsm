/*
 * ============================================================================
 * Arquivo : 02_matriz_vs_lista_adjacencia.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidade 6.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Montar o MESMO grafo nas duas representações físicas clássicas, aresta por
 *   aresta, e medir o que cada uma custa:
 *     - memória ocupada: O(V²) da matriz contra O(V + E) da lista;
 *     - pergunta "existe a aresta u-v?": O(1) na matriz, O(grau(u)) na lista;
 *     - pergunta "quem são os vizinhos de v?": O(V) na matriz, O(grau(v)) na lista.
 *   No fim, duas tabelas mostram quem vence em grafos ESPARSOS e DENSOS.
 *
 * EXEMPLO REAL
 *   Célula de manufatura: CLP, IHM, inversor, remota de I/O e servidor SCADA
 *   ligados a um switch. Redes industriais, mapas e redes sociais são esparsos
 *   (cada nó liga-se a poucos outros), por isso quase sempre usam lista de
 *   adjacência. Matrizes aparecem em grafos pequenos e densos, como a tabela
 *   de distâncias entre todas as estações de um sistema de transporte.
 *
 * COMPLEXIDADE (V = vértices, E = arestas, grau(v) = vizinhos de v)
 *   Operação              Matriz        Lista de adjacência
 *   memória               O(V²)         O(V + E)
 *   inserir aresta        O(1)          O(1) no início da lista (sem checar repetida)
 *   existe aresta u-v?    O(1)          O(grau(u))
 *   vizinhos de v         O(V)          O(grau(v))
 *   visitar todo o grafo  O(V²)         O(V + E)      (base de BFS e DFS)
 *   Por quê: a matriz reserva uma célula para CADA par possível (V·V), exista a
 *   aresta ou não; acessar m[u][v] é um cálculo de endereço. A lista guarda
 *   apenas as arestas que existem, mas para achar uma é preciso caminhar nela.
 *
 * COMO EXECUTAR
 *   ./run 02_matriz_vs_lista_adjacencia               (ENTER avança cada passo)
 *   ./run 02_matriz_vs_lista_adjacencia < /dev/null   (executa direto, sem pausas)
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

/* calloc que aborta com mensagem: evita repetir o teste de NULL em todo lugar. */
static void *aloca(size_t qtd, size_t tamanho) {
    void *p = calloc(qtd, tamanho);
    if (p == NULL) {
        fprintf(stderr, "Falha de alocação\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

/* ==========================================================================
 * AS DUAS REPRESENTAÇÕES (genéricas, com V escolhido em tempo de execução)
 * ==========================================================================
 *
 * Matriz: um único bloco de V·V bytes. A célula (u, v) fica na posição
 * u*V + v ("linha u, coluna v"). Usamos unsigned char (1 byte) para dar à
 * matriz a MELHOR chance possível; com pesos int ela ocuparia 4x mais.
 *
 * Lista: um vetor de V ponteiros; lista[v] aponta para o primeiro nó da lista
 * encadeada de vizinhos de v (mesma ideia de ../07_listas/).
 */
typedef struct No {
    int vertice;
    struct No *prox;
} No;

typedef struct {
    int v;                /* número de vértices                         */
    int e;                /* número de arestas (não dirigidas)          */
    unsigned char *mat;   /* V·V células: 1 = aresta, 0 = sem aresta     */
    No **lista;           /* V cabeças de lista                          */
} Grafo;

/* O(V²) de memória já na criação: a matriz nasce do tamanho final. */
static Grafo cria(int v) {
    Grafo g = { v, 0, aloca((size_t)v * (size_t)v, 1), aloca((size_t)v, sizeof(No *)) };
    return g;
}

/* O(1): inserir no INÍCIO da lista evita percorrê-la até o fim. */
static void empilha_no(No **cabeca, int vizinho) {
    No *novo = aloca(1, sizeof *novo);
    novo->vertice = vizinho;
    novo->prox = *cabeca;
    *cabeca = novo;
}

/*
 * Insere a aresta não dirigida u-v nas DUAS estruturas. A matriz é usada para
 * rejeitar repetidas em O(1); só com a lista o teste custaria O(grau(u)).
 * Armadilha: no grafo não dirigido, esquecer o lado simétrico (v,u) gera um
 * grafo "meio dirigido" e percursos que não voltam.
 */
static int insere(Grafo *g, int u, int v) {
    if (u == v || g->mat[(size_t)u * g->v + v]) return 0;
    g->mat[(size_t)u * g->v + v] = 1;
    g->mat[(size_t)v * g->v + u] = 1;
    empilha_no(&g->lista[u], v);
    empilha_no(&g->lista[v], u);
    g->e++;
    return 1;
}

static void libera(Grafo *g) {
    for (int i = 0; i < g->v; i++) {
        while (g->lista[i] != NULL) {
            No *prox = g->lista[i]->prox;
            free(g->lista[i]);
            g->lista[i] = prox;
        }
    }
    free(g->lista);
    free(g->mat);
    g->lista = NULL;
    g->mat = NULL;
}

/* O(1): um único acesso, qualquer que seja o tamanho do grafo. */
static int existe_matriz(const Grafo *g, int u, int v, long *acessos) {
    (*acessos)++;
    return g->mat[(size_t)u * g->v + v];
}

/* O(grau(u)): no pior caso (aresta ausente) percorre a lista inteira. */
static int existe_lista(const Grafo *g, int u, int v, long *acessos) {
    for (const No *a = g->lista[u]; a != NULL; a = a->prox) {
        (*acessos)++;
        if (a->vertice == v) return 1;
    }
    return 0;
}

/* ==========================================================================
 * PARTE 1 — O GRAFO DA CÉLULA DE MANUFATURA, PASSO A PASSO
 * ==========================================================================
 */
enum { CLP, IHM, SW, INV, RIO, SCADA, NV };
static const char *nome[NV] = { "CLP", "IHM", "SW", "INV", "RIO", "SCADA" };

/* Desenha a matriz; as células (nu,nv) e (nv,nu) recém-gravadas saem entre colchetes. */
static void desenha_matriz(const Grafo *g, int nu, int nv) {
    printf("      Matriz %dx%d (. = 0)\n      %6s", g->v, g->v, "");
    for (int j = 0; j < g->v; j++) printf("%6s", nome[j]);
    printf("\n");
    for (int i = 0; i < g->v; i++) {
        printf("      %6s", nome[i]);
        for (int j = 0; j < g->v; j++) {
            int novo = (i == nu && j == nv) || (i == nv && j == nu);
            if (novo) printf("   [1]");
            else printf("%6s", g->mat[(size_t)i * g->v + j] ? "1" : ".");
        }
        printf("\n");
    }
}

/* Desenha as listas no formato de nós encadeados [vizinho|*]-->... */
static void desenha_listas(const Grafo *g) {
    printf("      Listas de adjacência (novo nó sempre entra no início)\n");
    for (int i = 0; i < g->v; i++) {
        printf("      %6s -> ", nome[i]);
        for (const No *a = g->lista[i]; a != NULL; a = a->prox) {
            printf("[%s|*]-->", nome[a->vertice]);
        }
        printf("NULL\n");
    }
}

static void parte1_montagem(Grafo *g) {
    /*
     *            IHM     SCADA
     *              \      /
     *     CLP ----- SW ------ INV
     *       \       |
     *        \---- RIO
     */
    int cabos[][2] = { {CLP, SW}, {IHM, SW}, {SCADA, SW}, {INV, SW}, {RIO, SW}, {CLP, RIO} };
    int qtd = (int)(sizeof cabos / sizeof cabos[0]);

    printf("=== PARTE 1: montando o MESMO grafo nas duas representações ===\n\n");
    printf("            IHM     SCADA\n");
    printf("              \\      /\n");
    printf("     CLP ----- SW ------ INV\n");
    printf("       \\       |\n");
    printf("        \\---- RIO\n\n");
    pausar();

    for (int k = 0; k < qtd; k++) {
        int u = cabos[k][0], v = cabos[k][1];
        insere(g, u, v);
        printf("--- Passo %d: cabo %s — %s ---\n", k + 1, nome[u], nome[v]);
        printf("      matriz: grava m[%s][%s] = 1 e m[%s][%s] = 1 (2 escritas, O(1))\n",
               nome[u], nome[v], nome[v], nome[u]);
        printf("      lista : cria 1 nó na lista de %s e 1 na de %s (O(1) cada)\n\n",
               nome[u], nome[v]);
        desenha_matriz(g, u, v);
        printf("\n");
        desenha_listas(g);
        printf("\n");
        pausar();
    }
}

/* ==========================================================================
 * PARTE 2 — AS DUAS PERGUNTAS QUE TODO ALGORITMO DE GRAFO FAZ
 * ==========================================================================
 */
/* O(grau(v)): conta os nós da lista de v. */
static int grau(const Grafo *g, int v) {
    int d = 0;
    for (const No *a = g->lista[v]; a != NULL; a = a->prox) d++;
    return d;
}

/* "Existe a aresta u-v?": 1 leitura na matriz; caminhada narrada na lista. */
static void pergunta_existe(const Grafo *g, int u, int v) {
    long am = 0, al = 0;
    printf("  Pergunta: existe o cabo %s — %s?\n", nome[u], nome[v]);
    int rm = existe_matriz(g, u, v, &am);
    printf("    matriz: lê m[%s][%s] = %d -> %s   (%ld acesso, O(1))\n",
           nome[u], nome[v], rm, rm ? "SIM" : "NÃO", am);
    printf("    lista : percorre a lista de %s:\n", nome[u]);
    int achou = 0;
    for (const No *a = g->lista[u]; a != NULL && !achou; a = a->prox) {
        al++;
        achou = a->vertice == v;
        printf("            compara %-5s com %-5s -> %s\n", nome[a->vertice], nome[v],
               achou ? "igual, ACHOU" : "diferente, segue para o próximo");
    }
    if (!achou) printf("            chegou ao NULL -> NÃO existe\n");
    printf("            %ld acesso(s)  (pior caso O(grau(%s)) = %d)\n\n", al, nome[u], grau(g, u));
}

/* "Quem são os vizinhos de v?": a matriz varre a linha inteira; a lista, só os vizinhos. */
static void pergunta_vizinhos(const Grafo *g, int v) {
    long am = 0, al = 0;
    printf("  Pergunta: quem são os vizinhos de %s?\n", nome[v]);
    printf("    matriz: varre a linha %s inteira:", nome[v]);
    for (int j = 0; j < g->v; j++) {
        am++;
        if (g->mat[(size_t)v * g->v + j]) printf(" %s", nome[j]);
    }
    printf("   (%ld acessos = V; só %d célula(s) valem 1)\n", am, grau(g, v));
    printf("    lista : segue os nós:            ");
    for (const No *a = g->lista[v]; a != NULL; a = a->prox) {
        al++;
        printf(" %s", nome[a->vertice]);
    }
    printf("   (%ld acesso(s) = grau(%s))\n\n", al, nome[v]);
}

static void parte2_consultas(const Grafo *g) {
    printf("=== PARTE 2: as duas perguntas que BFS, DFS e Dijkstra fazem o tempo todo ===\n\n");
    pergunta_existe(g, SW, CLP);    /* CLP foi o 1º inserido na lista de SW: fica no fim */
    pergunta_existe(g, SW, SCADA);  /* inserido depois: fica perto do início            */
    pergunta_existe(g, IHM, INV);   /* não existe: a lista de IHM tem 1 nó só           */
    pausar();
    pergunta_vizinhos(g, IHM);      /* esparso: matriz lê 6 células para achar 1 vizinho */
    pergunta_vizinhos(g, SW);       /* hub: aqui a diferença é pequena                   */
    printf("  Conclusão: a matriz responde 'existe?' em O(1), mas paga O(V) para listar\n");
    printf("  vizinhos. BFS/DFS listam vizinhos de TODO vértice: V·V na matriz, 2E na lista.\n\n");
    pausar();
}

/* ==========================================================================
 * PARTE 3 — MEMÓRIA: CONTAS COM sizeof
 * ==========================================================================
 */

/* Escreve um número de bytes em B/KB/MB/GB para caber na tabela. */
static void formata_bytes(double b, char *saida, size_t tam) {
    const char *unidade[] = { "B", "KB", "MB", "GB", "TB" };
    int u = 0;
    while (b >= 1024.0 && u < 4) { b /= 1024.0; u++; }
    if (u == 0) snprintf(saida, tam, "%.0f %s", b, unidade[u]);
    else snprintf(saida, tam, "%.1f %s", b, unidade[u]);
}

/* Memória teórica: matriz V·V bytes; lista V ponteiros + 2E nós (cada aresta em 2 listas). */
static double bytes_matriz(double v) { return v * v * sizeof(unsigned char); }
static double bytes_lista(double v, double e) {
    return v * sizeof(No *) + 2.0 * e * sizeof(No);
}

static void parte3_memoria(const Grafo *g) {
    char bm[32], bl[32];
    printf("=== PARTE 3: memória ocupada ===\n\n");
    printf("  sizeof(unsigned char) = %zu (célula da matriz)\n", sizeof(unsigned char));
    printf("  sizeof(No *)          = %zu (cabeça de lista)\n", sizeof(No *));
    printf("  sizeof(No)            = %zu (int + ponteiro + alinhamento)\n\n", sizeof(No));
    formata_bytes(bytes_matriz(g->v), bm, sizeof bm);
    formata_bytes(bytes_lista(g->v, g->e), bl, sizeof bl);
    printf("  Grafo da célula: V = %d, E = %d\n", g->v, g->e);
    printf("    matriz: V·V = %d·%d células                     = %s\n", g->v, g->v, bm);
    printf("    lista : V ponteiros + 2E nós = %d·%zu + %d·%zu = %s\n", g->v, sizeof(No *),
           2 * g->e, sizeof(No), bl);
    printf("  Surpresa: com 6 vértices a MATRIZ gasta menos! Cada nó da lista carrega um\n");
    printf("  ponteiro de 8 bytes. A vantagem da lista só aparece quando V cresce e o\n");
    printf("  grafo continua esparso (próxima tabela). Armadilha: malloc ainda soma\n");
    printf("  ~16 bytes de controle por nó, que não entram nesta conta.\n\n");
    pausar();
}

/* ==========================================================================
 * PARTE 4 — TABELA DE MEMÓRIA PARA GRAFOS ESPARSOS E DENSOS
 * ==========================================================================
 * Esparso: grau médio 4 (cada switch liga-se a ~4 vizinhos) -> E = 2V.
 * Denso  : metade de todos os pares possíveis               -> E = V(V-1)/4.
 */
static void parte4_tabela_memoria(void) {
    const double tamanhos[] = { 10, 100, 1000, 10000 };
    printf("=== PARTE 4: memória (fórmulas acima) — esparso x denso ===\n\n");
    printf("  %-8s %6s %12s %12s %12s   %s\n", "tipo", "V", "E", "matriz", "lista", "menor");
    printf("  -------------------------------------------------------------------------\n");
    for (int tipo = 0; tipo < 2; tipo++) {
        for (int i = 0; i < 4; i++) {
            double v = tamanhos[i];
            double e = tipo == 0 ? 2.0 * v : v * (v - 1.0) / 4.0;
            double m = bytes_matriz(v), l = bytes_lista(v, e);
            char bm[32], bl[32];
            formata_bytes(m, bm, sizeof bm);
            formata_bytes(l, bl, sizeof bl);
            printf("  %-8s %6.0f %12.0f %12s %12s   %s\n", tipo == 0 ? "esparso" : "denso",
                   v, e, bm, bl, m < l ? "matriz" : "lista");
        }
    }
    printf("\n  Esparso: a matriz cresce com V² (10x mais vértices = 100x mais memória);\n");
    printf("  a lista cresce com V + E (10x mais vértices = 10x mais memória).\n");
    printf("  Denso  : E já é proporcional a V², e a lista paga 16 bytes por entrada contra\n");
    printf("  1 byte da matriz -> a matriz vence.\n\n");
    pausar();
}

/* ==========================================================================
 * PARTE 5 — CUSTO MEDIDO (contagem de acessos em grafos gerados)
 * ==========================================================================
 */
static unsigned long semente = 2026;

/* Gerador congruencial linear: deixa a tabela igual em toda execução. */
static unsigned long sorteio(void) {
    semente = semente * 1103515245UL + 12345UL;
    return (semente >> 16) & 0x7fffUL;
}

/* Esparso: anel (i, i+1) + corda (i, i+V/3) -> grau ~4, E ~ 2V. O(V). */
static void gera_esparso(Grafo *g) {
    for (int i = 0; i < g->v; i++) {
        insere(g, i, (i + 1) % g->v);
        insere(g, i, (i + g->v / 3) % g->v);
    }
}

/* Denso: cada par entra com 50% de chance. O(V²). */
static void gera_denso(Grafo *g) {
    for (int u = 0; u < g->v; u++) {
        for (int v = u + 1; v < g->v; v++) {
            if (sorteio() % 2) insere(g, u, v);
        }
    }
}

static void parte5_custo_medido(void) {
    const int tamanhos[] = { 10, 100, 1000 };
    const int consultas = 1000;
    printf("=== PARTE 5: acessos contados de verdade ===\n\n");
    printf("  'todos os vizinhos' = listar os vizinhos de todos os vértices (núcleo de BFS/DFS)\n");
    printf("  'existe?'           = %d consultas u-v sorteadas\n\n", consultas);
    printf("  %-8s %5s %7s | %-21s | %-21s\n", "", "", "", "todos os vizinhos", "existe? (1000x)");
    printf("  %-8s %5s %7s | %10s %10s | %10s %10s\n", "tipo", "V", "E",
           "matriz", "lista", "matriz", "lista");
    printf("  ------------------------------------------------------------------------\n");
    for (int tipo = 0; tipo < 2; tipo++) {
        for (int i = 0; i < 3; i++) {
            Grafo g = cria(tamanhos[i]);
            if (tipo == 0) gera_esparso(&g); else gera_denso(&g);

            long viz_m = 0, viz_l = 0, ex_m = 0, ex_l = 0;
            for (int u = 0; u < g.v; u++) {
                for (int v = 0; v < g.v; v++) {   /* matriz: linha inteira, O(V) */
                    viz_m++;
                    (void)g.mat[(size_t)u * g.v + v];
                }
                for (const No *a = g.lista[u]; a != NULL; a = a->prox) viz_l++;  /* O(grau) */
            }
            for (int q = 0; q < consultas; q++) {
                int u = (int)(sorteio() % (unsigned long)g.v);
                int v = (int)(sorteio() % (unsigned long)g.v);
                existe_matriz(&g, u, v, &ex_m);
                existe_lista(&g, u, v, &ex_l);
            }
            printf("  %-8s %5d %7d | %10ld %10ld | %10ld %10ld\n", tipo == 0 ? "esparso" : "denso",
                   g.v, g.e, viz_m, viz_l, ex_m, ex_l);
            libera(&g);
        }
    }
    printf("\n  Leitura da tabela:\n");
    printf("   - 'todos os vizinhos': matriz = V² sempre; lista = 2E. Esparso V=1000:\n");
    printf("     1 milhão contra 4 mil acessos -> BFS/DFS ficam O(V+E) só com lista.\n");
    printf("   - 'existe?': matriz = 1 acesso por consulta; lista ~ grau médio. No denso\n");
    printf("     com V=1000 cada consulta caminha centenas de nós.\n");
    printf("   - Regra prática: esparso ou 'percorrer' -> lista; pequeno, denso ou\n");
    printf("     'existe aresta?' muito frequente -> matriz.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    Grafo celula = cria(NV);
    parte1_montagem(&celula);
    parte2_consultas(&celula);
    parte3_memoria(&celula);
    libera(&celula);

    parte4_tabela_memoria();
    parte5_custo_medido();
    return 0;
}
