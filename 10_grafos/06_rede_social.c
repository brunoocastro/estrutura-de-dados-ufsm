/*
 * ============================================================================
 * Arquivo : 06_rede_social.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidade 6.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Mostrar que cada pergunta de uma rede social é uma operação de grafo.
 *   Vértice = pessoa; aresta = amizade (sempre mútua, grafo não dirigido).
 *   A lista de adjacência guarda os amigos de cada pessoa e responde:
 *     - amigos de uma pessoa (vizinhos) e quantidade (grau);
 *     - amigos em comum (interseção de vizinhanças);
 *     - sugestões de amizade (amigos de amigos, ordenados por amigos em comum);
 *     - grau de separação e a cadeia de amizades (BFS);
 *     - grupos isolados (componentes conexas);
 *     - pessoa mais popular (maior grau).
 *
 * EXEMPLO REAL
 *   "Pessoas que você talvez conheça" (LinkedIn, Instagram), graus de
 *   separação; o mesmo raciocínio vale para "quais equipamentos dependem de
 *   quais" numa planta.
 *
 * COMPLEXIDADE (V = pessoas, E = amizades, g = grau médio)
 *   Operação              Tempo           Por quê
 *   indice (nome -> nº)   O(V)            busca sequencial com strcmp
 *   sao_amigos            O(g)            percorre a lista de amigos
 *   amigos em comum       O(g²)           para cada amigo de a, testa se é amigo de b
 *   sugestões             O(g³ + V²)      amigos de amigos (g·g·g) + seleção repetida
 *   grau de separação     O(V + E)        uma BFS
 *   grupos (componentes)  O(V·(V + E))    uma BFS por grupo, varrendo V a cada uma
 *   Melhorias possíveis: tabela hash para nomes (O(1)); marcar visitados num
 *   único vetor para os grupos saírem em O(V + E) (como em 05_dfs_ciclos.c).
 *
 * COMO EXECUTAR
 *   ./run 06_rede_social
 * ============================================================================
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PESSOAS 32

typedef struct Amizade {
    int amigo;
    struct Amizade *proxima;
} Amizade;

typedef struct {
    const char *nome[MAX_PESSOAS];
    Amizade *amigos[MAX_PESSOAS];
    int n;
} Rede;

int adiciona_pessoa(Rede *r, const char *nome) {
    if (r->n == MAX_PESSOAS) return -1;
    r->nome[r->n] = nome;
    r->amigos[r->n] = NULL;
    return r->n++;
}

int indice(const Rede *r, const char *nome) {
    for (int i = 0; i < r->n; i++) {
        if (strcmp(r->nome[i], nome) == 0) return i;
    }
    return -1;
}

bool sao_amigos(const Rede *r, int a, int b) {
    for (const Amizade *x = r->amigos[a]; x != NULL; x = x->proxima) {
        if (x->amigo == b) return true;
    }
    return false;
}

static void liga(Rede *r, int de, int para) {
    Amizade *nova = malloc(sizeof *nova);
    if (nova == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    nova->amigo = para;
    nova->proxima = r->amigos[de];
    r->amigos[de] = nova;
}

bool adiciona_amizade(Rede *r, const char *a, const char *b) {
    int i = indice(r, a), j = indice(r, b);
    if (i < 0 || j < 0 || i == j || sao_amigos(r, i, j)) return false;
    liga(r, i, j);   /* não dirigido: guarda nos dois sentidos */
    liga(r, j, i);
    return true;
}

int num_amigos(const Rede *r, int p) {
    int n = 0;
    for (const Amizade *x = r->amigos[p]; x != NULL; x = x->proxima) n++;
    return n;
}

/* indice() com aviso: nome digitado errado não pode virar r->amigos[-1]. */
static int indice_ou_avisa(const Rede *r, const char *nome) {
    int p = indice(r, nome);
    if (p < 0) printf("%s não está na rede\n", nome);
    return p;
}

