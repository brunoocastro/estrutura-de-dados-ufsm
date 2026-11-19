/*
 * ============================================================================
 * Arquivo : 02_lista_passo_a_passo.c
 * Módulo  : 07 — Listas encadeadas           (Ementa: Unidade 1.3 / 0.7)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver, passo a passo, o que acontece na memória em cada operação de uma
 *   lista simplesmente encadeada:
 *     - inserção no início, no fim e em ordem;
 *     - remoção do início, do meio e do fim;
 *     - liberação completa da lista.
 *   A cada passo os nós são desenhados como [10|*]-->[20|*]-->NULL, com o
 *   endereço de cada nó logo abaixo. O ponto central é a técnica do
 *   "ponteiro para o elo" (No **elo): elo guarda o ENDEREÇO do ponteiro que
 *   será religado, que pode ser a variável `inicio` ou o campo `prox` de
 *   algum nó. Com isso, um único trecho de código serve para início, meio e
 *   fim, sem nenhum `if` especial para o primeiro nó.
 *
 * EXEMPLO REAL
 *   Lista de ordens de produção (OPs) de uma célula de manufatura mantida em
 *   ordem de prioridade: chegam OPs urgentes (entram no início), OPs normais
 *   (entram no fim ou na posição da prioridade) e OPs concluídas ou
 *   canceladas saem de qualquer posição. Em SCADA, a lista de alarmes ativos
 *   ordenada por severidade tem o mesmo comportamento.
 *
 * COMPLEXIDADE (n = número de nós)
 *   Operação            Melhor   Médio   Pior    Espaço extra
 *   insere_inicio       O(1)     O(1)    O(1)    O(1)
 *   insere_fim          O(n)     O(n)    O(n)    O(1)   (sem ponteiro p/ cauda)
 *   insere_ordenado     O(1)     O(n)    O(n)    O(1)
 *   remove_valor        O(1)     O(n)    O(n)    O(1)
 *   libera              O(n)     O(n)    O(n)    O(1)
 *   Por quê: religar ponteiros custa sempre O(1) (2 atribuições). O que custa
 *   O(n) é ACHAR o lugar: a lista não tem acesso por índice, então é preciso
 *   caminhar nó a nó a partir de `inicio`. O contador "nós visitados" mostra
 *   isso na prática.
 *
 * ENDEREÇOS
 *   "@a3f0" são os 4 últimos dígitos hexadecimais do endereço real devolvido
 *   por malloc (o endereço completo é longo demais para o desenho). Eles mudam
 *   a cada execução: o que importa é ver QUE ponteiro passa a guardar QUAL
 *   endereço.
 *
 * COMO EXECUTAR
 *   ./run 02_lista_passo_a_passo               (ENTER avança cada passo)
 *   ./run 02_lista_passo_a_passo < /dev/null   (executa direto, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L   /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct No {
    int valor;
    struct No *prox;
} No;

static int modo_interativo = 0;   /* 1 quando stdin é um terminal */

/* Contadores globais para evidenciar o custo de cada operação. */
static unsigned long nos_visitados = 0;     /* quantas vezes avançamos o elo */
static unsigned long ponteiros_escritos = 0; /* quantos ponteiros foram religados */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Endereço "curto": 4 últimos dígitos hex. Só para o desenho; nunca use isso
 * para comparar ponteiros (dois nós distintos poderiam coincidir). */
static unsigned curto(const void *p) {
    return (unsigned)((uintptr_t)p & 0xFFFFu);
}

/*
 * Desenha a lista. Cada célula ocupa 9 colunas: "[10|*]-->".
 *   elo      : se não for NULL, marca com "^ elo" QUAL ponteiro elo aponta
 *              (a variável inicio ou o campo prox de um nó).
 *   destaque : nó desenhado com chaves {25|*} (recém-inserido ou alvo).
 * O(n): percorre a lista uma vez por linha desenhada (não entra nos contadores).
 */
