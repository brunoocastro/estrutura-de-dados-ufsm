/*
 * ============================================================================
 * Arquivo : 03_formas_grafo.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidades 6.2 e 6.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Comparar, num único programa, as variações de implementação de um grafo:
 *   1. As 3 formas clássicas de ARMAZENAR o mesmo grafo ponderado:
 *      - Forma A: Matriz de Adjacência `m[V][V]`      -> Memória O(V²), consulta O(1)
 *      - Forma B: Lista de Adjacência `adj[V]`        -> Memória O(V + E), redes esparsas
 *      - Forma C: Lista/Vetor de Arestas `arestas[E]` -> Memória O(E), ideal p/ ordenar custos
 *   2. INSERIR e REMOVER, incluindo a REMOÇÃO DE UM VÉRTICE inteiro (desativa
 *      `v` e limpa todas as arestas que saem de `v` ou chegam em `v`).
 *   3. PERCORRER: DFS iterativa com pilha explícita (../05_pilhas/).
 *   4. ORDENAR em grafos:
 *      - arestas por peso crescente (primeiro passo do Kruskal,
 *        10_mst_cabeamento_sensores.c);
 *      - ordenação topológica em DAG pelo algoritmo de Kahn (versão passo a
 *        passo em 07_topologica_partida_maquinas.c).
 *
 * EXEMPLO REAL
 *   Etapas de um projeto de automação (especificação, compra do CLP, montagem
 *   do painel, programação, TAF, start-up) ordenadas por pré-requisito.
 *
 * COMPLEXIDADE (V = vértices, E = arestas)
 *   Operação               Matriz     Lista de adj.     Lista de arestas
 *   memória                O(V²)      O(V + E)          O(E)
 *   inserir aresta         O(1)       O(1)              O(1) no fim do vetor
 *   remover vértice        O(V)       O(V + E)          (não implementado)
 *   DFS                    O(V²)      O(V + E)          O(V·E)
 *   ordenar arestas        -          -                 O(E²) insertion sort
 *   Kahn (aqui com matriz) O(V²)      O(V + E)          -
 *   Por quê: remover um vértice da lista exige varrer as listas de TODOS os
 *   outros vértices atrás de arestas que chegam nele (V listas, E nós ao todo).
 *
 * COMO EXECUTAR
 *   ./run 03_formas_grafo
 * ============================================================================
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_V 6
#define MAX_E 16

/* ==========================================================================
 * 1. AS TRÊS REPRESENTAÇÕES DE ARMAZENAMENTO
 * ==========================================================================
 */

/* Forma A: Matriz de Adjacência */
typedef struct {
    int peso[MAX_V][MAX_V]; /* 0 = sem aresta; >0 = peso da aresta */
    bool ativo[MAX_V];      /* Permite remover/desativar um vértice inteiro */
    int n;
} GrafoMatriz;

/* Forma B: Lista de Adjacência */
typedef struct NoAdj {
    int destino;
    int peso;
    struct NoAdj *proximo;
} NoAdj;

typedef struct {
    NoAdj *head[MAX_V];
    bool ativo[MAX_V];
    int n;
} GrafoLista;

/* Forma C: Lista (Vetor) de Arestas */
typedef struct {
    int origem;
    int destino;
    int peso;
} Aresta;

typedef struct {
    Aresta itens[MAX_E];
    int qtd_arestas;
    int n_vertices;
} GrafoArestas;

/* Inicializa as 3 estruturas simultaneamente para comparação direta */
void inicializa_tres_formas(GrafoMatriz *gm, GrafoLista *gl, GrafoArestas *ga, int n) {
    gm->n = gl->n = ga->n_vertices = n;
    ga->qtd_arestas = 0;
    for (int i = 0; i < n; i++) {
        gm->ativo[i] = gl->ativo[i] = true;
        gl->head[i] = NULL;
        for (int j = 0; j < n; j++) gm->peso[i][j] = 0;
    }
}

