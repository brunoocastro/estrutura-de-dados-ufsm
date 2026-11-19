/*
 * ============================================================================
 * Arquivo : 04_lista_dupla_alarmes_ihm.c
 * Módulo  : 07 — Listas encadeadas           (Ementa: Unidade 1.3)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver uma lista DUPLAMENTE encadeada em ação: cada nó aponta para o
 *   anterior (ant) e para o próximo (prox). Isso permite:
 *     - navegar para frente e para trás em O(1) a partir de qualquer nó;
 *     - remover um nó em O(1) quando já se tem o ponteiro para ele (sem
 *       procurar o anterior, como seria necessário numa lista simples);
 *     - inserir no fim e descartar o mais antigo em O(1), com ponteiros
 *       para as duas pontas.
 *
 * EXEMPLO REAL
 *   Tela de histórico de alarmes de uma IHM (interface homem-máquina) de um
 *   painel industrial. Alarmes novos entram no fim; o operador navega com as
 *   teclas [<] e [>]; ao RECONHECER o alarme exibido, ele sai do histórico; e
 *   quando a memória do painel enche, o alarme mais antigo é descartado.
 *   O mesmo padrão aparece no histórico "voltar/avançar" de um navegador, no
 *   desfazer/refazer de editores e no cache LRU de sistemas operacionais.
 *
 * COMPLEXIDADE (n = alarmes no histórico)
 *   Operação                         Dupla    Simples (para comparação)
 *   registrar (inserir no fim)       O(1)     O(1) com ponteiro p/ cauda
 *   descartar o mais antigo          O(1)     O(1)
 *   navegar para o próximo           O(1)     O(1)
 *   navegar para o anterior          O(1)     O(n) (recomeça do início)
 *   reconhecer (remover o exibido)   O(1)     O(n) (precisa achar o anterior)
 *   listar do mais novo ao mais velho O(n)    O(n) só com pilha/recursão
 *   Espaço: um ponteiro a mais por nó (ant). É o preço das operações O(1).
 *
 * COMO EXECUTAR
 *   ./run 04_lista_dupla_alarmes_ihm               (ENTER avança; depois há um menu)
 *   ./run 04_lista_dupla_alarmes_ihm < /dev/null   (só o roteiro, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Alarme {
    int id;
    int instante;            /* segundos desde 00:00:00 */
    const char *tag;         /* aponta para o catálogo (texto constante) */
    const char *descricao;
    struct Alarme *ant;      /* alarme anterior (mais antigo); NULL no primeiro */
    struct Alarme *prox;     /* alarme seguinte (mais novo); NULL no último */
} Alarme;

typedef struct {
    Alarme *mais_antigo;     /* cabeça */
    Alarme *mais_recente;    /* cauda: permite inserir no fim em O(1) */
    Alarme *cursor;          /* alarme mostrado na tela da IHM */
    size_t qtd;
    size_t capacidade;       /* memória limitada do painel */
    int proximo_id;
    int relogio;
} Historico;

typedef struct { const char *tag, *descricao; } Modelo;

static const Modelo catalogo[] = {
    {"TT-101", "Temperatura alta no reator"},
    {"PT-205", "Pressao baixa na linha de ar"},
    {"LT-310", "Nivel alto no tanque 3"},
    {"FT-120", "Vazao abaixo do minimo"},
    {"M-07",   "Sobrecarga no motor da esteira"},
    {"ES-01",  "Botao de emergencia acionado"},
    {"VT-44",  "Vibracao alta no mancal"},
};
enum { TAM_CATALOGO = sizeof catalogo / sizeof catalogo[0] };

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */
static unsigned long ponteiros_escritos = 0;

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static unsigned curto(const void *p) {
    return p == NULL ? 0u : (unsigned)((uintptr_t)p & 0xFFFFu);
}

/* Imprime um endereço curto ou "NULL " com a mesma largura. */
static void imprime_end(const void *p) {
    if (p == NULL) printf("  NULL ");
    else           printf("  @%04x", curto(p));
}

/* ----------------------------- desenho ---------------------------------- */