static void desenha(No *const *inicio, No *const *elo, const No *destaque) {
    if (*inicio == NULL)
        printf("    inicio = NULL");
    else
        printf("    inicio = @%04x", curto(*inicio));
    if (elo == inicio)
        printf("      <== elo aponta para a variável 'inicio'");
    printf("\n");

    if (*inicio == NULL) {
        printf("    (lista vazia)\n\n");
        return;
    }
    printf("      |\n      v\n");

    /* linha dos nós */
    printf("    ");
    for (const No *p = *inicio; p != NULL; p = p->prox)
        printf(p == destaque ? "{%2d|*}-->" : "[%2d|*]-->", p->valor);
    printf("NULL\n");

    /* linha dos endereços (cada um embaixo do seu nó) */
    printf("    ");
    for (const No *p = *inicio; p != NULL; p = p->prox)
        printf(" @%04x   ", curto(p));
    printf("\n");

    /* linha do conteúdo de cada campo prox (o '*' do desenho) */
    printf("    ");
    for (const No *p = *inicio; p != NULL; p = p->prox) {
        if (p->prox == NULL) printf(" p=NULL  ");
        else                 printf(" p=%04x  ", curto(p->prox));
    }
    printf("\n");

    /* linha do marcador do elo: o '*' de cada célula fica na coluna 4 dela */
    if (elo != NULL && elo != inicio) {
        int coluna = 0;
        int achou = 0;
        for (const No *p = *inicio; p != NULL; p = p->prox, coluna += 9) {
            if (elo == &p->prox) { achou = 1; break; }
        }
        if (achou)
            printf("    %*s^ elo = &(este campo prox)\n", coluna + 4, "");
    }
    printf("\n");
}

/* Mostra um nó que existe na memória mas ainda (ou já) não faz parte da lista. */
static void mostra_solto(const char *nome, const No *n) {
    if (n->prox == NULL)
        printf("    %s = @%04x --> [%2d|*]-->NULL\n", nome, curto(n), n->valor);
    else
        printf("    %s = @%04x --> [%2d|*]-->@%04x (nó [%d])\n", nome, curto(n),
               n->valor, curto(n->prox), n->prox->valor);
}

/* malloc + checagem. Em caso de falha não há como continuar a demonstração. */
static No *novo_no(int valor) {
    No *n = malloc(sizeof *n);
    if (n == NULL) {
        fprintf(stderr, "Sem memória para um novo nó.\n");
        return NULL;
    }
    n->valor = valor;
    n->prox = NULL;   /* nunca deixe um ponteiro com lixo, mesmo por um instante */
    return n;
}

/* ------------------------------------------------------------------------ */
/* INSERÇÃO NO INÍCIO — O(1)                                                 */
/* Recebe No ** porque precisa ALTERAR a variável inicio de quem chamou.     */
/* Com "No *inicio" a função mudaria só uma cópia local e o nó se perderia   */
/* (vazamento de memória clássico de aluno).                                 */
/* ------------------------------------------------------------------------ */
static int insere_inicio(No **inicio, int valor) {
    printf(">>> insere_inicio(%d)\n", valor);
    No *novo = novo_no(valor);
    if (novo == NULL) return 0;
    printf("  1) malloc criou o nó, ainda solto:\n");
    mostra_solto("novo", novo);
    pausar();

    novo->prox = *inicio;   /* PRIMEIRO liga o novo ao resto da lista...      */
    ponteiros_escritos++;
    printf("  2) novo->prox = inicio  (o novo passa a apontar para o antigo primeiro)\n");
    mostra_solto("novo", novo);
    pausar();

    *inicio = novo;         /* ...DEPOIS move o inicio. Na ordem inversa, o   */
    ponteiros_escritos++;   /* endereço do antigo primeiro seria perdido.      */
    printf("  3) inicio = novo  (2 ponteiros escritos, nenhum nó percorrido: O(1))\n");
    desenha(inicio, NULL, novo);
    pausar();
    return 1;
}

