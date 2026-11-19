/*
 * ============================================================================
 * Arquivo : 09_automacao_agv_profinet.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidade 6.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Aplicação integrada: a topologia de um chão de fábrica como GRAFO
 *   PONDERADO (matriz de adjacência) e o que acontece quando um enlace cai.
 *   Por que grafo e não árvore? Em redes industriais críticas e em mapas de
 *   chão de fábrica existem CICLOS (anéis e malhas) projetados de propósito
 *   para dar REDUNDÂNCIA: se um cabo Ethernet rompe ou uma empilhadeira
 *   bloqueia um corredor, existe um caminho alternativo. Numa árvore, remover
 *   uma aresta sempre desconecta a rede.
 *
 * EXEMPLO REAL
 *   Parte 1 (rede PROFINET em anel, MRP — BFS): quatro CLPs em anel
 *     CLP1 - CLP2 - CLP3 - CLP4 - CLP1. Quando o cabo CLP1-CLP2 rompe, a BFS
 *     encontra a rota pelo outro lado do anel: CLP1 -> CLP4 -> CLP3 -> CLP2.
 *   Parte 2 (robô móvel AGV — Dijkstra): levar um palete da Doca até a Célula
 *     de Montagem no menor tempo (s). Com o Corredor Norte interditado,
 *     Dijkstra compara os desvios pelo Estoque (35 s) e pelo Pátio Sul (43 s)
 *     e escolhe o mais rápido.
 *   Versões passo a passo: 04_bfs_saltos_rede.c e 08_dijkstra_latencia_rede.c.
 *
 * COMPLEXIDADE (V = pontos da fábrica; matriz de adjacência V x V)
 *   Operação            Tempo    Por quê
 *   conecta/desconecta  O(1)     escreve m[u][v] e m[v][u]
 *   BFS (saltos)        O(V²)    cada vértice que sai da fila varre sua linha
 *   Dijkstra (vetor)    O(V²)    V iterações x busca linear do mínimo
 *   Espaço: O(V²) para a matriz; O(V) para fila, dist e anterior.
 *
 * COMO EXECUTAR
 *   ./run 09_automacao_agv_profinet
 * ============================================================================
 */
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#define N 6
#define SEM_CONEXAO 0
#define INFINITO INT_MAX

enum PontoFabrica {
    DOCA_CLP1,
    CORREDOR_NORTE_CLP2,
    CELULA_MONTAGEM_CLP3,
    ESTOQUE_CLP4,
    PATIO_SUL,
    CARREGADOR_AGV
};

static const char *nome_ponto[N] = {
    "Doca (CLP-01)",
    "Corredor Norte (CLP-02)",
    "Celula Montagem (CLP-03)",
    "Estoque (CLP-04)",
    "Patio Sul",
    "Estacao Recarga"
};

/*
 * Matriz de adjacência ponderada:
 *   tempo[u][v] = tempo de deslocamento em segundos (ou 0 se não há ligação).
 */
static int tempo[N][N];

static void conecta(int u, int v, int segundos) {
    tempo[u][v] = segundos;
    tempo[v][u] = segundos; /* Mão dupla (grafo não dirigido) */
}

static void desconecta(int u, int v) {
    tempo[u][v] = SEM_CONEXAO;
    tempo[v][u] = SEM_CONEXAO;
}

/*
 * BUSCA EM LARGURA (BFS): encontra o caminho com MENOR NÚMERO DE SALTOS
 * (switches/CLPs intermediários) na rede PROFINET.
 *
 * ATENÇÃO: Como o grafo tem ciclos (anel), o vetor `visitado[]` (aqui
 * representado por `saltos[v] != -1`) é OBRIGATÓRIO para não entrar em
 * loop infinito!
 */
static void bfs_rede_profinet(int origem, int destino) {
    int fila[N], ini = 0, fim = 0;
    int saltos[N], anterior[N];

    for (int v = 0; v < N; v++) {
        saltos[v] = -1;   /* -1 significa "ainda não visitado" */
        anterior[v] = -1;
    }

    /* Marca a origem como visitada (distância 0) e coloca na fila */
    saltos[origem] = 0;
    fila[fim++] = origem;

    while (ini < fim) {
        int u = fila[ini++]; /* Retira o próximo vértice da fila (FIFO) */
        for (int v = 0; v < N; v++) {
            /* Se existe cabo ativo entre u e v e `v` ainda não foi visitado */
            if (tempo[u][v] != SEM_CONEXAO && saltos[v] == -1) {
                saltos[v] = saltos[u] + 1;
                anterior[v] = u;
                fila[fim++] = v;
            }
        }
    }

    printf("  Rota PROFINET (%s -> %s):\n    ", nome_ponto[origem], nome_ponto[destino]);
    if (saltos[destino] == -1) {
        printf("FALHA CRITICA: Destino incomunicavel!\n");
        return;
    }

    /* Reconstrói o caminho de trás para frente usando o vetor `anterior[]` */
    int rota[N], k = 0;
    for (int v = destino; v != -1; v = anterior[v]) {
        rota[k++] = v;
    }
    while (k > 0) {
        k--;
        printf("%s%s", nome_ponto[rota[k]], k > 0 ? " -> " : "");
    }
    printf(" (%d salto[s])\n", saltos[destino]);
}

