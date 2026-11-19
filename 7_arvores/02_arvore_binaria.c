/*
 * Árvore binária de busca (ABB) de inteiros.
 *
 * Cada nó tem no máximo dois filhos: esquerdo e direito. Além disso, a ABB
 * mantém a regra de ordenação: valores menores ficam à esquerda e maiores à
 * direita. Valores repetidos são ignorados.
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct No {
    int valor;
    struct No *esquerda;
    struct No *direita;
} No;

No *cria_no(int valor) {
    No *no = malloc(sizeof *no);
    if (no == NULL) {
        fprintf(stderr, "Falha de alocacao\n");
        exit(EXIT_FAILURE);
    }
    no->valor = valor;
    no->esquerda = NULL;
    no->direita = NULL;
    return no;
}

/*
 * Insere `valor` mantendo a propriedade da ABB:
 *   - menores vão para a subárvore esquerda;
 *   - maiores vão para a subárvore direita;
 *   - iguais são ignorados (sem duplicatas).
 * Retorna a (possivelmente nova) raiz da subárvore.
 */
No *insere(No *raiz, int valor) {
    /* Caso base: encontrou posição vazia (folha); cria e pendura o novo nó */
    if (raiz == NULL) return cria_no(valor);

    if (valor < raiz->valor) {
        /* Valor menor que o nó atual: desce recursivamente pela ESQUERDA */
        raiz->esquerda = insere(raiz->esquerda, valor);
    } else if (valor > raiz->valor) {
        /* Valor maior que o nó atual: desce recursivamente pela DIREITA */
        raiz->direita = insere(raiz->direita, valor);
    }
    /* Se for igual (valor == raiz->valor), não faz nada e retorna a própria raiz */
    return raiz;
}

/*
 * Busca binária na árvore: a cada passo descarta metade da subárvore!
 * Custo O(altura) -> O(log n) se a árvore estiver balanceada.
 */
const No *busca(const No *raiz, int valor) {
    /* Enquanto não chegou em NULL e ainda não encontrou o valor procurado: */
    while (raiz != NULL && raiz->valor != valor) {
        /* Decide se desce para a esquerda (menor) ou para a direita (maior) */
        raiz = valor < raiz->valor ? raiz->esquerda : raiz->direita;
    }
    return raiz; /* Retorna o ponteiro do nó encontrado, ou NULL se não existir */
}

/* O menor elemento de uma ABB está sempre na extrema esquerda. */
No *minimo(No *raiz) {
    while (raiz != NULL && raiz->esquerda != NULL) {
        raiz = raiz->esquerda; /* Desce sempre pela esquerda até não ter mais filho esquerdo */
    }
    return raiz;
}

/*
 * Remove `valor` mantendo a árvore ordenada e retorna a nova raiz da subárvore.
 * Três casos possíveis ao encontrar o nó:
 *  1. Nó folha (0 filhos): libera o nó e retorna NULL para o pai;
 *  2. Nó com 1 único filho: libera o nó e retorna o filho para ocupar seu lugar;
 *  3. Nó com 2 filhos: substitui o valor pelo sucessor em-ordem (o menor valor
 *     da subárvore direita, que é maior que toda a esquerda e menor que o resto
 *     da direita) e remove recursivamente a cópia lá embaixo.
 */
No *remove_valor(No *raiz, int valor) {
    if (raiz == NULL) return NULL; /* Valor não encontrado na árvore */

    if (valor < raiz->valor) {
        raiz->esquerda = remove_valor(raiz->esquerda, valor); /* Procura à esquerda */
    } else if (valor > raiz->valor) {
        raiz->direita = remove_valor(raiz->direita, valor);   /* Procura à direita  */
    } else if (raiz->esquerda == NULL || raiz->direita == NULL) {
        /* Casos 1 e 2: tem 0 ou 1 filho. Pega o filho existente (ou NULL se folha) */
        No *filho = raiz->esquerda != NULL ? raiz->esquerda : raiz->direita;
        free(raiz);   /* Libera o nó removido */
        return filho; /* Devolve o filho para ser reenganchado diretamente no pai */
    } else {
        /* Caso 3: tem os 2 filhos. Busca o sucessor (menor da subárvore direita) */
        No *sucessor = minimo(raiz->direita);
        raiz->valor = sucessor->valor; /* Copia o valor do sucessor para o nó atual */
        /* Remove o nó duplicado do sucessor na subárvore direita (que terá 0 ou 1 filho) */
        raiz->direita = remove_valor(raiz->direita, sucessor->valor);
    }
    return raiz;
}