/* ------------------------------------------------------------------------ */
/* INSERÇÃO NO FIM — O(n)                                                    */
/* elo começa em &inicio e avança até achar um elo que vale NULL: é ali que  */
/* o novo nó deve ser pendurado. Se a lista está vazia, o próprio inicio é   */
/* esse elo e o mesmo código funciona.                                       */
/* ------------------------------------------------------------------------ */
static int insere_fim(No **inicio, int valor) {
    printf(">>> insere_fim(%d)\n", valor);
    unsigned long antes = nos_visitados;
    No **elo = inicio;
    printf("  elo = &inicio\n");
    desenha(inicio, elo, NULL);
    pausar();
    while (*elo != NULL) {
        printf("  *elo aponta para [%d], que não é NULL: elo = &(*elo)->prox\n", (*elo)->valor);
        elo = &(*elo)->prox;
        nos_visitados++;
        desenha(inicio, elo, NULL);
        pausar();
    }
    No *novo = novo_no(valor);
    if (novo == NULL) return 0;
    *elo = novo;            /* novo->prox já é NULL: ele vira o último */
    ponteiros_escritos++;
    printf("  *elo é NULL: achamos o fim. *elo = novo  (%lu nós percorridos: O(n))\n",
           nos_visitados - antes);
    desenha(inicio, NULL, novo);
    pausar();
    return 1;
}

/* ------------------------------------------------------------------------ */
/* INSERÇÃO ORDENADA — O(n) no pior caso                                     */
/* Avança enquanto o próximo valor for MENOR que o novo. Quando para, *elo é */
/* o primeiro nó >= valor (ou NULL) e o novo entra exatamente antes dele.    */
/* ------------------------------------------------------------------------ */
static int insere_ordenado(No **inicio, int valor) {
    printf(">>> insere_ordenado(%d)\n", valor);
    unsigned long antes = nos_visitados;
    No **elo = inicio;
    printf("  elo = &inicio\n");
    desenha(inicio, elo, NULL);
    pausar();
    while (*elo != NULL && (*elo)->valor < valor) {
        printf("  %d < %d: o novo vem depois deste nó, elo = &(*elo)->prox\n",
               (*elo)->valor, valor);
        elo = &(*elo)->prox;
        nos_visitados++;
        desenha(inicio, elo, NULL);
        pausar();
    }
    if (*elo == NULL)
        printf("  *elo é NULL: o %d é o maior e vai para o fim\n", valor);
    else
        printf("  %d >= %d: o %d entra ANTES de [%d]\n", (*elo)->valor, valor, valor,
               (*elo)->valor);

    No *novo = novo_no(valor);
    if (novo == NULL) return 0;
    novo->prox = *elo;      /* 1º: o novo aponta para o sucessor */
    ponteiros_escritos++;
    printf("  1) novo->prox = *elo\n");
    mostra_solto("novo", novo);
    pausar();
    *elo = novo;            /* 2º: o elo (inicio ou prox do antecessor) aponta para o novo */
    ponteiros_escritos++;
    printf("  2) *elo = novo  (%lu nós percorridos, 2 ponteiros escritos)\n",
           nos_visitados - antes);
    desenha(inicio, NULL, novo);
    pausar();
    return 1;
}

/* ------------------------------------------------------------------------ */
/* REMOÇÃO POR VALOR — O(n) no pior caso                                     */
/* Procura o elo que aponta para o alvo. Remover = fazer esse elo "pular" o  */
/* alvo (*elo = alvo->prox) e só então liberar o alvo.                       */
/* Retorna 1 se removeu, 0 se o valor não existe.                            */
/* ------------------------------------------------------------------------ */
static int remove_valor(No **inicio, int valor) {
    printf(">>> remove_valor(%d)\n", valor);
    unsigned long antes = nos_visitados;
    No **elo = inicio;
    printf("  elo = &inicio\n");
    desenha(inicio, elo, NULL);
    pausar();
    while (*elo != NULL && (*elo)->valor != valor) {
        printf("  [%d] != %d: elo = &(*elo)->prox\n", (*elo)->valor, valor);
        elo = &(*elo)->prox;
        nos_visitados++;
        desenha(inicio, elo, NULL);
        pausar();
    }
    if (*elo == NULL) {
        printf("  chegamos a NULL sem achar %d: nada a remover (%lu nós percorridos)\n\n",
               valor, nos_visitados - antes);
        pausar();
        return 0;
    }

    No *alvo = *elo;        /* guarda o endereço ANTES de religar */
    printf("  achou! alvo = *elo = @%04x\n", curto(alvo));
    desenha(inicio, elo, alvo);
    pausar();

    *elo = alvo->prox;      /* o elo agora pula o alvo */
    ponteiros_escritos++;
    printf("  1) *elo = alvo->prox  (o elo passa a apontar para o sucessor do alvo)\n");
    desenha(inicio, NULL, NULL);
    printf("     O alvo saiu da lista, mas ainda ocupa memória:\n");
    mostra_solto("alvo", alvo);
    pausar();

    free(alvo);             /* sem isto: vazamento de memória */
    printf("  2) free(alvo)  (%lu nós percorridos, 1 ponteiro escrito)\n", nos_visitados - antes);
    printf("     Depois do free, 'alvo' é um ponteiro pendente: NÃO use alvo->prox!\n\n");
    pausar();
    return 1;
}

