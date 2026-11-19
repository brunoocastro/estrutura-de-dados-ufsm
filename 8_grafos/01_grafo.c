/*
 * Grafo simples com lista de adjacência.
 *
 * Vértices são numerados de 0 a n-1. Cada vértice guarda uma lista encadeada
 * com os vizinhos. O grafo pode ser dirigido ou não dirigido; no não
 * dirigido, cada aresta u-v é guardada nas duas listas.
 *
 * Operações: inserir/remover/consultar aresta, grau, busca em largura (BFS),
 * busca em profundidade (DFS), caminho mínimo em número de arestas e
 * detecção de ciclo.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Adj {
    int vertice;
    struct Adj *proximo;
} Adj;

typedef struct {
    int n;
    bool dirigido;
    Adj **lista;   /* lista[v] = primeiro vizinho de v */
} Grafo;

static void *aloca(size_t qtd, size_t tamanho) {
    void *p = calloc(qtd, tamanho);
    if (p == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

Grafo cria_grafo(int n, bool dirigido) {
    Grafo g = { n, dirigido, aloca((size_t)n, sizeof(Adj *)) };
    return g;
}

static bool valido(const Grafo *g, int v) { return v >= 0 && v < g->n; }

/* Percorre a lista encadeada de `u` para verificar se `v` já é vizinho. */
bool existe_aresta(const Grafo *g, int u, int v) {
    if (!valido(g, u) || !valido(g, v)) return false;
    for (const Adj *a = g->lista[u]; a != NULL; a = a->proximo) {
        if (a->vertice == v) return true;
    }
    return false;
}

/* Insere o vizinho `v` no início da lista encadeada em O(1). */
static void empilha_adj(Adj **lista, int v) {
    Adj *novo = aloca(1, sizeof *novo);
    novo->vertice = v;
    novo->proximo = *lista; /* O novo nó aponta para o antigo primeiro da lista */
    *lista = novo;          /* O início da lista passa a apontar para o novo nó */
}

/* Grafo simples: rejeita laços (u == v) e arestas repetidas. */
bool insere_aresta(Grafo *g, int u, int v) {
    if (!valido(g, u) || !valido(g, v) || u == v || existe_aresta(g, u, v)) return false;
    empilha_adj(&g->lista[u], v); /* Cria a ligação u -> v */
    if (!g->dirigido) {
        /* Se o grafo é não dirigido (mão dupla), também cria a volta v -> u */
        empilha_adj(&g->lista[v], u);
    }
    return true;
}

/*
 * Remove o vértice `v` da lista encadeada.
 * Usa ponteiro para ponteiro (`Adj **lista`) para religar diretamente o
 * ponteiro anterior sem precisar guardar variável `anterior` separada.
 */
static bool remove_adj(Adj **lista, int v) {
    for (; *lista != NULL; lista = &(*lista)->proximo) {
        if ((*lista)->vertice == v) {
            Adj *alvo = *lista;     /* Guarda o nó que será removido       */
            *lista = alvo->proximo; /* Pula o nó removido na lista         */
            free(alvo);             /* Devolve a memória do nó para o heap */
            return true;
        }
    }
    return false;
}

bool remove_aresta(Grafo *g, int u, int v) {
    if (!valido(g, u) || !valido(g, v) || !remove_adj(&g->lista[u], v)) return false;
    if (!g->dirigido) {
        /* Em grafo não dirigido, remove também a entrada espelhada em `lista[v]` */
        remove_adj(&g->lista[v], u);
    }
    return true;
}

/* Grau = quantidade de arestas ligadas ao vértice `v`. */
int grau(const Grafo *g, int v) {
    int d = 0;
    for (const Adj *a = g->lista[v]; a != NULL; a = a->proximo) d++;
    return d;
}

void imprime(const Grafo *g) {
    for (int v = 0; v < g->n; v++) {
        printf("  %d:", v);
        for (const Adj *a = g->lista[v]; a != NULL; a = a->proximo) printf(" %d", a->vertice);
        printf("\n");
    }
}

/*
 * Busca em Largura (BFS): visita por camadas (distância 0, 1, 2...) usando FILA.
 * Preenche:
 *   - dist[v]: menor número de arestas da origem até v (-1 = não visitado)
 *   - pai[v]:  por qual vértice chegamos a v (permite reconstruir o caminho)
 */
void bfs(const Grafo *g, int origem, int dist[], int pai[], bool imprimir) {
    int *fila = aloca((size_t)g->n, sizeof(int));
    int ini = 0, fim = 0;
    /* Inicializa todos com dist = -1 (funciona como visitado[v] == false) */
    for (int v = 0; v < g->n; v++) { dist[v] = -1; pai[v] = -1; }

    dist[origem] = 0;      /* A origem está a 0 arestas dela mesma */
    fila[fim++] = origem;  /* Enfileira a origem para iniciar as ondas */

    while (ini < fim) {
        int u = fila[ini++]; /* Retira o vértice mais antigo da fila (FIFO) */
        if (imprimir) printf(" %d", u);

        /* Inspeciona todos os vizinhos de `u` */
        for (const Adj *a = g->lista[u]; a != NULL; a = a->proximo) {
            /* Só visita se `dist == -1` (evita entrar em loop infinito nos ciclos!) */
            if (dist[a->vertice] == -1) {
                dist[a->vertice] = dist[u] + 1; /* Camada seguinte (+1 aresta) */
                pai[a->vertice] = u;            /* Registra quem descobriu este vértice */
                fila[fim++] = a->vertice;       /* Enfileira para expandir seus vizinhos depois */
            }
        }
    }
    free(fila);
}

/*
 * Busca em Profundidade (DFS): mergulha recursivamente o mais fundo possível
 * antes de retroceder (backtracking). Obrigatório marcar `visitado[u] = true`!
 */
static void dfs_rec(const Grafo *g, int u, bool visitado[]) {
    visitado[u] = true; /* Marca imediatamente ao entrar para evitar loop em ciclos */
    printf(" %d", u);
    for (const Adj *a = g->lista[u]; a != NULL; a = a->proximo) {
        if (!visitado[a->vertice]) {
            dfs_rec(g, a->vertice, visitado); /* Mergulha no primeiro vizinho não visitado */
        }
    }
}

void dfs(const Grafo *g, int origem) {
    bool *visitado = aloca((size_t)g->n, sizeof(bool)); /* calloc já zera para false */
    dfs_rec(g, origem, visitado);
    free(visitado);
}

void imprime_caminho(const Grafo *g, int origem, int destino) {
    int *dist = aloca((size_t)g->n, sizeof(int));
    int *pai = aloca((size_t)g->n, sizeof(int));
    bfs(g, origem, dist, pai, false); /* Calcula distâncias e pais a partir da origem */
    printf("Caminho %d -> %d: ", origem, destino);
    if (dist[destino] == -1) {
        printf("inexistente\n");
    } else {
        /* Segue os ponteiros `pai[]` do destino até a origem e imprime invertido */
        int *caminho = aloca((size_t)g->n, sizeof(int));
        int k = 0;
        for (int v = destino; v != -1; v = pai[v]) caminho[k++] = v;
        while (k > 0) {
            k--;
            printf("%d%s", caminho[k], k > 0 ? " - " : "");
        }
        printf(" (%d arestas)\n", dist[destino]);
        free(caminho);
    }
    free(dist);
    free(pai);
}

/*
 * Detecção de ciclo em grafo não dirigido usando DFS:
 * Se a busca encontrar um vizinho `a->vertice` que JÁ FOI VISITADO e que
 * NÃO É o próprio `pai` imediato de onde acabamos de vir, existe um ciclo!
 */
static bool ciclo_rec(const Grafo *g, int u, int pai, bool visitado[]) {
    visitado[u] = true;
    for (const Adj *a = g->lista[u]; a != NULL; a = a->proximo) {
        if (!visitado[a->vertice]) {
            if (ciclo_rec(g, a->vertice, u, visitado)) return true;
        } else if (a->vertice != pai) {
            /* Achou aresta de retorno para um vértice antigo: fechou um ciclo! */
            return true;
        }
    }
    return false;
}

bool tem_ciclo(const Grafo *g) {
    bool *visitado = aloca((size_t)g->n, sizeof(bool));
    bool achou = false;
    /* Testa todos os vértices para cobrir também componentes desconexas */
    for (int v = 0; v < g->n && !achou; v++) {
        if (!visitado[v]) achou = ciclo_rec(g, v, -1, visitado);
    }
    free(visitado);
    return achou;
}

void libera_grafo(Grafo *g) {
    for (int v = 0; v < g->n; v++) {
        /* Esvazia cada lista encadeada de vizinhos antes de liberar o vetor */
        while (g->lista[v] != NULL) remove_adj(&g->lista[v], g->lista[v]->vertice);
    }
    free(g->lista);
    g->lista = NULL;
}

int main(void) {
    /*
     *   0 --- 1 --- 4
     *   |     |
     *   2 --- 3     5 --- 6
     */
    Grafo g = cria_grafo(7, false);
    int arestas[][2] = { {0, 1}, {0, 2}, {1, 3}, {2, 3}, {1, 4}, {5, 6} };
    for (size_t i = 0; i < sizeof arestas / sizeof arestas[0]; i++) {
        insere_aresta(&g, arestas[i][0], arestas[i][1]);
    }
    printf("Insere 0-1 de novo: %s\n", insere_aresta(&g, 0, 1) ? "inserida" : "rejeitada (repetida)");
    printf("Insere 2-2: %s\n", insere_aresta(&g, 2, 2) ? "inserida" : "rejeitada (laco)");

    printf("\nLista de adjacencia (nao dirigido):\n");
    imprime(&g);
    printf("Grau de 1: %d\n", grau(&g, 1));

    int dist[7], pai[7];
    printf("BFS a partir de 0:");
    bfs(&g, 0, dist, pai, true);
    printf("\nDFS a partir de 0:");
    dfs(&g, 0);
    printf("\n");
    imprime_caminho(&g, 2, 4);
    imprime_caminho(&g, 0, 6);
    printf("Tem ciclo? %s\n", tem_ciclo(&g) ? "sim (0-1-3-2-0)" : "nao");

    remove_aresta(&g, 2, 3);
    printf("\nApos remover 2-3, tem ciclo? %s\n", tem_ciclo(&g) ? "sim" : "nao");
    libera_grafo(&g);

    /* Grafo dirigido: a aresta só vale em um sentido. */
    Grafo d = cria_grafo(3, true);
    insere_aresta(&d, 0, 1);
    insere_aresta(&d, 1, 2);
    printf("\nDirigido 0->1->2:\n");
    imprime(&d);
    imprime_caminho(&d, 0, 2);
    imprime_caminho(&d, 2, 0);
    libera_grafo(&d);
    return 0;
}