/*
 * Desenha a lista inteira:
 *   NULL<--[*|#1|*]<-->[*|#2|*]-->NULL
 * e, embaixo, o cursor e uma tabela com os endereços ant/prox de cada nó.
 * O(n).
 */
static void desenha(const Historico *h) {
    printf("    mais_antigo =");
    imprime_end(h->mais_antigo);
    printf("    mais_recente =");
    imprime_end(h->mais_recente);
    printf("    qtd = %zu/%zu\n", h->qtd, h->capacidade);
    if (h->mais_antigo == NULL) {
        printf("    (histórico vazio)\n\n");
        return;
    }

    printf("    NULL<--");
    for (const Alarme *a = h->mais_antigo; a != NULL; a = a->prox)
        printf("[*|#%d|*]%s", a->id, a->prox != NULL ? "<-->" : "-->NULL");
    printf("\n");

    /* marcador do cursor: cada célula "[*|#d|*]<-->" tem largura variável */
    printf("           ");
    for (const Alarme *a = h->mais_antigo; a != NULL; a = a->prox) {
        int largura = snprintf(NULL, 0, "[*|#%d|*]", a->id);
        if (a == h->cursor) {
            printf("^cursor");
            break;
        }
        printf("%*s", largura + 4, "");
    }
    printf("\n");

    printf("      id   nó     ant    prox    hora      tag     descrição\n");
    for (const Alarme *a = h->mais_antigo; a != NULL; a = a->prox) {
        printf("    %s #%-2d", a == h->cursor ? ">" : " ", a->id);
        imprime_end(a);
        imprime_end(a->ant);
        imprime_end(a->prox);
        printf("   %02d:%02d:%02d  %-7s %s\n", a->instante / 3600, a->instante / 60 % 60,
               a->instante % 60, a->tag, a->descricao);
    }
    printf("\n");
}

/* Simula a tela da IHM mostrando o alarme do cursor. O(1). */
static void tela_ihm(const Historico *h) {
    const Alarme *a = h->cursor;
    printf("    +------------------------------------------------+\n");
    if (a == NULL) {
        printf("    | HISTÓRICO DE ALARMES: nenhum alarme            |\n");
    } else {
        printf("    | ALARME #%-3d  %02d:%02d:%02d   %-7s                |\n", a->id,
               a->instante / 3600, a->instante / 60 % 60, a->instante % 60, a->tag);
        printf("    | %-46s |\n", a->descricao);
        printf("    |  %s anterior                     próximo %s  |\n",
               a->ant != NULL ? "[<]" : "[ ]", a->prox != NULL ? "[>]" : "[ ]");
    }
    printf("    +------------------------------------------------+\n");
}

/* --------------------------- operações ---------------------------------- */

/*
 * Remove o nó 'a' da lista em O(1). Pré-condição: 'a' pertence a h.
 * Só os vizinhos são religados; ninguém é percorrido. Os dois "if" tratam as
 * pontas (a é o primeiro ou o último). Com um nó sentinela eles somem
 * (veja 06_lista_sentinela).
 * 'narrar' != 0 imprime cada escrita de ponteiro.
 */
static void desliga_no(Historico *h, Alarme *a, int narrar) {
    if (a->ant != NULL) {
        a->ant->prox = a->prox;
        if (narrar) {
            if (a->prox != NULL)
                printf("  a->ant->prox = a->prox   (#%d->prox pula o #%d e vai direto ao #%d)\n",
                       a->ant->id, a->id, a->prox->id);
            else
                printf("  a->ant->prox = a->prox   (#%d vira o último: prox = NULL)\n", a->ant->id);
        }
    } else {
        h->mais_antigo = a->prox;
        if (narrar) printf("  mais_antigo = a->prox    (a era o primeiro)\n");
    }
    ponteiros_escritos++;

    if (a->prox != NULL) {
        a->prox->ant = a->ant;
        if (narrar) {
            if (a->ant != NULL)
                printf("  a->prox->ant = a->ant    (#%d->ant pula o #%d e volta direto ao #%d)\n",
                       a->prox->id, a->id, a->ant->id);
            else
                printf("  a->prox->ant = a->ant    (#%d vira o primeiro: ant = NULL)\n", a->prox->id);
        }
    } else {
        h->mais_recente = a->ant;
        if (narrar) printf("  mais_recente = a->ant    (a era o último)\n");
    }
    ponteiros_escritos++;

    /* O cursor não pode ficar apontando para memória liberada. */
    if (h->cursor == a)
        h->cursor = a->prox != NULL ? a->prox : a->ant;
    h->qtd--;
}

