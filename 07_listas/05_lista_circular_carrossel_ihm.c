/*
 * ============================================================================
 * Arquivo : 05_lista_circular_carrossel_ihm.c
 * Módulo  : 07 — Listas encadeadas           (Ementa: Unidade 1.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   1. Lista CIRCULAR: o último nó aponta de volta para o primeiro, então não
 *      existe NULL no fim e "dar a volta" é natural. Guardando só um ponteiro
 *      para o ÚLTIMO nó (cauda), temos o primeiro de graça (cauda->prox),
 *      inserção no fim do ciclo em O(1) e avanço em O(1) (cauda = cauda->prox).
 *      O exemplo é o carrossel de telas de uma IHM, executado passo a passo,
 *      incluindo os casos de borda (um nó só apontando para si, anel vazio).
 *   2. BÔNUS — detecção de ciclo (algoritmo de Floyd, "tartaruga e lebre"):
 *      descobre se uma lista que DEVERIA terminar em NULL tem um ciclo por
 *      erro, onde ele começa e qual o seu tamanho, usando só dois ponteiros.
 *
 * EXEMPLO REAL
 *   - Carrossel de telas de uma IHM ou de um painel de chão de fábrica, que
 *     alterna sozinho "Visão geral → Tanques → Motores → Alarmes → Visão
 *     geral...". O técnico pode incluir ou retirar telas com o carrossel
 *     rodando.
 *   - Escalonador round-robin de tarefas: a mesma ideia de "dar a volta",
 *     feita com fila em ../06_filas/06_round_robin_clp.c.
 *   - Anel lógico de passagem de token entre mestres de uma rede Profibus.
 *   - Floyd: verificar a integridade de listas em firmware (uma escrita fora
 *     dos limites pode transformar uma lista em ciclo e travar um laço).
 *
 * COMPLEXIDADE (n = telas ou nós)
 *   Operação                         Tempo    Espaço
 *   tela atual                       O(1)     O(1)   (cauda->prox)
 *   avançar para a próxima tela      O(1)     O(1)   (cauda = cauda->prox)
 *   incluir tela no fim do ciclo     O(1)     O(1)
 *   retirar tela pelo nome           O(n)     O(1)   (busca; religar é O(1))
 *   listar todas (uma volta)         O(n)     O(1)
 *   Floyd (detectar ciclo)           O(n)     O(1)
 *   alternativa: marcar visitados    O(n)     O(n)   (precisa de memória extra)
 *   Por quê (Floyd): dentro do ciclo, a lebre (2 passos) se aproxima da
 *   tartaruga (1 passo) uma posição por iteração, então elas se encontram em
 *   no máximo "tamanho do ciclo" iterações depois de ambas entrarem nele.
 *
 * COMO EXECUTAR
 *   ./run 05_lista_circular_carrossel_ihm               (ENTER avança cada passo)
 *   ./run 05_lista_circular_carrossel_ihm < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

static void titulo(const char *t) {
    printf("============================================================================\n");
    printf("%s\n", t);
    printf("============================================================================\n");
}

/* ======================================================================== */
/* PARTE A — CARROSSEL DE TELAS COM LISTA CIRCULAR                          */
/* ======================================================================== */

typedef struct Tela {
    const char *nome;       /* até 5 letras, para caber no desenho */
    int segundos;           /* tempo de exibição da tela */
    struct Tela *prox;      /* na lista circular nunca é NULL */
} Tela;

/* Lista circular representada SÓ pela cauda (último nó do ciclo).
 * Vazia: cauda == NULL. Um nó só: cauda->prox == cauda (aponta para si).
 * A tela exibida agora é sempre cauda->prox. */
typedef struct {
    Tela *cauda;
    int n;
} Carrossel;

/* Desenha o anel começando pela tela exibida (cauda->prox). O(n).
 * Cada célula "[NOME |ss]-->" ocupa 14 colunas. */
static void desenha(const Carrossel *c) {
    if (c->cauda == NULL) {
        printf("    cauda = NULL (carrossel vazio)\n\n");
        return;
    }
    /* rótulo "cauda" sobre o último nó */
    printf("    %*scauda\n", 4 + 14 * (c->n - 1), "");
    printf("    %*sv\n", 4 + 14 * (c->n - 1) + 2, "");

    printf("    +-->");
    const Tela *t = c->cauda->prox;
    for (int k = 0; k < c->n; k++, t = t->prox)
        printf("[%-5s|%2ds]%s", t->nome, t->segundos, k < c->n - 1 ? "-->" : "--+");
    printf("\n");
    printf("    |%*s|\n", 14 * c->n + 2, "");
    printf("    +");
    for (int k = 0; k < 14 * c->n + 2; k++) putchar('-');
    printf("+\n");
    printf("        ^ na tela agora = cauda->prox   (a linha de baixo fecha o anel: não há NULL)\n\n");
}