void mostra_amigos(const Rede *r, const char *nome) {
    int p = indice_ou_avisa(r, nome);
    if (p < 0) return;
    printf("Amigos de %s (%d):", nome, num_amigos(r, p));
    for (const Amizade *x = r->amigos[p]; x != NULL; x = x->proxima) {
        printf(" %s", r->nome[x->amigo]);
    }
    printf("\n");
}

int amigos_em_comum(const Rede *r, int a, int b, bool imprimir) {
    int total = 0;
    for (const Amizade *x = r->amigos[a]; x != NULL; x = x->proxima) {
        if (sao_amigos(r, b, x->amigo)) {
            if (imprimir) printf(" %s", r->nome[x->amigo]);
            total++;
        }
    }
    return total;
}

void mostra_em_comum(const Rede *r, const char *a, const char *b) {
    int i = indice_ou_avisa(r, a), j = indice_ou_avisa(r, b);
    if (i < 0 || j < 0) return;
    printf("Amigos em comum entre %s e %s:", a, b);
    if (amigos_em_comum(r, i, j, true) == 0) printf(" nenhum");
    printf("\n");
}

/*
 * Sugestões de amizade ("Pessoas que você talvez conheça"):
 * Percorre os amigos de `p` (distância 1) e depois os amigos desses amigos
 * (distância 2), contando quantos amigos em comum cada candidato possui.
 */
void sugere_amizades(const Rede *r, const char *nome) {
    int p = indice_ou_avisa(r, nome);
    if (p < 0) return;
    int comuns[MAX_PESSOAS] = {0};
    for (const Amizade *x = r->amigos[p]; x != NULL; x = x->proxima) {
        for (const Amizade *y = r->amigos[x->amigo]; y != NULL; y = y->proxima) {
            /* Ignora a própria pessoa `p` e quem já é amigo direto de `p` */
            if (y->amigo != p && !sao_amigos(r, p, y->amigo)) {
                comuns[y->amigo]++;
            }
        }
    }
    printf("Sugestoes para %s:", nome);
    bool alguma = false;
    for (;;) {   /* Seleciona repetidamente o candidato com mais amigos em comum */
        int melhor = -1;
        for (int i = 0; i < r->n; i++) {
            if (comuns[i] > 0 && (melhor < 0 || comuns[i] > comuns[melhor])) melhor = i;
        }
        if (melhor < 0) break;
        printf(" %s (%d em comum)", r->nome[melhor], comuns[melhor]);
        comuns[melhor] = 0; /* Zera para não imprimir o mesmo candidato novamente */
        alguma = true;
    }
    if (!alguma) printf(" nenhuma");
    printf("\n");
}

/*
 * BFS a partir de `origem`:
 *   - dist[i] guarda o grau de separação (menor nº de amizades até `i`)
 *   - pai[i] guarda quem apresentou quem no caminho mais curto
 */
static void bfs(const Rede *r, int origem, int dist[], int pai[]) {
    int fila[MAX_PESSOAS], ini = 0, fim = 0;
    for (int i = 0; i < r->n; i++) { dist[i] = -1; pai[i] = -1; }
    dist[origem] = 0;
    fila[fim++] = origem;
    while (ini < fim) {
        int u = fila[ini++];
        for (const Amizade *x = r->amigos[u]; x != NULL; x = x->proxima) {
            /* Só enfileira se `dist < 0` (ainda não visitado) para não travar nos ciclos */
            if (dist[x->amigo] < 0) {
                dist[x->amigo] = dist[u] + 1;
                pai[x->amigo] = u;
                fila[fim++] = x->amigo;
            }
        }
    }
}

