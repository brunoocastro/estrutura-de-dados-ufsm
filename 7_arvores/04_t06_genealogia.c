/*
 * Tarefa Avaliada 06 - Questão 2
 *
 * Árvore genealógica. Cada pessoa guarda uma lista encadeada de filhos(as),
 * de tamanho arbitrário, e uma lista de progenitores (pais/mães). Não há
 * distinção de gênero: "progenitor" vale para pai ou mãe e "filho" vale para
 * filho ou filha.
 *
 * Entrada de dados: uma relação por linha, no formato
 *     progenitor;filho
 * Uma linha vazia (ou "fim") encerra a entrada. Pessoas são criadas na
 * primeira vez em que aparecem.
 *
 * Consulta: o programa lista os ascendentes de cada pessoa cadastrada e, em
 * seguida, aceita nomes para consulta individual até uma linha vazia.
 *
 * Exemplo de uso não interativo:
 *     printf 'Ana;Bia\nCarlos;Bia\nBia;Duda\n\nDuda\n' | ./genealogia
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME 64
#define TAM_LINHA (2 * TAM_NOME + 8)

struct Pessoa;

/* Nó de lista encadeada de pessoas (usado para filhos e para progenitores). */
typedef struct Elo {
    struct Pessoa *pessoa;
    struct Elo *proximo;
} Elo;

typedef struct Pessoa {
    char nome[TAM_NOME];
    Elo *filhos;
    Elo *progenitores;
    unsigned marca;          /* evita listar o mesmo ascendente duas vezes */
    struct Pessoa *proxima;  /* lista de todas as pessoas cadastradas */
} Pessoa;

typedef struct {
    Pessoa *inicio;
    Pessoa *fim;
    unsigned consulta_atual;
} Arvore;

static void *aloca(size_t tamanho) {
    void *p = calloc(1, tamanho);
    if (p == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    return p;
}

Pessoa *busca_pessoa(const Arvore *arv, const char *nome) {
    for (Pessoa *p = arv->inicio; p != NULL; p = p->proxima) {
        if (strcmp(p->nome, nome) == 0) return p;
    }
    return NULL;
}

Pessoa *obtem_pessoa(Arvore *arv, const char *nome) {
    Pessoa *p = busca_pessoa(arv, nome);
    if (p != NULL) return p;
    p = aloca(sizeof *p);
    snprintf(p->nome, sizeof p->nome, "%s", nome);
    if (arv->fim == NULL) arv->inicio = p;
    else arv->fim->proxima = p;
    arv->fim = p;
    return p;
}

static int contem(const Elo *lista, const Pessoa *p) {
    for (; lista != NULL; lista = lista->proximo) {
        if (lista->pessoa == p) return 1;
    }
    return 0;
}

/* Insere no fim para preservar a ordem de cadastro. */
static void anexa(Elo **lista, Pessoa *p) {
    Elo *novo = aloca(sizeof *novo);
    novo->pessoa = p;
    while (*lista != NULL) lista = &(*lista)->proximo;
    *lista = novo;
}

/*
 * Verifica recursivamente se `alvo` é a própria pessoa `p` ou se já está
 * acima de `p` (progenitor, avô, bisavô...). Isso impede que um descendente
 * seja cadastrado como progenitor do seu próprio ancestral (o que criaria ciclo!).
 */
static int eh_ascendente_ou_igual(const Pessoa *p, const Pessoa *alvo) {
    if (p == alvo) return 1; /* Encontrou `alvo` subindo pelos progenitores! */
    for (const Elo *e = p->progenitores; e != NULL; e = e->proximo) {
        if (eh_ascendente_ou_igual(e->pessoa, alvo)) return 1;
    }
    return 0;
}

/* Retorna mensagem de erro, ou NULL em caso de sucesso. */
const char *adiciona_relacao(Arvore *arv, const char *nome_prog, const char *nome_filho) {
    if (strcmp(nome_prog, nome_filho) == 0) return "uma pessoa nao pode ser progenitora de si mesma";
    Pessoa *prog = obtem_pessoa(arv, nome_prog);
    Pessoa *filho = obtem_pessoa(arv, nome_filho);
    if (contem(prog->filhos, filho)) return "relacao ja cadastrada";
    /* Se o filho já é ascendente do progenitor, a relação criaria um ciclo. */
    if (eh_ascendente_ou_igual(prog, filho)) return "relacao criaria um ciclo";

    /* Liga nos dois sentidos: para descer (filhos) e para subir (progenitores) */
    anexa(&prog->filhos, filho);
    anexa(&filho->progenitores, prog);
    return NULL;
}

/*
 * Imprime os ascendentes subindo geração por geração (recursivo).
 * Como na genealogia real uma pessoa tem múltiplos progenitores (Grafo DAG),
 * dois caminhos diferentes podem chegar ao mesmo ancestral; o campo `a->marca`
 * funciona como o vetor `visitado[]` para não imprimir ninguém duas vezes!
 */
static int imprime_ascendentes_rec(const Pessoa *p, unsigned marca, int geracao) {
    int total = 0;
    for (const Elo *e = p->progenitores; e != NULL; e = e->proximo) {
        Pessoa *a = e->pessoa;
        if (a->marca == marca) continue; /* Já foi visitado nesta consulta! */
        a->marca = marca;                /* Marca como visitado nesta consulta */
        printf("    %s (%s)\n", a->nome,
               geracao == 1 ? "progenitor" :
               geracao == 2 ? "avo" : "ascendente distante");
        total++;
        /* Sobe mais um nível na ancestralidade (geracao + 1) */
        total += imprime_ascendentes_rec(a, marca, geracao + 1);
    }
    return total;
}

void imprime_ascendentes(Arvore *arv, const Pessoa *p) {
    printf("  %s:\n", p->nome);
    /* Incrementa `consulta_atual` para gerar um identificador de visita único */
    if (imprime_ascendentes_rec(p, ++arv->consulta_atual, 1) == 0) {
        printf("    (nenhum ascendente cadastrado)\n");
    }
}

void imprime_filhos(const Pessoa *p) {
    printf("  %s:", p->nome);
    if (p->filhos == NULL) printf(" (sem filhos)");
    for (const Elo *e = p->filhos; e != NULL; e = e->proximo) {
        printf(" %s%s", e->pessoa->nome, e->proximo ? "," : "");
    }
    printf("\n");
}

static void libera_elos(Elo *e) {
    while (e != NULL) {
        Elo *prox = e->proximo;
        free(e);
        e = prox;
    }
}

void libera_arvore(Arvore *arv) {
    Pessoa *p = arv->inicio;
    while (p != NULL) {
        Pessoa *prox = p->proxima;
        libera_elos(p->filhos);
        libera_elos(p->progenitores);
        free(p);
        p = prox;
    }
    arv->inicio = arv->fim = NULL;
}

/* Lê uma linha sem o '\n' e sem espaços nas pontas. Retorna 0 no EOF. */
static int le_linha(char *buf, size_t tamanho) {
    if (fgets(buf, (int)tamanho, stdin) == NULL) return 0;
    size_t n = strcspn(buf, "\r\n");
    if (buf[n] == '\0' && !feof(stdin)) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}  /* descarta excesso */
    }
    buf[n] = '\0';
    while (n > 0 && (buf[n - 1] == ' ' || buf[n - 1] == '\t')) buf[--n] = '\0';
    size_t ini = strspn(buf, " \t");
    memmove(buf, buf + ini, n - ini + 1);
    return 1;
}

