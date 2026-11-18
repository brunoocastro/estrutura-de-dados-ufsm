/*
 * Árvore genérica (n-ária) de inteiros.
 *
 * Cada nó pode ter qualquer quantidade de filhos. A representação usa
 * "primeiro filho / próximo irmão": cada nó guarda apenas dois ponteiros,
 * um para o seu primeiro filho e outro para o irmão seguinte.
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *primeiro_filho;
    struct No *proximo_irmao;
} No;

No *cria_no(int valor) {
    No *no = malloc(sizeof *no);
    if (no == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    no->valor = valor;
    no->primeiro_filho = NULL;
    no->proximo_irmao = NULL;
    return no;
}

/* Adiciona `filho` como último filho de `pai`. Retorna o próprio filho. */
No *adiciona_filho(No *pai, No *filho) {
    if (pai->primeiro_filho == NULL) {
        pai->primeiro_filho = filho;
    } else {
        No *irmao = pai->primeiro_filho;
        while (irmao->proximo_irmao != NULL) {
            irmao = irmao->proximo_irmao;
        }
        irmao->proximo_irmao = filho;
    }
    return filho;
}

/* Imprime a árvore com indentação proporcional à profundidade (pré-ordem). */
void imprime(const No *no, int profundidade) {
    if (no == NULL) return;
    for (int i = 0; i < profundidade; i++) printf("  ");
    printf("%d\n", no->valor);
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        imprime(f, profundidade + 1);
    }
}

int conta_nos(const No *no) {
    if (no == NULL) return 0;
    int total = 1;
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        total += conta_nos(f);
    }
    return total;
}

int conta_folhas(const No *no) {
    if (no == NULL) return 0;
    if (no->primeiro_filho == NULL) return 1;
    int total = 0;
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        total += conta_folhas(f);
    }
    return total;
}

/* Altura = número de arestas no caminho mais longo da raiz até uma folha. */
int altura(const No *no) {
    if (no == NULL) return -1;
    int maior = -1;
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        int h = altura(f);
        if (h > maior) maior = h;
    }
    return maior + 1;
}

/* Grau = número de filhos do nó. */
int grau(const No *no) {
    int g = 0;
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) g++;
    return g;
}

const No *busca(const No *no, int valor) {
    if (no == NULL) return NULL;
    if (no->valor == valor) return no;
    for (const No *f = no->primeiro_filho; f != NULL; f = f->proximo_irmao) {
        const No *achou = busca(f, valor);
        if (achou != NULL) return achou;
    }
    return NULL;
}

/* Libera em pós-ordem: filhos antes do pai. */
void libera(No *no) {
    while (no != NULL) {
        No *irmao = no->proximo_irmao;
        libera(no->primeiro_filho);
        free(no);
        no = irmao;
    }
}

int main(void) {
    /*
     *            1
     *         /  |  \
     *        2   3   4
     *       / \      |
     *      5   6     7
     *                |
     *                8
     */
    No *raiz = cria_no(1);
    No *n2 = adiciona_filho(raiz, cria_no(2));
    adiciona_filho(raiz, cria_no(3));
    No *n4 = adiciona_filho(raiz, cria_no(4));
    adiciona_filho(n2, cria_no(5));
    adiciona_filho(n2, cria_no(6));
    No *n7 = adiciona_filho(n4, cria_no(7));
    adiciona_filho(n7, cria_no(8));

    printf("Arvore (pre-ordem, indentada por nivel):\n");
    imprime(raiz, 0);

    printf("\nNos: %d\n", conta_nos(raiz));
    printf("Folhas: %d\n", conta_folhas(raiz));
    printf("Altura: %d\n", altura(raiz));
    printf("Grau da raiz: %d\n", grau(raiz));
    printf("Busca 7: %s\n", busca(raiz, 7) ? "encontrado" : "nao encontrado");
    printf("Busca 9: %s\n", busca(raiz, 9) ? "encontrado" : "nao encontrado");

    libera(raiz);
    return 0;
}