/* Insere a mesma aresta não dirigida (u <-> v com `peso`) nas 3 representações */
void insere_nas_tres(GrafoMatriz *gm, GrafoLista *gl, GrafoArestas *ga,
                     int u, int v, int peso) {
    /* 1. Na Matriz: acesso direto O(1) nas duas posições simétricas */
    gm->peso[u][v] = peso;
    gm->peso[v][u] = peso;

    /* 2. Na Lista de Adjacência: aloca nós encadeados nas listas de `u` e `v` */
    NoAdj *nu = malloc(sizeof *nu);
    NoAdj *nv = malloc(sizeof *nv);
    if (!nu || !nv) exit(EXIT_FAILURE);
    nu->destino = v; nu->peso = peso; nu->proximo = gl->head[u]; gl->head[u] = nu;
    nv->destino = u; nv->peso = peso; nv->proximo = gl->head[v]; gl->head[v] = nv;

    /* 3. Na Lista de Arestas: acrescenta um registro {u, v, peso} no fim do vetor */
    ga->itens[ga->qtd_arestas++] = (Aresta){ u, v, peso };
}

void imprime_tres_formas(const GrafoMatriz *gm, const GrafoLista *gl, const GrafoArestas *ga) {
    printf("  [A] Matriz de Adjacencia (%dx%d):\n", gm->n, gm->n);
    for (int i = 0; i < gm->n; i++) {
        if (!gm->ativo[i]) continue;
        printf("      [%d]: ", i);
        for (int j = 0; j < gm->n; j++) {
            if (gm->ativo[j]) printf("%2d ", gm->peso[i][j]);
        }
        printf("\n");
    }

    printf("  [B] Lista de Adjacencia:\n");
    for (int i = 0; i < gl->n; i++) {
        if (!gl->ativo[i]) continue;
        printf("      [%d] ->", i);
        for (const NoAdj *a = gl->head[i]; a != NULL; a = a->proximo) {
            printf(" %d(peso %d)", a->destino, a->peso);
        }
        printf(" -> NULL\n");
    }

    printf("  [C] Lista de Arestas (%d arestas):\n      ", ga->qtd_arestas);
    for (int k = 0; k < ga->qtd_arestas; k++) {
        printf("(%d-%d: %d) ", ga->itens[k].origem, ga->itens[k].destino, ga->itens[k].peso);
    }
    printf("\n");
}

/* ==========================================================================
 * 2. REMOÇÃO COMPLETA DE UM VÉRTICE (E TODAS AS ARESTAS CONECTADAS A ELE)
 * ==========================================================================
 * Remover um vértice `alvo` é mais trabalhoso que remover uma aresta:
 *   Passo 1: Liberar toda a lista de arestas que SAEM de `alvo`;
 *   Passo 2: Percorrer todos os outros vértices e remover as arestas que
 *            CHEGAM em `alvo` (para não deixar ponteiros para um vértice morto!);
 *   Passo 3: Marcar `ativo[alvo] = false`.
 */
void remove_vertice_completo(GrafoMatriz *gm, GrafoLista *gl, int alvo) {
    /* Na Matriz: zera a linha e a coluna de `alvo` e desativa o vértice */
    gm->ativo[alvo] = false;
    for (int i = 0; i < gm->n; i++) {
        gm->peso[alvo][i] = 0;
        gm->peso[i][alvo] = 0;
    }

    /* Na Lista de Adjacência: */
    gl->ativo[alvo] = false;
    /* Passo 1: apaga todas as arestas que saem de `alvo` */
    while (gl->head[alvo] != NULL) {
        NoAdj *tmp = gl->head[alvo];
        gl->head[alvo] = tmp->proximo;
        free(tmp);
    }
    /* Passo 2: varre as listas dos demais vértices apagando conexões para `alvo` */
    for (int u = 0; u < gl->n; u++) {
        if (!gl->ativo[u]) continue;
        NoAdj **p = &gl->head[u];
        while (*p != NULL) {
            if ((*p)->destino == alvo) {
                NoAdj *removido = *p;
                *p = removido->proximo; /* Pula o nó que apontava para `alvo` */
                free(removido);
            } else {
                p = &(*p)->proximo;
            }
        }
    }
}

