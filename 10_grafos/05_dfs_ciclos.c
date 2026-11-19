/*
 * ============================================================================
 * Arquivo : 05_dfs_ciclos.c
 * Módulo  : 10 — Grafos                       (Ementa: Unidade 6.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a BUSCA EM PROFUNDIDADE (DFS) "mergulhando" e voltando (backtracking):
 *     1. DFS recursiva: pilha de chamadas desenhada, ordem de descoberta e de
 *        término, e detecção de ciclo em grafo NÃO dirigido;
 *     2. DFS com PILHA EXPLÍCITA (sem recursão), com a pilha desenhada na
 *        vertical, chegando à mesma ordem de descoberta;
 *     3. detecção de ciclo em grafo DIRIGIDO com três cores (branco, cinza,
 *        preto) num esquema de intertravamentos de máquinas;
 *     4. componentes conexos (ilhas de uma rede após falhas);
 *     5. tabela: profundidade da recursão, o risco de estouro de pilha.
 *
 * EXEMPLO REAL
 *   - Robô de inspeção explorando os corredores de um galpão desconhecido:
 *     segue um corredor até o fim e volta ao último cruzamento com saída
 *     inexplorada (é exatamente a DFS).
 *   - Intertravamentos no CLP: "a prensa só liga se a bomba hidráulica
 *     estiver ligada". Se as permissões formarem um CICLO, nada liga nunca
 *     (deadlock de partida). A DFS encontra o ciclo antes do comissionamento.
 *   - Diagnóstico de rede: quais equipamentos ficaram isolados após falhas?
 *
 * COMPLEXIDADE (V = vértices, E = arestas)
 *   Operação                 Lista de adj.   Matriz     Espaço extra
 *   DFS (recursiva/pilha)    O(V + E)        O(V²)      O(V) (visitados + pilha)
 *   ciclo (não dirigido)     O(V + E)        O(V²)      O(V)
 *   ciclo (dirigido, cores)  O(V + E)        O(V²)      O(V)
 *   componentes conexos      O(V + E)        O(V²)      O(V)
 *   Por quê: cada vértice é descoberto uma única vez (marcado ao entrar) e sua
 *   lista de vizinhos é percorrida uma vez; a soma das listas é E (dirigido)
 *   ou 2E (não dirigido). A pilha (de chamadas ou explícita) pode chegar a V
 *   níveis num grafo em linha: esse é o custo O(V) de espaço.
 *
 * COMO EXECUTAR
 *   ./run 05_dfs_ciclos               (ENTER avança cada passo)
 *   ./run 05_dfs_ciclos < /dev/null   (executa direto, sem pausas)
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
 * GRAFO (dirigido ou não) COM LISTA DE ADJACÊNCIA
 * ==========================================================================
 * Inserção no FIM da lista: os vizinhos são visitados na ordem declarada,
 * o que torna a narração previsível.
 */
typedef struct No {
    int vertice;
    struct No *prox;
} No;

typedef struct {
    int n;
    int dirigido;
    No **inicio;
    No **fim;
    const char *const *nome;   /* rótulos para imprimir (NULL nas tabelas) */
} Grafo;

static Grafo cria(int n, int dirigido, const char *const *nome) {
    Grafo g = { n, dirigido, aloca((size_t)n, sizeof(No *)), aloca((size_t)n, sizeof(No *)), nome };
    return g;
}

static void anexa(Grafo *g, int u, int v) {
    No *novo = aloca(1, sizeof *novo);
    novo->vertice = v;
    if (g->fim[u] == NULL) g->inicio[u] = novo;
    else g->fim[u]->prox = novo;
    g->fim[u] = novo;
}

/* O(1). No não dirigido a aresta vai para as duas listas. */
static void aresta(Grafo *g, int u, int v) {
    anexa(g, u, v);
    if (!g->dirigido) anexa(g, v, u);
}

