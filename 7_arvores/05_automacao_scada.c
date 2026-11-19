/*
 * Problema 4A (Engenharia de Controle e Automação):
 * Supervisório SCADA e Hierarquia ISA-95 / OPC-UA modelados como ÁRVORE.
 *
 * Na norma ISA-95 e no espaço de endereçamento OPC-UA, os ativos de uma
 * planta industrial seguem uma hierarquia estrita (1 único pai por ativo):
 *
 *   Planta_Fabril
 *   ├── Area_Usinagem
 *   │   └── CLP_Usinagem (50 W)
 *   │       ├── Torno_CNC (4500 W)
 *   │       └── Sensor_Vibracao (5 W)
 *   └── Area_Envase
 *       ├── CLP_Envase (40 W)
 *       │   ├── Esteira_Principal (1500 W)
 *       │   ├── Valvula_Proporcional (25 W)
 *       │   └── Sensor_PT100 (2 W)
 *       └── Robo_Paletizador_KUKA (3200 W)
 *
 * Como cada nó pode ter qualquer quantidade de subordinados (0, 1, 2, ..., N),
 * usamos uma Árvore Genérica com a representação "Primeiro Filho / Próximo Irmão".
 *
 * Operações demonstradas e qual percurso resolve cada problema:
 *   1. Exibir a árvore de ativos do supervisório  -> Percurso em PRÉ-ORDEM
 *   2. Calcular a potência elétrica total (W)     -> Percurso em PÓS-ORDEM
 *      (soma primeiro o consumo dos filhos para depois acumular no pai)
 *   3. Queda do disjuntor da Área de Envase       -> Percurso em PÓS-ORDEM
 *      (desliga atuadores/sensores filhos antes de desenergizar o CLP/Área)
 *   4. Liberar toda a memória alocada (`free`)    -> Percurso em PÓS-ORDEM
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ativo {
    const char *tag;              /* Nome/Tag OPC-UA do equipamento ou área */
    int potencia_watts;           /* Consumo próprio nominal quando ligado  */
    bool energizado;              /* Estado atual: true = ON, false = OFF   */
    struct Ativo *primeiro_filho; /* Ponteiro para descer 1 nível (filho)   */
    struct Ativo *proximo_irmao;  /* Ponteiro para caminhar no mesmo nível  */
} Ativo;

/* Aloca e inicializa um novo ativo da planta (inicialmente energizado). */
Ativo *cria_ativo(const char *tag, int potencia_watts) {
    Ativo *novo = malloc(sizeof *novo);
    if (novo == NULL) {
        fprintf(stderr, "Falha de alocacao de memoria\n");
        exit(EXIT_FAILURE);
    }
    novo->tag = tag;
    novo->potencia_watts = potencia_watts;
    novo->energizado = true;
    novo->primeiro_filho = NULL;
    novo->proximo_irmao = NULL;
    return novo;
}

/*
 * Pendura `subordinado` como último filho de `supervisor`.
 * Todos os filhos de um mesmo pai formam uma lista encadeada ligada por
 * `proximo_irmao`, iniciando em `supervisor->primeiro_filho`.
 */
Ativo *adiciona_subordinado(Ativo *supervisor, Ativo *subordinado) {
    if (supervisor->primeiro_filho == NULL) {
        supervisor->primeiro_filho = subordinado;
    } else {
        Ativo *atual = supervisor->primeiro_filho;
        while (atual->proximo_irmao != NULL) {
            atual = atual->proximo_irmao;
        }
        atual->proximo_irmao = subordinado;
    }
    return subordinado;
}

/*
 * Localiza um ativo na árvore pelo nome da Tag (busca em profundidade).
 * Retorna o ponteiro para o nó encontrado ou NULL se não existir.
 */
Ativo *busca_tag(Ativo *raiz, const char *tag) {
    if (raiz == NULL) return NULL;
    if (strcmp(raiz->tag, tag) == 0) return raiz;

    /* Procura recursivamente em cada subárvore filha */
    for (Ativo *f = raiz->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        Ativo *encontrado = busca_tag(f, tag);
        if (encontrado != NULL) return encontrado;
    }
    return NULL;
}

/*
 * PERCURSO EM PÓS-ORDEM: calcula a potência ativa total da subárvore.
 * Por que pós-ordem? Porque para saber a potência total de uma Área ou CLP,
 * precisamos primeiro somar a potência de todos os equipamentos subordinados
 * a ela, e só no final somar a potência própria do nó.
 */