/*
 * ALGORITMO DE DIJKSTRA: encontra a rota de MENOR TEMPO TOTAL (soma dos pesos)
 * para o robô móvel AGV atravessar a fábrica.
 */
static void dijkstra_rota_agv(int origem, int destino) {
    int dist[N], anterior[N];
    bool fixado[N] = {false}; /* Vértices cuja menor distância já é definitiva */

    for (int v = 0; v < N; v++) {
        dist[v] = INFINITO;
        anterior[v] = -1;
    }
    dist[origem] = 0;

    for (int passo = 0; passo < N; passo++) {
        /* 1. Escolhe o vértice ainda não fixado com o menor tempo acumulado */
        int u = -1;
        for (int v = 0; v < N; v++) {
            if (!fixado[v] && dist[v] != INFINITO && (u == -1 || dist[v] < dist[u])) {
                u = v;
            }
        }
        if (u == -1) break; /* Demais vértices são inalcançáveis */
        fixado[u] = true;

        /* 2. Relaxamento: testa se passar por `u` melhora o tempo até `v` */
        for (int v = 0; v < N; v++) {
            int custo_uv = tempo[u][v];
            if (custo_uv != SEM_CONEXAO && !fixado[v] && dist[u] + custo_uv < dist[v]) {
                dist[v] = dist[u] + custo_uv;
                anterior[v] = u;
            }
        }
    }

    printf("  Rota Otima do AGV (%s -> %s):\n    ", nome_ponto[origem], nome_ponto[destino]);
    if (dist[destino] == INFINITO) {
        printf("BLOQUEIO TOTAL: Sem rota disponivel!\n");
        return;
    }

    int rota[N], k = 0;
    for (int v = destino; v != -1; v = anterior[v]) {
        rota[k++] = v;
    }
    while (k > 0) {
        k--;
        printf("%s%s", nome_ponto[rota[k]], k > 0 ? " -> " : "");
    }
    printf(" [Tempo total: %d s]\n", dist[destino]);
}

int main(void) {
    /*
     * Topologia da planta (anel principal + corredores do pátio):
     *
     *   [Doca/CLP1] ----10s---- [Corredor Norte/CLP2] ----12s---- [Celula/CLP3]
     *        |                                                      /     |
     *       15s                                              20s  /       |
     *        |                                                  /         |
     *   [Estoque/CLP4] -------------------------------------------+       18s
     *        |                                                            |
     *       10s                                                           |
     *        |                                                            |
     *   [Patio Sul] ------------------------------------------------------+
     *        |
     *        8s
     *        |
     *   [Estacao Recarga]
     *
     * Anel PROFINET: Doca - Corredor Norte - Celula - Estoque - Doca.
     */
    conecta(DOCA_CLP1, CORREDOR_NORTE_CLP2, 10);
    conecta(CORREDOR_NORTE_CLP2, CELULA_MONTAGEM_CLP3, 12);
    conecta(CELULA_MONTAGEM_CLP3, ESTOQUE_CLP4, 20);
    conecta(ESTOQUE_CLP4, DOCA_CLP1, 15);
    conecta(ESTOQUE_CLP4, PATIO_SUL, 10);
    conecta(PATIO_SUL, CELULA_MONTAGEM_CLP3, 18);
    conecta(PATIO_SUL, CARREGADOR_AGV, 8);

    printf("=== CENARIO 1: OPERACAO NORMAL (SEM FALHAS) ===\n");
    bfs_rede_profinet(DOCA_CLP1, CORREDOR_NORTE_CLP2);
    dijkstra_rota_agv(DOCA_CLP1, CELULA_MONTAGEM_CLP3);

    printf("\n=== CENARIO 2: CABO ROMPIDO / CORREDOR NORTE INTERDITADO ===\n");
    printf("  [!] Evento: Queda do enlace Doca <-> Corredor Norte!\n\n");
    desconecta(DOCA_CLP1, CORREDOR_NORTE_CLP2);

    /*
     * Graças ao ciclo do Grafo, nem a rede PROFINET nem o robô AGV param:
     * - A BFS acha o caminho alternativo pelo outro lado do anel (3 saltos).
     * - O Dijkstra compara os tempos e recalcula a rota mais rápida!
     */
    bfs_rede_profinet(DOCA_CLP1, CORREDOR_NORTE_CLP2);
    dijkstra_rota_agv(DOCA_CLP1, CELULA_MONTAGEM_CLP3);

    return 0;
}