/* Inclui uma tela no FIM do ciclo em O(1): ela entra entre a cauda e a tela
 * atual e passa a ser a nova cauda (será exibida por último na volta).
 * Devolve 0 se faltar memória. */
static int insere_fim(Carrossel *c, const char *nome, int segundos) {
    Tela *nova = malloc(sizeof *nova);
    if (nova == NULL) return 0;
    nova->nome = nome;
    nova->segundos = segundos;
    if (c->cauda == NULL) {
        nova->prox = nova;              /* sozinha: aponta para si mesma */
    } else {
        nova->prox = c->cauda->prox;    /* nova aponta para a primeira */
        c->cauda->prox = nova;          /* a antiga última aponta para a nova */
    }
    c->cauda = nova;                    /* a nova é a última */
    c->n++;
    return 1;
}

/* Avança para a próxima tela em O(1). Nada é movido: a cauda anda um nó e,
 * com isso, quem era a primeira vira a última. Sem nenhum "if chegou ao
 * fim, volta ao início": o anel já faz isso. */
static void avanca(Carrossel *c) {
    if (c->cauda != NULL)
        c->cauda = c->cauda->prox;
}

/*
 * Retira a tela 'nome'. O(n) para achar, O(1) para religar.
 * Procura com o ANTERIOR (ant), pois numa lista simples é ele que precisa
 * mudar. O laço é do-while e para quando ant VOLTA à cauda: numa lista
 * circular não existe NULL para encerrar a busca (while (p != NULL) giraria
 * para sempre). Devolve 1 se retirou, 0 se não existe.
 */
static int retira(Carrossel *c, const char *nome) {
    if (c->cauda == NULL) {
        printf("  retira(%s): carrossel vazio\n", nome);
        return 0;
    }
    Tela *ant = c->cauda;
    int visitados = 0;
    do {
        visitados++;
        if (strcmp(ant->prox->nome, nome) == 0) {
            Tela *alvo = ant->prox;
            if (alvo == ant) {                  /* era a única tela */
                c->cauda = NULL;
                printf("  retira(%s): era a única (alvo->prox == alvo) → cauda = NULL\n", nome);
            } else {
                ant->prox = alvo->prox;         /* o anterior pula o alvo */
                int era_cauda = alvo == c->cauda;
                if (era_cauda)                  /* retirou a última: a cauda recua */
                    c->cauda = ant;
                printf("  retira(%s): achada após %d visita(s); ant->prox = alvo->prox%s\n",
                       nome, visitados, era_cauda ? "; era a cauda → cauda = ant" : "");
            }
            free(alvo);
            c->n--;
            return 1;
        }
        ant = ant->prox;
    } while (ant != c->cauda);
    printf("  retira(%s): deu uma volta completa (%d visitas) e voltou à cauda: não existe\n",
           nome, visitados);
    return 0;
}

static void libera_carrossel(Carrossel *c) {
    while (c->cauda != NULL)
        retira(c, c->cauda->nome);
}

/* Mostra 'quantas' exibições a partir da tela atual, com o relógio. */
static void exibe(Carrossel *c, int quantas, int *relogio) {
    for (int k = 0; k < quantas && c->cauda != NULL; k++) {
        const Tela *t = c->cauda->prox;
        printf("  t=%3d..%3d s: exibindo %-5s → avança (cauda = cauda->prox)\n",
               *relogio, *relogio + t->segundos, t->nome);
        *relogio += t->segundos;
        avanca(c);
        desenha(c);
        pausar();
    }
}