/* 1. Pré-ordem (Raiz -> Esq -> Dir): útil para clonar ou exibir hierarquia. */
void pre_ordem(const No *r) {
    if (r == NULL) return;
    printf("%d ", r->valor); /* 1º Processa a raiz atual     */
    pre_ordem(r->esquerda);  /* 2º Visita subárvore esquerda */
    pre_ordem(r->direita);   /* 3º Visita subárvore direita  */
}

/* 2. Em-ordem (Esq -> Raiz -> Dir): em uma ABB, produz os valores ORDENADOS! */
void em_ordem(const No *r) {
    if (r == NULL) return;
    em_ordem(r->esquerda);   /* 1º Visita todos os menores (esquerda) */
    printf("%d ", r->valor); /* 2º Imprime o nó atual                 */
    em_ordem(r->direita);    /* 3º Visita todos os maiores (direita)  */
}

/* 3. Pós-ordem (Esq -> Dir -> Raiz): filhos sempre antes do pai (ex.: free). */
void pos_ordem(const No *r) {
    if (r == NULL) return;
    pos_ordem(r->esquerda);  /* 1º Visita subárvore esquerda */
    pos_ordem(r->direita);   /* 2º Visita subárvore direita  */
    printf("%d ", r->valor); /* 3º Processa a raiz por último */
}

/*
 * 4. Em nível / Largura (BFS): visita nível 0, nível 1, nível 2...
 * Usa uma FILA (FIFO) de ponteiros em vez de recursão.
 */
void em_nivel(const No *raiz) {
    if (raiz == NULL) return;
    const No *fila[64];
    int ini = 0, fim = 0;
    fila[fim++] = raiz; /* Enfileira a raiz (nível 0) */
    while (ini < fim) {
        const No *atual = fila[ini++]; /* Desenfileira o nó mais antigo */
        printf("%d ", atual->valor);
        /* Enfileira os filhos esquerdo e direito para serem visitados no próximo nível */
        if (atual->esquerda != NULL) fila[fim++] = atual->esquerda;
        if (atual->direita != NULL)  fila[fim++] = atual->direita;
    }
}

int conta_nos(const No *r) {
    /* Soma 1 (nó atual) + nós da esquerda + nós da direita */
    return r == NULL ? 0 : 1 + conta_nos(r->esquerda) + conta_nos(r->direita);
}

int conta_folhas(const No *r) {
    if (r == NULL) return 0;
    /* Se não tem filho à esquerda nem à direita, é folha (retorna 1) */
    if (r->esquerda == NULL && r->direita == NULL) return 1;
    return conta_folhas(r->esquerda) + conta_folhas(r->direita);
}

int altura(const No *r) {
    if (r == NULL) return -1;      /* Árvore vazia tem altura -1 (folha terá 0) */
    int he = altura(r->esquerda);  /* Mede altura da esquerda (pós-ordem)       */
    int hd = altura(r->direita);   /* Mede altura da direita (pós-ordem)        */
    return 1 + (he > hd ? he : hd);/* 1 aresta + a maior das duas subárvores    */
}

void libera(No *r) {
    if (r == NULL) return;
    libera(r->esquerda); /* 1º Libera subárvore esquerda */
    libera(r->direita);  /* 2º Libera subárvore direita  */
    free(r);             /* 3º Libera o pai por último   */
}
static void imprime_percursos(const No *raiz) {
    printf("  pre-ordem: "); pre_ordem(raiz); printf("\n");
    printf("  em ordem:  "); em_ordem(raiz);  printf("\n");
    printf("  pos-ordem: "); pos_ordem(raiz); printf("\n");
    printf("  em nivel:  "); em_nivel(raiz);  printf("\n");
}

int main(void) {
    /*
     *            50
     *          /    \
     *        30      70
     *       /  \    /  \
     *      20  40  60  80
     */
    int valores[] = {50, 30, 70, 20, 40, 60, 80};
    No *raiz = NULL;
    for (size_t i = 0; i < sizeof valores / sizeof valores[0]; i++) {
        raiz = insere(raiz, valores[i]);
    }

    printf("Percursos:\n");
    imprime_percursos(raiz);
    printf("Nos: %d | Folhas: %d | Altura: %d\n",
           conta_nos(raiz), conta_folhas(raiz), altura(raiz));
    printf("Busca 60: %s\n", busca(raiz, 60) ? "encontrado" : "nao encontrado");
    printf("Busca 65: %s\n", busca(raiz, 65) ? "encontrado" : "nao encontrado");

    printf("\nRemove 20 (folha), 70 (dois filhos) e 50 (raiz):\n");
    raiz = remove_valor(raiz, 20);
    raiz = remove_valor(raiz, 70);
    raiz = remove_valor(raiz, 50);
    imprime_percursos(raiz);
    printf("Nos: %d | Altura: %d\n", conta_nos(raiz), altura(raiz));

    libera(raiz);
    return 0;
}