/* Libera todos os nós. O(n). Guarda o próximo ANTES do free: depois do free,
 * ler atual->prox é comportamento indefinido (erro muito comum). */
static void libera(No **inicio) {
    printf(">>> libera()\n");
    No *atual = *inicio;
    while (atual != NULL) {
        No *seguinte = atual->prox;
        printf("  free([%d] @%04x)\n", atual->valor, curto(atual));
        free(atual);
        atual = seguinte;
    }
    *inicio = NULL;   /* evita que quem chamou fique com um ponteiro pendente */
    desenha(inicio, NULL, NULL);
}

static void titulo(const char *t) {
    printf("============================================================================\n");
    printf("%s\n", t);
    printf("============================================================================\n");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    No *inicio = NULL;    /* lista vazia: o primeiro elo vale NULL */
    int ok = 1;

    printf("Como ler os desenhos:\n"
           "    [10|*]-->   nó com valor 10; '*' é o campo prox  ({10|*} = nó em destaque)\n"
           "     @00d0      endereço do nó (4 últimos dígitos hex do endereço real)\n"
           "     p=00f0     valor guardado no campo prox: o endereço do nó seguinte\n"
           "        ^ elo   o ponteiro que elo (No **) aponta e que será religado\n\n");

    titulo("PARTE 1 — inserção no início: 30, 20, 10 (cada uma é O(1))");
    ok = ok && insere_inicio(&inicio, 30);
    ok = ok && insere_inicio(&inicio, 20);
    ok = ok && insere_inicio(&inicio, 10);

    titulo("PARTE 2 — inserção no fim: 40 e 50 (precisa caminhar até o NULL: O(n))");
    ok = ok && insere_fim(&inicio, 40);
    ok = ok && insere_fim(&inicio, 50);

    titulo("PARTE 3 — inserção ordenada: 25 (meio), 5 (início), 60 (fim)");
    printf("Repare: o MESMO código trata início, meio e fim. Quando o 5 entra, elo\n"
           "continua valendo &inicio, então *elo = novo altera a própria variável inicio.\n\n");
    ok = ok && insere_ordenado(&inicio, 25);
    ok = ok && insere_ordenado(&inicio, 5);
    ok = ok && insere_ordenado(&inicio, 60);

    if (ok) {
        titulo("PARTE 4 — remoção do início (5), do meio (30), do fim (60) e de valor ausente (99)");
        remove_valor(&inicio, 5);
        remove_valor(&inicio, 30);
        remove_valor(&inicio, 60);
        remove_valor(&inicio, 99);
    }

    titulo("PARTE 5 — liberando todos os nós");
    libera(&inicio);

    titulo("RESUMO");
    printf("  nós visitados ao todo : %lu\n", nos_visitados);
    printf("  ponteiros religados   : %lu\n", ponteiros_escritos);
    printf("\n"
           "  * Religar ponteiros é sempre O(1): insere = 2 escritas, remove = 1 escrita.\n"
           "  * O custo O(n) vem da BUSCA pela posição (contador de nós visitados).\n"
           "  * No **elo elimina o caso especial do primeiro nó. Sem ele, o código seria:\n"
           "        if (anterior == NULL) inicio = novo;  else anterior->prox = novo;\n"
           "    e cada função precisaria desse if (fonte comum de bugs).\n"
           "  * Outra forma de eliminar o caso especial: nó sentinela (veja 06_lista_sentinela).\n");
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