static int demo_carrossel(void) {
    Carrossel c = {NULL, 0};
    int relogio = 0;

    titulo("PARTE A1 — montando o carrossel: GERAL, TANQ, MOTOR, ALARM");
    if (!insere_fim(&c, "GERAL", 5) || !insere_fim(&c, "TANQ", 3) ||
        !insere_fim(&c, "MOTOR", 4) || !insere_fim(&c, "ALARM", 2)) {
        libera_carrossel(&c);
        return 0;
    }
    desenha(&c);
    pausar();

    titulo("PARTE A2 — o carrossel roda: 5 exibições (passa da última de volta à primeira)");
    exibe(&c, 5, &relogio);
    printf("  Depois do ALARM veio o GERAL de novo: nenhum teste de fim de lista.\n\n");
    pausar();

    titulo("PARTE A3 — o técnico inclui a tela VIBR (vibração) com o carrossel rodando");
    if (!insere_fim(&c, "VIBR", 3)) {
        libera_carrossel(&c);
        return 0;
    }
    printf("  insere_fim(VIBR): nova->prox = cauda->prox; cauda->prox = nova; cauda = nova  (O(1))\n");
    desenha(&c);
    pausar();
    exibe(&c, 2, &relogio);

    titulo("PARTE A4 — retirar telas: do meio, a última do ciclo e uma inexistente");
    retira(&c, "MOTOR");
    desenha(&c);
    pausar();
    retira(&c, c.cauda->nome);   /* a última do ciclo: a cauda precisa recuar */
    desenha(&c);
    pausar();
    retira(&c, "XYZ");
    printf("\n");
    pausar();

    titulo("PARTE A5 — casos de borda: um nó só e anel vazio");
    while (c.n > 1)
        retira(&c, c.cauda->prox->nome);
    desenha(&c);
    printf("  Sobrou uma tela: cauda->prox == cauda? %s. Avançar a mantém na tela.\n",
           c.cauda->prox == c.cauda ? "sim" : "não");
    avanca(&c);
    printf("  Depois de avançar, a tela exibida continua sendo %s.\n", c.cauda->prox->nome);
    pausar();
    retira(&c, c.cauda->nome);
    desenha(&c);
    avanca(&c);
    printf("  avançar com o carrossel vazio não faz nada (o if de avanca protege).\n\n");
    pausar();
    libera_carrossel(&c);
    return 1;
}

/* ======================================================================== */
/* PARTE B — DETECÇÃO DE CICLO (FLOYD)                                       */
/* ======================================================================== */

typedef struct No {
    int valor;
    struct No *prox;
} No;

typedef struct {
    int tem_ciclo;
    No *inicio_ciclo;   /* primeiro nó do ciclo (NULL se não houver) */
    int tamanho_ciclo;
    unsigned long passos; /* quantos ponteiros foram seguidos no total */
} ResultadoFloyd;

/*
 * Floyd em três fases, O(n) tempo e O(1) espaço:
 *   1) tartaruga anda 1, lebre anda 2. Se a lebre achar NULL, não há ciclo.
 *      Se as duas se encontrarem, há ciclo.
 *   2) põe a tartaruga de volta no início; as duas andam 1 por vez; onde se
 *      encontrarem é o início do ciclo (resultado matemático: a distância do
 *      início da lista até o ciclo é igual à distância do ponto de encontro
 *      até o início do ciclo, dando voltas inteiras).
 *   3) uma volta a partir do início do ciclo mede o seu tamanho.
 * 'narrar' != 0 imprime uma linha por iteração.
 */
static ResultadoFloyd floyd(No *inicio, int narrar) {
    ResultadoFloyd r = {0, NULL, 0, 0};
    No *tartaruga = inicio;
    No *lebre = inicio;
    int it = 0;

    if (narrar) printf("  Fase 1: tartaruga anda 1, lebre anda 2\n");
    while (lebre != NULL && lebre->prox != NULL) {
        tartaruga = tartaruga->prox;
        lebre = lebre->prox->prox;
        r.passos += 3;
        it++;
        if (narrar) {
            printf("    iteração %2d: tartaruga em [%d]   lebre em ", it, tartaruga->valor);
            if (lebre == NULL) printf("NULL\n");
            else printf("[%d]%s\n", lebre->valor,
                        tartaruga == lebre ? "   ← ENCONTRO: existe ciclo!" : "");
        }
        if (tartaruga == lebre) {
            r.tem_ciclo = 1;
            break;
        }
    }
    if (!r.tem_ciclo) {
        if (narrar) printf("    a lebre chegou a NULL: a lista termina, sem ciclo\n");
        return r;
    }

    if (narrar) printf("  Fase 2: tartaruga volta ao início; ambas andam 1\n");
    tartaruga = inicio;
    while (tartaruga != lebre) {
        tartaruga = tartaruga->prox;
        lebre = lebre->prox;
        r.passos += 2;
        if (narrar) printf("    tartaruga em [%d]   lebre em [%d]\n", tartaruga->valor, lebre->valor);
    }
    r.inicio_ciclo = tartaruga;
    if (narrar) printf("    encontro em [%d]: é aí que o ciclo começa\n", tartaruga->valor);

    if (narrar) printf("  Fase 3: dar uma volta para medir o ciclo\n");
    No *p = r.inicio_ciclo;
    do {
        p = p->prox;
        r.tamanho_ciclo++;
        r.passos++;
    } while (p != r.inicio_ciclo);
    return r;
}