void grau_de_separacao(const Rede *r, const char *a, const char *b) {
    int dist[MAX_PESSOAS], pai[MAX_PESSOAS], cadeia[MAX_PESSOAS], k = 0;
    int i = indice_ou_avisa(r, a), j = indice_ou_avisa(r, b);
    if (i < 0 || j < 0) return;
    bfs(r, i, dist, pai);
    printf("Separacao %s -> %s: ", a, b);
    if (dist[j] < 0) {
        printf("nao conectados\n");
        return;
    }
    /* Reconstrói a cadeia de amizades do destino `j` de volta até a origem `i` */
    for (int v = j; v != -1; v = pai[v]) cadeia[k++] = v;
    printf("%d grau(s): ", dist[j]);
    while (k > 0) {
        k--;
        printf("%s%s", r->nome[cadeia[k]], k > 0 ? " -> " : "\n");
    }
}

/*
 * Descobre grupos isolados (componentes conexas):
 * Roda uma BFS para cada pessoa ainda não vista; todos os alcançáveis a partir
 * dela (`dist[j] >= 0`) pertencem ao mesmo grupo conectado.
 */
void grupos(const Rede *r) {
    int dist[MAX_PESSOAS], pai[MAX_PESSOAS];
    bool visto[MAX_PESSOAS] = {false};
    int g = 0;
    printf("Grupos (componentes conexas):\n");
    for (int i = 0; i < r->n; i++) {
        if (visto[i]) continue; /* Já pertence a um grupo impresso anteriormente */
        bfs(r, i, dist, pai);
        printf("  grupo %d:", ++g);
        for (int j = 0; j < r->n; j++) {
            if (dist[j] >= 0) { visto[j] = true; printf(" %s", r->nome[j]); }
        }
        printf("\n");
    }
}
void mais_popular(const Rede *r) {
    int melhor = 0;
    for (int i = 1; i < r->n; i++) {
        if (num_amigos(r, i) > num_amigos(r, melhor)) melhor = i;
    }
    printf("Mais popular: %s (%d amigos)\n", r->nome[melhor], num_amigos(r, melhor));
}

void libera(Rede *r) {
    for (int i = 0; i < r->n; i++) {
        while (r->amigos[i] != NULL) {
            Amizade *prox = r->amigos[i]->proxima;
            free(r->amigos[i]);
            r->amigos[i] = prox;
        }
    }
}

int main(void) {
    /*
     *   Ana ---- Bruno ---- Diego ---- Fabio
     *    |  \      |          |
     *    |   \     |          |
     *  Carla --- Eva        Gabi
     *
     *   Hugo ---- Iris       (grupo isolado)
     */
    Rede r = {0};
    const char *pessoas[] = {"Ana", "Bruno", "Carla", "Diego", "Eva",
                             "Fabio", "Gabi", "Hugo", "Iris"};
    for (size_t i = 0; i < sizeof pessoas / sizeof pessoas[0]; i++) {
        adiciona_pessoa(&r, pessoas[i]);
    }
    const char *amizades[][2] = {
        {"Ana", "Bruno"}, {"Ana", "Carla"}, {"Ana", "Eva"}, {"Carla", "Eva"},
        {"Bruno", "Eva"}, {"Bruno", "Diego"}, {"Diego", "Fabio"},
        {"Diego", "Gabi"}, {"Hugo", "Iris"},
    };
    for (size_t i = 0; i < sizeof amizades / sizeof amizades[0]; i++) {
        adiciona_amizade(&r, amizades[i][0], amizades[i][1]);
    }
    printf("Amizade repetida Bruno-Ana: %s\n\n",
           adiciona_amizade(&r, "Bruno", "Ana") ? "aceita" : "rejeitada");

    mostra_amigos(&r, "Ana");
    mostra_amigos(&r, "Diego");
    mostra_amigos(&r, "Zeca");   /* nome inexistente: avisa em vez de acessar amigos[-1] */
    mostra_em_comum(&r, "Ana", "Bruno");
    mostra_em_comum(&r, "Carla", "Diego");
    sugere_amizades(&r, "Carla");
    sugere_amizades(&r, "Fabio");
    grau_de_separacao(&r, "Carla", "Fabio");
    grau_de_separacao(&r, "Ana", "Iris");
    mais_popular(&r);
    grupos(&r);

    libera(&r);
    return 0;
}