/* Descarta o alarme mais antigo. O(1). */
static void descarta_mais_antigo(Historico *h) {
    Alarme *velho = h->mais_antigo;
    printf("  Memória cheia (%zu/%zu): descartando o mais antigo, #%d\n", h->qtd,
           h->capacidade, velho->id);
    desliga_no(h, velho, 1);
    free(velho);
}

/*
 * Registra um alarme novo no fim (é o mais recente). O(1) graças ao ponteiro
 * mais_recente. O cursor vai para o alarme novo, como numa IHM real.
 * Devolve 0 se faltar memória.
 */
static int registra(Historico *h, int modelo, int narrar) {
    Alarme *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;
    h->relogio += 37 + 23 * modelo;          /* só para variar os horários */
    novo->id = h->proximo_id++;
    novo->instante = h->relogio;
    novo->tag = catalogo[modelo].tag;
    novo->descricao = catalogo[modelo].descricao;
    novo->prox = NULL;                       /* será o último */

    if (narrar) printf(">>> registra #%d (%s)\n", novo->id, novo->tag);
    novo->ant = h->mais_recente;             /* 1: novo olha para trás */
    ponteiros_escritos++;
    if (narrar) printf("  novo->ant = mais_recente\n");
    if (h->mais_recente != NULL) {
        h->mais_recente->prox = novo;        /* 2: o antigo último olha para frente */
        if (narrar) printf("  mais_recente->prox = novo\n");
    } else {
        h->mais_antigo = novo;               /* lista vazia: novo é também o primeiro */
        if (narrar) printf("  mais_antigo = novo (era vazia)\n");
    }
    ponteiros_escritos++;
    h->mais_recente = novo;                  /* 3: novo é o último */
    ponteiros_escritos++;
    if (narrar) printf("  mais_recente = novo     (3 escritas: O(1), sem percorrer nada)\n");
    h->cursor = novo;
    h->qtd++;

    if (h->qtd > h->capacidade)
        descarta_mais_antigo(h);
    return 1;
}

/* Move o cursor. O(1): só segue um ponteiro (ant ou prox). */
static void navega(Historico *h, int para_tras) {
    Alarme *destino = h->cursor == NULL ? NULL : (para_tras ? h->cursor->ant : h->cursor->prox);
    if (destino == NULL) {
        printf("  [%s] não há alarme %s: cursor fica onde está\n", para_tras ? "<" : ">",
               para_tras ? "anterior" : "seguinte");
        return;
    }
    printf("  [%s] cursor = cursor->%s   (1 ponteiro seguido: O(1))\n", para_tras ? "<" : ">",
           para_tras ? "ant" : "prox");
    h->cursor = destino;
}

/*
 * Reconhece (remove) o alarme exibido. O(1).
 * Para comparação, conta quantos saltos uma lista SIMPLES precisaria para
 * achar o anterior: essa contagem é feita só para mostrar o número; ela não
 * faz parte do algoritmo da lista dupla.
 */
static void reconhece(Historico *h, int narrar) {
    Alarme *a = h->cursor;
    if (a == NULL) {
        printf("  nada para reconhecer\n");
        return;
    }
    unsigned saltos_simples = 0;
    for (const Alarme *p = h->mais_antigo; p != a; p = p->prox)
        saltos_simples++;

    if (narrar) printf(">>> reconhece #%d (%s) — a = cursor\n", a->id, a->tag);
    desliga_no(h, a, narrar);
    free(a);
    if (narrar) {
        printf("  free(a)\n");
        printf("  Lista dupla: 2 escritas, 0 saltos. Lista simples: %u salto(s) só para achar o\n"
               "  anterior (O(n)), porque o nó não sabe quem aponta para ele.\n", saltos_simples);
    }
}