static void apara(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == ' ' || s[n - 1] == '\t')) s[--n] = '\0';
    size_t ini = strspn(s, " \t");
    memmove(s, s + ini, n - ini + 1);
}

int main(void) {
    Arvore arv = {0};
    char linha[TAM_LINHA];

    printf("=== Entrada de dados ===\n");
    printf("Digite uma relacao por linha no formato progenitor;filho\n");
    printf("(linha vazia ou 'fim' para encerrar)\n");
    while (le_linha(linha, sizeof linha) && linha[0] != '\0' && strcmp(linha, "fim") != 0) {
        char *sep = strchr(linha, ';');
        if (sep == NULL) {
            printf("  ignorado (falta ';'): %s\n", linha);
            continue;
        }
        *sep = '\0';
        char *prog = linha, *filho = sep + 1;
        apara(prog);
        apara(filho);
        if (prog[0] == '\0' || filho[0] == '\0') {
            printf("  ignorado (nome vazio)\n");
            continue;
        }
        if (strlen(prog) >= TAM_NOME || strlen(filho) >= TAM_NOME) {
            printf("  ignorado (nome com mais de %d caracteres)\n", TAM_NOME - 1);
            continue;
        }
        const char *erro = adiciona_relacao(&arv, prog, filho);
        if (erro != NULL) printf("  ignorado (%s): %s;%s\n", erro, prog, filho);
        else printf("  %s -> %s\n", prog, filho);
    }

    if (arv.inicio == NULL) {
        printf("Nenhuma pessoa cadastrada.\n");
        return 0;
    }

    printf("\n=== Filhos de cada pessoa ===\n");
    for (Pessoa *p = arv.inicio; p != NULL; p = p->proxima) imprime_filhos(p);

    printf("\n=== Ascendentes de cada pessoa ===\n");
    for (Pessoa *p = arv.inicio; p != NULL; p = p->proxima) imprime_ascendentes(&arv, p);

    printf("\n=== Consulta ===\n");
    printf("Digite um nome para listar seus ascendentes (linha vazia encerra)\n");
    while (le_linha(linha, sizeof linha) && linha[0] != '\0') {
        Pessoa *p = busca_pessoa(&arv, linha);
        if (p == NULL) printf("  %s: pessoa nao cadastrada\n", linha);
        else imprime_ascendentes(&arv, p);
    }

    libera_arvore(&arv);
    return 0;
}