/* O(grau(u)): remove a primeira ocorrência de v na lista de u. */
static void remove_arco(Grafo *g, int u, int v) {
    No *ant = NULL;
    for (No *a = g->inicio[u]; a != NULL; ant = a, a = a->prox) {
        if (a->vertice != v) continue;
        if (ant == NULL) g->inicio[u] = a->prox;
        else ant->prox = a->prox;
        if (g->fim[u] == a) g->fim[u] = ant;   /* armadilha: atualizar o ponteiro de fim */
        free(a);
        return;
    }
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

static void recua(int nivel) {
    for (int i = 0; i < nivel; i++) printf("   ");
}

/* ==========================================================================
 * PARTE 1 — DFS RECURSIVA (galpão explorado por um robô)
 * ==========================================================================
 */
enum { A, B, C, D, E, F, G, H, NG };
static const char *const letra[NG] = { "A", "B", "C", "D", "E", "F", "G", "H" };

typedef struct {
    int *visitado, *pai, *descoberta, *termino;
    int *caminho;          /* pilha de chamadas atual, só para desenhar */
    int prof, prof_max, relogio, narrar;
    long chamadas, arestas;
    int ciclo_u, ciclo_v;  /* primeira aresta de retorno encontrada (-1 = nenhuma) */
} EstadoDFS;

static EstadoDFS estado_novo(int n, int narrar) {
    EstadoDFS s = { aloca((size_t)n, sizeof(int)), aloca((size_t)n, sizeof(int)),
                    aloca((size_t)n, sizeof(int)), aloca((size_t)n, sizeof(int)),
                    aloca((size_t)n, sizeof(int)), 0, 0, 0, narrar, 0, 0, -1, -1 };
    for (int v = 0; v < n; v++) s.pai[v] = -1;
    return s;
}

static void estado_libera(EstadoDFS *s) {
    free(s->visitado); free(s->pai); free(s->descoberta); free(s->termino); free(s->caminho);
}

static void desenha_pilha_chamadas(const Grafo *g, const EstadoDFS *s) {
    printf("   pilha de chamadas: ");
    for (int i = 0; i < s->prof; i++) printf("%s%s", g->nome[s->caminho[i]], i + 1 < s->prof ? " > " : "\n");
}

/*
 * DFS recursiva em grafo NÃO dirigido, a partir de u (que chegou por `pai`).
 * Regra de ciclo: vizinho já visitado que NÃO é o pai -> existe outro caminho
 * até ele -> ciclo. O pai precisa ser excluído porque a aresta u—pai aparece
 * nas duas listas: sem esse teste, TODA aresta pareceria um ciclo.
 * Custo: O(grau(u)) por chamada; somando todas, O(V + E).
 */
static void dfs_rec(const Grafo *g, int u, int pai, EstadoDFS *s) {
    s->visitado[u] = 1;
    s->pai[u] = pai;
    s->descoberta[u] = ++s->relogio;
    s->caminho[s->prof++] = u;
    if (s->prof > s->prof_max) s->prof_max = s->prof;
    s->chamadas++;
    if (s->narrar) {
        recua(s->prof - 1);
        printf("-> entra em %s (descoberta no instante %d)", g->nome[u], s->descoberta[u]);
        desenha_pilha_chamadas(g, s);
        pausar();
    }
    for (const No *a = g->inicio[u]; a != NULL; a = a->prox) {
        int v = a->vertice;
        s->arestas++;
        if (!s->visitado[v]) {
            if (s->narrar) { recua(s->prof); printf("%s: vizinho %s novo -> desce\n", g->nome[u], g->nome[v]); }
            dfs_rec(g, v, u, s);
        } else if (v == pai) {
            if (s->narrar) { recua(s->prof); printf("%s: vizinho %s é o pai (a mesma aresta de volta) -> ignora\n", g->nome[u], g->nome[v]); }
        } else if (s->descoberta[v] < s->descoberta[u]) {
            if (s->narrar) {
                recua(s->prof);
                printf("%s: vizinho %s JÁ VISITADO e não é o pai -> aresta de retorno: CICLO!\n",
                       g->nome[u], g->nome[v]);
            }
            if (s->ciclo_u < 0) { s->ciclo_u = u; s->ciclo_v = v; }
        } else if (s->narrar) {
            recua(s->prof);
            printf("%s: vizinho %s já terminado (é a aresta %s—%s vista do outro lado) -> ignora\n",
                   g->nome[u], g->nome[v], g->nome[v], g->nome[u]);
        }
    }
    s->termino[u] = ++s->relogio;
    s->prof--;
    if (s->narrar) {
        recua(s->prof);
        printf("<- termina %s (instante %d): sem vizinhos novos, volta%s%s\n", g->nome[u],
               s->termino[u], pai >= 0 ? " para " : "", pai >= 0 ? g->nome[pai] : " (fim)");
    }
}

static void monta_galpao(Grafo *g) {
    int corredores[][2] = { {A, B}, {A, C}, {B, D}, {B, E}, {D, G}, {C, F}, {E, F}, {F, H} };
    for (size_t k = 0; k < sizeof corredores / sizeof corredores[0]; k++) {
        aresta(g, corredores[k][0], corredores[k][1]);
    }
}

static void desenha_galpao(void) {
    printf("      A --- B --- D        cruzamentos = vértices\n");
    printf("      |     |     |        corredores  = arestas\n");
    printf("      C     E     G\n");
    printf("       \\   /\n");
    printf("         F --- H\n\n");
}

static void parte1_recursiva(void) {
    Grafo g = cria(NG, 0, letra);
    monta_galpao(&g);
    printf("=== PARTE 1: DFS recursiva — robô explorando o galpão a partir de A ===\n\n");
    desenha_galpao();
    pausar();

    EstadoDFS s = estado_novo(NG, 1);
    dfs_rec(&g, A, -1, &s);

    printf("\n  Ordem de descoberta: ");
    for (int t = 1; t <= 2 * NG; t++) {
        for (int v = 0; v < NG; v++) if (s.descoberta[v] == t) printf("%s ", letra[v]);
    }
    printf("\n  Instantes (descoberta/término):");
    for (int v = 0; v < NG; v++) printf(" %s=%d/%d", letra[v], s.descoberta[v], s.termino[v]);
    printf("\n  Os intervalos são aninhados: G (4/5) está dentro de D (3/6), que está\n");
    printf("  dentro de B (2/15). Isso é a pilha de chamadas vista no tempo.\n");
    if (s.ciclo_u >= 0) {
        /* O ciclo é o caminho da árvore DFS de ciclo_v até ciclo_u, mais a aresta de retorno. */
        printf("  Ciclo encontrado: %s", letra[s.ciclo_v]);
        int rota[NG], k = 0;
        for (int v = s.ciclo_u; v != s.ciclo_v; v = s.pai[v]) rota[k++] = v;
        while (k > 0) printf(" - %s", letra[rota[--k]]);
        printf(" - %s\n", letra[s.ciclo_v]);
    }
    printf("  Contadores: %ld chamadas (= V), %ld vizinhos examinados (= 2E = 16),\n",
           s.chamadas, s.arestas);
    printf("  profundidade máxima da recursão = %d.\n\n", s.prof_max);
    estado_libera(&s);
    libera(&g);
    pausar();
}

/* ==========================================================================
 * PARTE 2 — DFS COM PILHA EXPLÍCITA (mesmo galpão)
 * ==========================================================================
 * Troca a pilha de chamadas do C por um vetor (../05_pilhas/). Vantagem: a
 * pilha fica no heap, que é muito maior que a pilha do programa (~8 MB);
 * uma DFS recursiva num grafo em linha com milhões de vértices estoura.
 *
 * Detalhes que fazem a ordem coincidir com a recursiva:
 *   - marcar como visitado ao DESEMPILHAR (um vértice pode estar 2x na
 *     pilha; a segunda cópia é descartada);
 *   - empilhar os vizinhos em ordem INVERSA, para o primeiro vizinho ficar
 *     no topo e ser explorado primeiro, como na recursão.
 * Tamanho da pilha: cada aresta empilha no máximo 1 vértice por lado -> 2E + 1.
 */
static void desenha_pilha(const Grafo *g, const int pilha[], int topo) {
    if (topo == 0) { printf("      |       |  (vazia)\n      +-------+\n"); return; }
    for (int i = topo - 1; i >= 0; i--) {
        printf("      |  %-3s  |%s\n", g->nome[pilha[i]], i == topo - 1 ? " <- topo" : "");
    }
    printf("      +-------+\n");
}

static void parte2_pilha_explicita(void) {
    Grafo g = cria(NG, 0, letra);
    monta_galpao(&g);
    printf("=== PARTE 2: a mesma DFS com PILHA EXPLÍCITA (sem recursão) ===\n\n");

    int capacidade = 1;
    for (int v = 0; v < NG; v++) for (const No *a = g.inicio[v]; a != NULL; a = a->prox) capacidade++;
    int *pilha = aloca((size_t)capacidade, sizeof(int));
    int visitado[NG] = { 0 };
    int topo = 0, passo = 0;
    long arestas = 0;

    pilha[topo++] = A;
    printf("  empilha a origem A\n");
    desenha_pilha(&g, pilha, topo);
    pausar();

    char ordem[64] = "";
    int len = 0;
    while (topo > 0) {
        int u = pilha[--topo];
        printf("\n--- Passo %d: desempilha %s ---\n", ++passo, letra[u]);
        if (visitado[u]) {
            printf("      %s já foi visitado por outro caminho -> descarta\n", letra[u]);
            desenha_pilha(&g, pilha, topo);
            continue;
        }
        visitado[u] = 1;
        len += snprintf(ordem + len, sizeof ordem - (size_t)len, "%s ", letra[u]);

        int viz[NG], k = 0;   /* coleta para empilhar em ordem inversa */
        for (const No *a = g.inicio[u]; a != NULL; a = a->prox) {
            arestas++;
            if (!visitado[a->vertice]) viz[k++] = a->vertice;
        }
        printf("      visita %s; vizinhos não visitados:", letra[u]);
        if (k == 0) printf(" nenhum (beco sem saída: o robô volta)");
        for (int i = 0; i < k; i++) printf(" %s", letra[viz[i]]);
        printf("\n");
        if (k > 0) printf("      empilha em ordem inversa para %s ficar no topo\n", letra[viz[0]]);
        while (k > 0) pilha[topo++] = viz[--k];
        desenha_pilha(&g, pilha, topo);
        printf("      descobertos: %s\n", ordem);
        pausar();
    }
    printf("\n  Ordem final: %s(igual à recursiva)\n", ordem);
    printf("  %d passos de desempilhar, %ld vizinhos examinados: O(V + E).\n\n", passo, arestas);
    free(pilha);
    libera(&g);
    pausar();
}

/* ==========================================================================
 * PARTE 3 — CICLO EM GRAFO DIRIGIDO: INTERTRAVAMENTOS DE PARTIDA
 * ==========================================================================
 * Arco u -> v: "v só pode ligar depois que u estiver ligado" (u é permissivo
 * de v). Um ciclo significa que nenhum equipamento do ciclo jamais liga.
 *
 * Por que três cores e não só "visitado"?
 *   BRANCO = ainda não descoberto; CINZA = descoberto e ainda na pilha de
 *   chamadas (é ancestral do vértice atual); PRETO = terminado.
 *   Arco para CINZA  -> volta para um ancestral -> CICLO.
 *   Arco para PRETO  -> só outro caminho até um vértice já resolvido -> NÃO é
 *   ciclo. Com um único "visitado" os dois casos se confundiriam: armadilha
 *   comum é reaproveitar o teste do grafo não dirigido no dirigido.
 */
enum { BOMBA, PRENSA, ESTEIRA_SAI, ESTEIRA_ENT, EXAUSTOR, ROBO, NM };
static const char *const maquina[NM] = {
    "Bomba hidráulica", "Prensa", "Esteira de saída", "Esteira de entrada", "Exaustor", "Robô"
};
enum { BRANCO, CINZA, PRETO };
static const char *const nome_cor[] = { "branco", "CINZA", "PRETO" };

typedef struct {
    int cor[NM], pai[NM];
    int ordem_termino[NM], qtd_terminados;
    int ciclo_u, ciclo_v;
} EstadoCores;

/* Imprime s e completa com espaços até `largura` COLUNAS (acentos UTF-8 têm 2 bytes). */
static void imprime_alinhado(const char *s, int largura) {
    int colunas = 0;
    for (const char *p = s; *p != '\0'; p++) {
        if (((unsigned char)*p & 0xC0) != 0x80) colunas++;   /* ignora bytes de continuação */
    }
    printf("%s%*s", s, largura > colunas ? largura - colunas : 0, "");
}

static void desenha_cores(const EstadoCores *s) {
    for (int v = 0; v < NM; v++) {
        fputs(v % 2 == 0 ? "        " : "   ", stdout);
        imprime_alinhado(maquina[v], 19);
        printf("= ");
        imprime_alinhado(nome_cor[s->cor[v]], 7);
        if (v % 2 == 1) printf("\n");
    }
}

/* Retorna 1 assim que acha um arco para vértice CINZA. O(V + E) no total. */
static int dfs_cores(const Grafo *g, int u, int nivel, EstadoCores *s) {
    s->cor[u] = CINZA;
    recua(nivel);
    printf("-> %s fica CINZA (em análise)\n", maquina[u]);
    for (const No *a = g->inicio[u]; a != NULL; a = a->prox) {
        int v = a->vertice;
        recua(nivel + 1);
        if (s->cor[v] == BRANCO) {
            printf("arco %s -> %s: branco, desce\n", maquina[u], maquina[v]);
            s->pai[v] = u;
            if (dfs_cores(g, v, nivel + 1, s)) return 1;
        } else if (s->cor[v] == CINZA) {
            printf("arco %s -> %s: %s está CINZA (ancestral) -> CICLO!\n",
                   maquina[u], maquina[v], maquina[v]);
            s->ciclo_u = u;
            s->ciclo_v = v;
            return 1;
        } else {
            printf("arco %s -> %s: PRETO (já resolvido por outro caminho) -> não é ciclo\n",
                   maquina[u], maquina[v]);
        }
    }
    s->cor[u] = PRETO;
    s->ordem_termino[s->qtd_terminados++] = u;
    recua(nivel);
    printf("<- %s fica PRETO (todas as dependências resolvidas)\n", maquina[u]);
    return 0;
}

/* Roda a DFS de cores a partir de cada vértice branco; 1 se houver ciclo. */
static int procura_ciclo(const Grafo *g, EstadoCores *s) {
    for (int v = 0; v < NM; v++) { s->cor[v] = BRANCO; s->pai[v] = -1; }
    s->qtd_terminados = 0;
    s->ciclo_u = s->ciclo_v = -1;
    for (int v = 0; v < NM; v++) {
        if (s->cor[v] != BRANCO) continue;
        printf("  nova DFS a partir de %s\n", maquina[v]);
        if (dfs_cores(g, v, 1, s)) return 1;
        desenha_cores(s);
        pausar();
    }
    return 0;
}

static void parte3_intertravamentos(void) {
    Grafo g = cria(NM, 1, maquina);
    int permissivos[][2] = {
        {BOMBA, PRENSA}, {PRENSA, ROBO}, {ESTEIRA_SAI, ESTEIRA_ENT}, {ESTEIRA_ENT, ROBO},
        {EXAUSTOR, PRENSA},
        {ROBO, BOMBA},   /* ERRO de projeto: "bomba só liga com o robô ligado" */
    };
    for (size_t k = 0; k < sizeof permissivos / sizeof permissivos[0]; k++) {
        aresta(&g, permissivos[k][0], permissivos[k][1]);
    }
    printf("=== PARTE 3: ciclo em grafo DIRIGIDO — intertravamentos de partida ===\n\n");
    printf("  Bomba -> Prensa -> Robô <- Esteira de entrada <- Esteira de saída\n");
    printf("  Exaustor -> Prensa          e, por engano, Robô -> Bomba\n\n");
    pausar();

    EstadoCores s;
    if (procura_ciclo(&g, &s)) {
        printf("\n  CICLO DE INTERTRAVAMENTO: %s", maquina[s.ciclo_v]);
        int rota[NM], k = 0;
        for (int v = s.ciclo_u; v != s.ciclo_v; v = s.pai[v]) rota[k++] = v;
        while (k > 0) printf(" -> %s", maquina[rota[--k]]);
        printf(" -> %s\n  Nenhuma dessas máquinas consegue partir. Corrigindo: remove Robô -> Bomba.\n\n",
               maquina[s.ciclo_v]);
    }
    pausar();

    remove_arco(&g, ROBO, BOMBA);
    if (!procura_ciclo(&g, &s)) {
        printf("\n  Sem ciclo: o grafo é um DAG. Bônus: a ordem INVERSA de término é uma\n");
        printf("  ordem de partida válida (ordenação topológica por DFS):\n   ");
        for (int i = s.qtd_terminados - 1; i >= 0; i--) {
            printf(" %s%s", maquina[s.ordem_termino[i]], i > 0 ? " ->" : "\n");
        }
        printf("  O algoritmo de Kahn (07_topologica_partida_maquinas.c) chega a uma ordem\n");
        printf("  válida pelo outro lado: começa por quem não depende de ninguém.\n\n");
    }
    libera(&g);
    pausar();
}

/* ==========================================================================
 * PARTE 4 — COMPONENTES CONEXOS: ILHAS DE UMA REDE APÓS FALHAS
 * ==========================================================================
 * Uma DFS a partir de um vértice alcança exatamente a sua componente. Repetindo
 * para cada vértice ainda não visitado, contamos as componentes em O(V + E).
 */
static void marca_componente(const Grafo *g, int u, int id, int comp[]) {
    comp[u] = id;
    printf(" %s", g->nome[u]);
    for (const No *a = g->inicio[u]; a != NULL; a = a->prox) {
        if (comp[a->vertice] == 0) marca_componente(g, a->vertice, id, comp);
    }
}

static void parte4_componentes(void) {
    enum { N_SENS = 9 };
    static const char *const no[N_SENS] = { "GW", "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8" };
    Grafo g = cria(N_SENS, 0, no);
    int enlaces[][2] = { {0, 1}, {1, 2}, {0, 3}, {4, 5}, {5, 6}, {4, 6}, {7, 8} };
    for (size_t k = 0; k < sizeof enlaces / sizeof enlaces[0]; k++) aresta(&g, enlaces[k][0], enlaces[k][1]);

    printf("=== PARTE 4: componentes conexos — rede de sensores após falhas de enlace ===\n\n");
    printf("      GW --- S1 --- S2      S4 --- S5      S7 --- S8\n");
    printf("      |                      \\    /\n");
    printf("      S3                       S6\n\n");
    int comp[N_SENS] = { 0 };   /* 0 = ainda sem componente */
    int qtd = 0;
    for (int v = 0; v < N_SENS; v++) {
        if (comp[v] != 0) continue;
        printf("  %s ainda não visitado -> nova DFS, componente %d:", no[v], ++qtd);
        marca_componente(&g, v, qtd, comp);
        printf("\n");
    }
    printf("  %d componentes. Só a componente do gateway (GW) chega ao supervisório;\n", qtd);
    printf("  S4..S8 precisam de um novo enlace (10_mst_cabeamento_sensores.c escolhe os\n");
    printf("  cabos mais baratos para unir tudo).\n\n");
    libera(&g);
    pausar();
}

/* ==========================================================================
 * PARTE 5 — PROFUNDIDADE DA RECURSÃO
 * ==========================================================================
 * Mesmo V, formatos diferentes: em LINHA (esteiras em série) a recursão
 * empilha V chamadas; em ESTRELA (switch central) empilha 2. É por isso que
 * DFS recursiva em grafos enormes estoura a pilha e se usa a pilha explícita.
 *
 * Para MEDIR sem correr o risco, simulamos a recursão com uma pilha de
 * "quadros" no heap: cada quadro guarda o que a chamada recursiva guardaria
 * (o vértice e o próximo vizinho a examinar). A altura máxima dessa pilha é
 * exatamente a profundidade que a recursão atingiria, e a ordem de visita é
 * a mesma da dfs_rec. Nos testes deste repositório, a recursão de verdade
 * compilada com -fsanitize=address (quadros de ~1 KB) estourou a pilha
 * padrão de 8 MB com V = 10000 em linha.
 */
typedef struct {
    int *visitado;
    long chamadas, arestas;
    int prof_max;
} Medida;

typedef struct {
    int vertice;
    const No *proximo;   /* próximo vizinho a examinar: o "ponto de retorno" */
} Quadro;

/* O(V + E) de tempo; a pilha cabe em V quadros porque cada vértice entra 1 vez. */
static void dfs_mede(const Grafo *g, int origem, Medida *m) {
    Quadro *pilha = aloca((size_t)g->n, sizeof *pilha);
    int topo = 0;
    m->visitado[origem] = 1;
    m->chamadas++;
    pilha[topo++] = (Quadro){ origem, g->inicio[origem] };
    m->prof_max = 1;
    while (topo > 0) {
        Quadro *q = &pilha[topo - 1];
        if (q->proximo == NULL) { topo--; continue; }   /* = "return" da recursão */
        int v = q->proximo->vertice;
        q->proximo = q->proximo->prox;
        m->arestas++;
        if (!m->visitado[v]) {                          /* = chamada recursiva */
            m->visitado[v] = 1;
            m->chamadas++;
            pilha[topo++] = (Quadro){ v, g->inicio[v] };
            if (topo > m->prof_max) m->prof_max = topo;
        }
    }
    free(pilha);
}

static void parte5_profundidade(void) {
    const int tamanhos[] = { 10, 100, 1000, 10000 };
    printf("=== PARTE 5: custo e profundidade da DFS recursiva ===\n\n");
    printf("  %7s | %10s %10s %12s | %10s %12s\n", "V", "chamadas", "vizinhos", "prof. linha",
           "vizinhos", "prof. estrela");
    printf("  ----------------------------------------------------------------------\n");
    for (int i = 0; i < 4; i++) {
        int n = tamanhos[i];
        Grafo linha = cria(n, 0, NULL), estrela = cria(n, 0, NULL);
        for (int v = 0; v + 1 < n; v++) aresta(&linha, v, v + 1);
        for (int v = 1; v < n; v++) aresta(&estrela, 0, v);
        Medida ml = { aloca((size_t)n, sizeof(int)), 0, 0, 0 };
        Medida me = { aloca((size_t)n, sizeof(int)), 0, 0, 0 };
        dfs_mede(&linha, 0, &ml);
        dfs_mede(&estrela, 0, &me);
        printf("  %7d | %10ld %10ld %12d | %10ld %12d\n", n, ml.chamadas, ml.arestas, ml.prof_max,
               me.arestas, me.prof_max);
        free(ml.visitado);
        free(me.visitado);
        libera(&linha);
        libera(&estrela);
    }
    printf("\n  Chamadas = V e vizinhos = 2E = 2(V-1) nos dois formatos: O(V + E).\n");
    printf("  A profundidade, porém, chega a V na linha. Com milhões de vértices a\n");
    printf("  recursão estoura a pilha do programa; a pilha explícita da Parte 2 não.\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    parte1_recursiva();
    parte2_pilha_explicita();
    parte3_intertravamentos();
    parte4_componentes();
    parte5_profundidade();
    return 0;
}