/* Imprime os primeiros 'limite' nós seguindo prox, sem confiar que exista NULL.
 * Mostra por que um while (p != NULL) nunca terminaria numa lista com ciclo. */
static void percurso_limitado(const No *inicio, int limite) {
    printf("    ");
    int k = 0;
    for (const No *p = inicio; p != NULL && k < limite; p = p->prox, k++)
        printf("[%d]-->", p->valor);
    printf(k == limite ? "...\n    (nunca chega a NULL)\n" : "NULL\n");
}

/* Cria n nós 10, 20, ..., guardando os endereços em v[]. Devolve 0 se faltar memória. */
static int cria_lista(No **v, int n) {
    for (int k = 0; k < n; k++) {
        v[k] = malloc(sizeof *v[k]);
        if (v[k] == NULL) {
            while (k-- > 0) free(v[k]);
            return 0;
        }
        v[k]->valor = 10 * (k + 1);
        v[k]->prox = NULL;
        if (k > 0) v[k - 1]->prox = v[k];
    }
    return 1;
}

/*
 * Libera uma lista que PODE ter ciclo. ARMADILHA: o laço comum de liberação
 * entraria no ciclo e daria free duas vezes no mesmo nó (double free).
 * Por isso, se houver ciclo, primeiro o "desfazemos": achamos o último nó do
 * ciclo (o que aponta para inicio_ciclo) e o fazemos apontar para NULL.
 */
static void libera_seguro(No *inicio) {
    ResultadoFloyd r = floyd(inicio, 0);
    if (r.tem_ciclo) {
        No *ultimo = r.inicio_ciclo;
        for (int k = 1; k < r.tamanho_ciclo; k++)
            ultimo = ultimo->prox;
        ultimo->prox = NULL;     /* agora é uma lista comum */
    }
    while (inicio != NULL) {
        No *seguinte = inicio->prox;
        free(inicio);
        inicio = seguinte;
    }
}

static int demo_floyd(void) {
    enum { N = 8 };
    No *v[N];

    titulo("BÔNUS 1 — lista correta (termina em NULL)");
    if (!cria_lista(v, N)) return 0;
    percurso_limitado(v[0], 20);
    ResultadoFloyd r = floyd(v[0], 1);
    printf("  Resultado: %s (%lu ponteiros seguidos)\n\n", r.tem_ciclo ? "CICLO" : "sem ciclo",
           r.passos);
    pausar();

    titulo("BÔNUS 2 — o mesmo erro que uma escrita fora do vetor pode causar: [80]->prox = [40]");
    v[N - 1]->prox = v[3];       /* bug proposital: o último volta para o 4º nó */
    printf("  Um percurso comum até NULL ficaria preso para sempre:\n");
    percurso_limitado(v[0], 12);
    pausar();
    r = floyd(v[0], 1);
    printf("  Resultado: ciclo começa em [%d], tem %d nós; %lu ponteiros seguidos,\n"
           "  usando só 2 ponteiros extras (O(1) de memória).\n\n",
           r.inicio_ciclo->valor, r.tamanho_ciclo, r.passos);
    pausar();

    printf("  Liberando com segurança (desfaz o ciclo antes do free).\n\n");
    libera_seguro(v[0]);
    return 1;
}

/* Floyd também confirma que uma lista circular é, por definição, um grande ciclo. */
static int demo_anel_e_ciclo(void) {
    enum { N = 4 };
    No *v[N];
    titulo("BÔNUS 3 — numa lista circular, o ciclo é intencional");
    if (!cria_lista(v, N)) return 0;
    v[N - 1]->prox = v[0];       /* fecha o anel: último aponta para o primeiro */
    percurso_limitado(v[0], 10);
    ResultadoFloyd r = floyd(v[0], 0);
    printf("  Floyd: ciclo começa em [%d] e tem %d nós (o anel inteiro).\n"
           "  Por isso, em listas circulares o laço de percurso para quando VOLTA ao\n"
           "  ponto de partida (do { ... } while (p != inicio)), nunca em NULL.\n\n",
           r.inicio_ciclo->valor, r.tamanho_ciclo);
    libera_seguro(v[0]);
    return 1;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    if (!demo_carrossel() || !demo_floyd() || !demo_anel_e_ciclo()) {
        fprintf(stderr, "Sem memória para a demonstração.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