void libera_grafo_lista(GrafoLista *gl) {
    for (int i = 0; i < gl->n; i++) {
        while (gl->head[i] != NULL) {
            NoAdj *tmp = gl->head[i];
            gl->head[i] = tmp->proximo;
            free(tmp);
        }
    }
}

/* ==========================================================================
 * 3. DFS ITERATIVA COM PILHA EXPLÍCITA (CONECTANDO COM ../05_pilhas/)
 * ==========================================================================
 * Substitui a recursão por uma pilha explícita LIFO (`pilha[]` e `topo`):
 *   - Empilha a origem;
 *   - Enquanto a pilha não esvaziar, desempilha o topo `u`;
 *   - Se `u` ainda não foi visitado, marca como visitado, processa `u` e
 *     empilha seus vizinhos não visitados.
 */
void dfs_iterativa_com_pilha(const GrafoLista *gl, int origem) {
    bool visitado[MAX_V] = {false};
    int pilha[MAX_V * MAX_V];
    int topo = 0;

    pilha[topo++] = origem; /* Push inicial */
    printf("  DFS Iterativa (com Pilha explicita) a partir de %d:", origem);

    while (topo > 0) {
        int u = pilha[--topo]; /* Pop (LIFO: retira o último inserido) */
        if (visitado[u] || !gl->ativo[u]) continue;

        visitado[u] = true;
        printf(" %d", u);

        /* Empilha os vizinhos ainda não visitados */
        for (const NoAdj *a = gl->head[u]; a != NULL; a = a->proximo) {
            if (!visitado[a->destino] && gl->ativo[a->destino]) {
                pilha[topo++] = a->destino;
            }
        }
    }
    printf("\n");
}

/* ==========================================================================
 * 4. ORDENAÇÃO EM GRAFOS:
 *    4A) Ordenar Arestas por Peso Crescente
 *    4B) Ordenação Topológica em DAG (Pré-requisitos / Algoritmo de Kahn)
 * ==========================================================================
 */

/* 4A: Ordena o vetor de arestas da mais barata para a mais cara (Insertion Sort) */
void ordena_arestas_por_peso(GrafoArestas *ga) {
    for (int i = 1; i < ga->qtd_arestas; i++) {
        Aresta chave = ga->itens[i];
        int j = i - 1;
        while (j >= 0 && ga->itens[j].peso > chave.peso) {
            ga->itens[j + 1] = ga->itens[j];
            j--;
        }
        ga->itens[j + 1] = chave;
    }
}

/*
 * 4B: Ordenação Topológica (Algoritmo de Kahn com Fila):
 * Dado um Grafo Dirigido Acíclico (DAG) de dependências `u -> v`
 * ("para fazer `v`, antes é obrigatório concluir `u`"):
 *   1. Calcula o `grau_entrada[v]` de cada tarefa (quantos pré-requisitos faltam);
 *   2. Coloca na FILA todas as tarefas com `grau_entrada == 0` (já podem começar!);
 *   3. Retira `u` da fila, adiciona na ordem final e reduz em -1 o `grau_entrada`
 *      de todas as tarefas `v` que dependiam de `u`. Se alguma zerar, entra na fila!
 */