int calcula_potencia_total(const Ativo *no) {
    if (no == NULL || !no->energizado) return 0;

    int soma_filhos = 0;
    for (const Ativo *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        soma_filhos += calcula_potencia_total(f); /* 1º visita os filhos */
    }
    return soma_filhos + no->potencia_watts;     /* 2º soma o próprio nó */
}

/*
 * PERCURSO EM PRÉ-ORDEM: imprime a tela do Supervisório SCADA com indentação.
 * Por que pré-ordem? Porque o nome da Área/CLP pai deve aparecer no topo
 * antes da listagem dos seus sensores e atuadores filhos.
 */
void imprime_supervisorio(const Ativo *no, int nivel) {
    if (no == NULL) return;

    for (int i = 0; i < nivel; i++) printf("  ");
    printf("[%s] %-24s | proprio: %4d W | subarvore: %4d W\n",
           no->energizado ? "ON " : "OFF",
           no->tag,
           no->energizado ? no->potencia_watts : 0,
           calcula_potencia_total(no));

    for (const Ativo *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        imprime_supervisorio(f, nivel + 1);
    }
}

/*
 * PERCURSO EM PÓS-ORDEM: desligamento seguro em cascata (Queda de Disjuntor).
 * Na automação industrial, desliga-se da ponta (atuadores/motores e sensores)
 * em direção ao controlador/área pai, registrando a sequência de eventos (SOE).
 */
void desenergiza_em_pos_ordem(Ativo *no) {
    if (no == NULL) return;

    /* 1º Desenergiza todos os filhos recursivamente */
    for (Ativo *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        desenergiza_em_pos_ordem(f);
    }

    /* 2º Só depois desenergiza o nó pai atual */
    no->energizado = false;
    printf("  -> Desligado: %s\n", no->tag);
}

/*
 * PERCURSO EM PÓS-ORDEM: libera toda a memória alocada.
 * Jamais podemos chamar free(pai) antes de liberar seus filhos!
 */
void libera_arvore(Ativo *no) {
    while (no != NULL) {
        Ativo *proximo = no->proximo_irmao;
        libera_arvore(no->primeiro_filho); /* 1º libera toda a subárvore filha */
        free(no);                          /* 2º libera o nó atual             */
        no = proximo;
    }
}

int main(void) {
    /* Montagem da árvore ISA-95 / OPC-UA da Planta */
    Ativo *planta = cria_ativo("Planta_Santa_Maria", 0);

    Ativo *usinagem = adiciona_subordinado(planta, cria_ativo("Area_Usinagem", 0));
    Ativo *clp_usi  = adiciona_subordinado(usinagem, cria_ativo("CLP_Usinagem", 50));
    adiciona_subordinado(clp_usi, cria_ativo("Torno_CNC", 4500));
    adiciona_subordinado(clp_usi, cria_ativo("Sensor_Vibracao", 5));

    Ativo *envase   = adiciona_subordinado(planta, cria_ativo("Area_Envase", 0));
    Ativo *clp_env  = adiciona_subordinado(envase, cria_ativo("CLP_Envase", 40));
    adiciona_subordinado(clp_env, cria_ativo("Esteira_Principal", 1500));
    adiciona_subordinado(clp_env, cria_ativo("Valvula_Proporcional", 25));
    adiciona_subordinado(clp_env, cria_ativo("Sensor_PT100", 2));
    adiciona_subordinado(envase, cria_ativo("Robo_Paletizador_KUKA", 3200));

    printf("=== 1. ESTADO INICIAL DO SUPERVISORIO SCADA (PRE-ORDEM) ===\n");
    imprime_supervisorio(planta, 0);
    printf("\nPotencia total demandada pela planta: %d W\n\n",
           calcula_potencia_total(planta));

    printf("=== 2. EVENTO: QUEDA DO DISJUNTOR DA 'Area_Envase' (POS-ORDEM) ===\n");
    Ativo *alvo = busca_tag(planta, "Area_Envase");
    if (alvo != NULL) {
        desenergiza_em_pos_ordem(alvo);
    }

    printf("\n=== 3. SUPERVISORIO APOS DESARME DA AREA DE ENVASE ===\n");
    imprime_supervisorio(planta, 0);
    printf("\nNova potencia total demandada pela planta: %d W\n",
           calcula_potencia_total(planta));

    libera_arvore(planta);
    return 0;
}
