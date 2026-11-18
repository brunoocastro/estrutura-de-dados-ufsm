/*
 * Tarefa Avaliada 06 - Mapa rodoviário da região central do RS
 *
 * O mapa é modelado como grafo não dirigido e ponderado: vértices são cidades
 * e trevos; arestas são trechos de estrada com peso igual à distância em km.
 * O algoritmo de Dijkstra calcula a menor distância e a rota entre cidades.
 *
 * O trevo entre Santa Maria e Recanto Maestro, onde sai a estrada para
 * Silveira Martins, não tem nome no mapa; aqui ele se chama "Trevo SM".
 *
 * Uso:
 *   ./mapa                      rotas de Santa Maria para todas as cidades
 *   ./mapa "Origem" "Destino"   menor rota entre duas cidades
 */
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

enum {
    SANTA_MARIA, TREVO_SM, SILVEIRA_MARTINS, IVORA, FAXINAL, NOVA_PALMA,
    DONA_FRANCISCA, AGUDO, SAO_JOAO, RECANTO_MAESTRO, SAO_MIGUEL, MOSQUITO,
    RESTINGA_SECA, N
};

static const char *nome[N] = {
    "Santa Maria", "Trevo SM", "Silveira Martins", "Ivora", "Faxinal do Soturno",
    "Nova Palma", "Dona Francisca", "Agudo", "Sao Joao do Polesine",
    "Recanto Maestro", "Sao Miguel", "Mosquito", "Restinga Seca",
};

#define SEM_ARESTA 0
#define INFINITO INT_MAX

/* Matriz de adjacência: distancia[u][v] em km, 0 = sem estrada direta. */
static int distancia[N][N];

static void estrada(int u, int v, int km) {
    distancia[u][v] = km;
    distancia[v][u] = km;
}

static void monta_mapa(void) {
    estrada(SANTA_MARIA, TREVO_SM, 30);
    estrada(TREVO_SM, SILVEIRA_MARTINS, 13);
    estrada(TREVO_SM, RECANTO_MAESTRO, 6);
    estrada(SILVEIRA_MARTINS, IVORA, 21);
    estrada(IVORA, FAXINAL, 18);
    estrada(FAXINAL, NOVA_PALMA, 14);
    estrada(FAXINAL, DONA_FRANCISCA, 10);
    estrada(FAXINAL, SAO_JOAO, 5);
    estrada(DONA_FRANCISCA, AGUDO, 10);
    estrada(AGUDO, MOSQUITO, 8);
    estrada(SAO_JOAO, RECANTO_MAESTRO, 14);
    estrada(RECANTO_MAESTRO, SAO_MIGUEL, 19);
    estrada(SAO_MIGUEL, MOSQUITO, 5);
    estrada(SAO_MIGUEL, RESTINGA_SECA, 13);
}

/*
 * Dijkstra: a cada passo fixa o vértice não visitado de menor distância e
 * relaxa as arestas que saem dele. Com matriz de adjacência custa O(n²).
 */
static void dijkstra(int origem, int dist[N], int anterior[N]) {
    bool fixado[N] = {false};
    for (int v = 0; v < N; v++) { dist[v] = INFINITO; anterior[v] = -1; }
    dist[origem] = 0;

    for (int passo = 0; passo < N; passo++) {
        int u = -1;
        for (int v = 0; v < N; v++) {
            if (!fixado[v] && dist[v] != INFINITO && (u < 0 || dist[v] < dist[u])) u = v;
        }
        if (u < 0) break;   /* restantes são inalcançáveis */
        fixado[u] = true;
        for (int v = 0; v < N; v++) {
            int km = distancia[u][v];
            if (km != SEM_ARESTA && !fixado[v] && dist[u] + km < dist[v]) {
                dist[v] = dist[u] + km;
                anterior[v] = u;
            }
        }
    }
}

static void imprime_rota(int destino, const int anterior[N]) {
    int rota[N], k = 0;
    for (int v = destino; v != -1; v = anterior[v]) rota[k++] = v;
    while (k > 0) {
        k--;
        printf("%s%s", nome[rota[k]], k > 0 ? " -> " : "");
    }
}

static int busca_cidade(const char *s) {
    for (int v = 0; v < N; v++) {
        if (strcmp(nome[v], s) == 0) return v;
    }
    return -1;
}

static void lista_cidades(void) {
    fprintf(stderr, "Cidades disponiveis:\n");
    for (int v = 0; v < N; v++) fprintf(stderr, "  %s\n", nome[v]);
}

int main(int argc, char *argv[]) {
    int dist[N], anterior[N];
    monta_mapa();

    if (argc == 3) {
        int origem = busca_cidade(argv[1]), destino = busca_cidade(argv[2]);
        if (origem < 0 || destino < 0) {
            fprintf(stderr, "Cidade desconhecida.\n");
            lista_cidades();
            return 1;
        }
        dijkstra(origem, dist, anterior);
        printf("%s -> %s: %d km\n  ", argv[1], argv[2], dist[destino]);
        imprime_rota(destino, anterior);
        printf("\n");
        return 0;
    }
    if (argc != 1) {
        fprintf(stderr, "Uso: %s [\"Origem\" \"Destino\"]\n", argv[0]);
        return 1;
    }

    dijkstra(SANTA_MARIA, dist, anterior);
    printf("Menores distancias a partir de Santa Maria:\n");
    for (int v = 0; v < N; v++) {
        if (v == SANTA_MARIA || v == TREVO_SM) continue;
        printf("  %-22s %3d km  ", nome[v], dist[v]);
        imprime_rota(v, anterior);
        printf("\n");
    }
    return 0;
}