void ordenacao_topologica_dag(void) {
    const char *tarefa[MAX_V] = {
        "0: Especificacao Eletrica",
        "1: Compra do CLP e Sensores",
        "2: Montagem do Painel",
        "3: Programacao Ladder/SFC",
        "4: Teste em Bancada (TAF)",
        "5: Partida na Planta (Start-up)"
    };
    /* Matriz dirigida: dep[u][v] = 1 significa que `u` é pré-requisito de `v` */
    int dep[MAX_V][MAX_V] = {0};
    int grau_entrada[MAX_V] = {0};

    /* Definindo as dependências do projeto de automação: */
    int pre_requisitos[][2] = {
        {0, 1}, /* Especificação -> Compra do CLP       */
        {0, 3}, /* Especificação -> Programação Ladder  */
        {1, 2}, /* Compra do CLP -> Montagem do Painel  */
        {2, 4}, /* Montagem      -> Teste em Bancada    */
        {3, 4}, /* Programação   -> Teste em Bancada    */
        {4, 5}  /* Teste Bancada -> Partida na Planta   */
    };

    for (size_t k = 0; k < sizeof pre_requisitos / sizeof pre_requisitos[0]; k++) {
        int u = pre_requisitos[k][0];
        int v = pre_requisitos[k][1];
        dep[u][v] = 1;
        grau_entrada[v]++; /* `v` tem mais um pré-requisito pendente */
    }

    /* Enfileira todas as tarefas que não dependem de nenhuma anterior (grau 0) */
    int fila[MAX_V], ini = 0, fim = 0;
    for (int v = 0; v < MAX_V; v++) {
        if (grau_entrada[v] == 0) fila[fim++] = v;
    }

    printf("  Ordem Topologica valida de execucao das etapas (DAG):\n");
    int concluidas = 0;
    while (ini < fim) {
        int u = fila[ini++];
        printf("    %dº passo -> [%s]\n", ++concluidas, tarefa[u]);

        /* Libera as tarefas que aguardavam a conclusão de `u` */
        for (int v = 0; v < MAX_V; v++) {
            if (dep[u][v]) {
                grau_entrada[v]--;
                if (grau_entrada[v] == 0) {
                    /* Todos os pré-requisitos de `v` foram cumpridos! */
                    fila[fim++] = v;
                }
            }
        }
    }
}

int main(void) {
    GrafoMatriz gm;
    GrafoLista gl;
    GrafoArestas ga;

    inicializa_tres_formas(&gm, &gl, &ga, 4);
    insere_nas_tres(&gm, &gl, &ga, 0, 1, 25);
    insere_nas_tres(&gm, &gl, &ga, 0, 2, 10);
    insere_nas_tres(&gm, &gl, &ga, 1, 2, 5);
    insere_nas_tres(&gm, &gl, &ga, 1, 3, 15);

    printf("=== 1. AS 3 FORMAS DE ARMAZENAR O MESMO GRAFO ===\n");
    imprime_tres_formas(&gm, &gl, &ga);

    printf("\n=== 2. ORDENACAO DE ARESTAS POR PESO CRESCENTE ===\n");
    ordena_arestas_por_peso(&ga);
    printf("  Arestas ordenadas da mais leve para a mais pesada:\n    ");
    for (int k = 0; k < ga.qtd_arestas; k++) {
        printf("(%d-%d: peso %d) ", ga.itens[k].origem, ga.itens[k].destino, ga.itens[k].peso);
    }
    printf("\n");

    printf("\n=== 3. PERCURSO DFS ITERATIVO COM PILHA EXPLICITA ===\n");
    dfs_iterativa_com_pilha(&gl, 0);

    printf("\n=== 4. REMOCAO COMPLETA DO VERTICE [2] (LIMPANDO TODAS AS ARESTAS) ===\n");
    remove_vertice_completo(&gm, &gl, 2);
    imprime_tres_formas(&gm, &gl, &ga);

    printf("\n=== 5. ORDENACAO TOPOLOGICA EM GRAFO DIRIGIDO ACICLICO (DAG) ===\n");
    ordenacao_topologica_dag();

    libera_grafo_lista(&gl);
    return 0;
}
