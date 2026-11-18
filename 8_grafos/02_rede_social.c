/*
 * Rede social modelada como grafo não dirigido.
 *
 * Vértice = pessoa; aresta = amizade (sempre mútua). A lista de adjacência
 * guarda os amigos de cada pessoa. Sobre o grafo, o programa responde:
 *   - amigos de uma pessoa (vizinhos) e quantidade (grau);
 *   - amigos em comum (interseção de vizinhanças);
 *   - sugestões de amizade (amigos de amigos, ordenados por amigos em comum);
 *   - grau de separação e a cadeia de amizades (BFS);
 *   - grupos isolados (componentes conexas);
 *   - pessoa mais popular (maior grau).
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

void mostra_amigos(const Rede *r, const char *nome) {
    int p = indice(r, nome);
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
    printf("Amigos em comum entre %s e %s:", a, b);
    if (amigos_em_comum(r, indice(r, a), indice(r, b), true) == 0) printf(" nenhum");
    printf("\n");
}

/* Sugestões = pessoas a distância 2 (amigos de amigos que ainda não são amigos). */
void sugere_amizades(const Rede *r, const char *nome) {
    int p = indice(r, nome);
    int comuns[MAX_PESSOAS] = {0};
    for (const Amizade *x = r->amigos[p]; x != NULL; x = x->proxima) {
        for (const Amizade *y = r->amigos[x->amigo]; y != NULL; y = y->proxima) {
            if (y->amigo != p && !sao_amigos(r, p, y->amigo)) comuns[y->amigo]++;
        }
    }
    printf("Sugestoes para %s:", nome);
    bool alguma = false;
    for (;;) {   /* imprime em ordem decrescente de amigos em comum */
        int melhor = -1;
        for (int i = 0; i < r->n; i++) {
            if (comuns[i] > 0 && (melhor < 0 || comuns[i] > comuns[melhor])) melhor = i;
        }
        if (melhor < 0) break;
        printf(" %s (%d em comum)", r->nome[melhor], comuns[melhor]);
        comuns[melhor] = 0;
        alguma = true;
    }
    if (!alguma) printf(" nenhuma");
    printf("\n");
}

/* BFS a partir de `origem`: dist = número de amizades até cada pessoa. */
static void bfs(const Rede *r, int origem, int dist[], int pai[]) {
    int fila[MAX_PESSOAS], ini = 0, fim = 0;
    for (int i = 0; i < r->n; i++) { dist[i] = -1; pai[i] = -1; }
    dist[origem] = 0;
    fila[fim++] = origem;
    while (ini < fim) {
        int u = fila[ini++];
        for (const Amizade *x = r->amigos[u]; x != NULL; x = x->proxima) {
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
    int i = indice(r, a), j = indice(r, b);
    bfs(r, i, dist, pai);
    printf("Separacao %s -> %s: ", a, b);
    if (dist[j] < 0) {
        printf("nao conectados\n");
        return;
    }
    for (int v = j; v != -1; v = pai[v]) cadeia[k++] = v;
    printf("%d grau(s): ", dist[j]);
    while (k > 0) {
        k--;
        printf("%s%s", r->nome[cadeia[k]], k > 0 ? " -> " : "\n");
    }
}

void grupos(const Rede *r) {
    int dist[MAX_PESSOAS], pai[MAX_PESSOAS];
    bool visto[MAX_PESSOAS] = {false};
    int g = 0;
    printf("Grupos (componentes conexas):\n");
    for (int i = 0; i < r->n; i++) {
        if (visto[i]) continue;
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