/* Percorre do mais recente ao mais antigo usando ant. O(n), sem recursão. */
static void lista_do_mais_recente(const Historico *h) {
    printf("  Do mais recente ao mais antigo (seguindo ant): ");
    for (const Alarme *a = h->mais_recente; a != NULL; a = a->ant)
        printf("#%d ", a->id);
    printf("\n");
}

/* Libera todos os nós. O(n). */
static void libera(Historico *h) {
    Alarme *a = h->mais_antigo;
    while (a != NULL) {
        Alarme *seguinte = a->prox;   /* guarda antes do free */
        free(a);
        a = seguinte;
    }
    h->mais_antigo = h->mais_recente = h->cursor = NULL;
    h->qtd = 0;
}

static void titulo(const char *t) {
    printf("============================================================================\n");
    printf("%s\n", t);
    printf("============================================================================\n");
}

/* ------------------------------ menu ------------------------------------ */

/* Menu livre. Termina com 'q' ou com fim da entrada (EOF). */
static int menu(Historico *h) {
    char linha[64];
    int modelo = 0;
    titulo("MENU LIVRE — explore a lista");
    for (;;) {
        printf("Comandos: < anterior | > próximo | r reconhecer | n novo alarme | "
               "l listar | q sair\n> ");
        fflush(stdout);
        if (fgets(linha, sizeof linha, stdin) == NULL) {
            printf("\n(fim da entrada)\n");
            return 1;
        }
        switch (linha[0]) {
        case '<': navega(h, 1); break;
        case '>': navega(h, 0); break;
        case 'r': reconhece(h, 1); break;
        case 'n':
            if (!registra(h, modelo, 1)) return 0;
            modelo = (modelo + 3) % TAM_CATALOGO;
            break;
        case 'l': lista_do_mais_recente(h); break;
        case 'q': return 1;
        default: printf("  comando desconhecido\n"); continue;
        }
        desenha(h);
        tela_ihm(h);
    }
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    Historico h = {NULL, NULL, NULL, 0, 5, 1, 8 * 3600};
    int ok = 1;

    titulo("PARTE 1 — chegam 6 alarmes; o painel guarda no máximo 5");
    for (int k = 0; k < 6 && ok; k++) {
        ok = registra(&h, k, 1);
        if (ok) desenha(&h);
        pausar();
    }

    if (ok) {
        titulo("PARTE 2 — o operador navega pelo histórico");
        tela_ihm(&h);
        pausar();
        int roteiro[] = {1, 1, 1, 0};      /* 1 = [<], 0 = [>] */
        for (size_t k = 0; k < sizeof roteiro / sizeof roteiro[0]; k++) {
            navega(&h, roteiro[k]);
            desenha(&h);
            tela_ihm(&h);
            pausar();
        }

        titulo("PARTE 3 — reconhecer o alarme exibido (remoção O(1) dado o nó, no MEIO)");
        reconhece(&h, 1);
        desenha(&h);
        tela_ihm(&h);
        pausar();

        titulo("PARTE 4 — remover nas pontas: o mais antigo e o mais recente");
        while (h.cursor != NULL && h.cursor->ant != NULL)
            navega(&h, 1);
        reconhece(&h, 1);
        desenha(&h);
        pausar();
        while (h.cursor != NULL && h.cursor->prox != NULL)
            navega(&h, 0);
        reconhece(&h, 1);
        desenha(&h);
        tela_ihm(&h);
        pausar();

        titulo("PARTE 5 — percorrer de trás para frente (só possível em O(n) com ant)");
        lista_do_mais_recente(&h);
        printf("  Escritas de ponteiro no roteiro inteiro: %lu\n\n", ponteiros_escritos);
        pausar();

        ok = menu(&h);
    }

    libera(&h);
    if (!ok) {
        fprintf(stderr, "Sem memória para registrar alarme.\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
