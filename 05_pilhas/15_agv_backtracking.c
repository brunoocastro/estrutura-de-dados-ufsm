/*
 * ============================================================================
 * Arquivo : 15_agv_backtracking.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidades 2.1 e 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Backtracking (tentativa e retorno) com PILHA EXPLÍCITA: um AGV (veículo
 *   guiado automaticamente) procura uma rota até a estação de entrega numa
 *   grade do chão de fábrica. A cada passo o mapa é redesenhado:
 *     A = posição atual do AGV (topo da pilha)    o = caminho atual (na pilha)
 *     x = beco sem saída (já desempilhado)         # = parede/máquina
 *     . = livre, ainda não visitado                E = estação de entrega
 *   Regra de cada passo: no topo, tenta os vizinhos na ordem
 *   direita, baixo, esquerda, cima. Achou um livre e não visitado -> push
 *   (avança). Nenhum -> pop (volta uma casa: é o "backtrack").
 *   A pilha É o caminho: da base ao topo, a sequência de casas que leva da
 *   origem até a posição atual. Por isso, ao chegar em E, a rota já está
 *   pronta dentro da pilha.
 *
 * EXEMPLO REAL
 *   Planejamento de rotas de AGVs/robôs móveis, exploração de labirintos,
 *   busca de soluções em quebra-cabeças (sudoku, 8 rainhas), varredura de
 *   regiões em processamento de imagem (preenchimento por inundação).
 *
 * COMPLEXIDADE (V = casas livres da grade)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   busca (DFS)     O(dist.)    O(V)        O(V)        O(V) pilha + visitados
 *   Por quê: cada casa é empilhada no máximo uma vez (marcamos "visitada") e
 *   desempilhada no máximo uma vez; cada uma testa 4 vizinhos: O(4V) = O(V).
 *   Atenção: a busca em profundidade encontra UMA rota, não necessariamente a
 *   MAIS CURTA. Para a mais curta, troca-se a pilha por uma FILA (busca em
 *   largura, ../06_filas/ e ../10_grafos/).
 *
 * COMO EXECUTAR
 *   ./run 15_agv_backtracking               (ENTER avança cada passo)
 *   ./run 15_agv_backtracking < /dev/null   (executa direto)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int modo_interativo = 0; /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

#define LIN 7
#define COL 13
#define ALTURA_VISIVEL LIN /* quantos elementos da pilha cabem ao lado do mapa */

/* Planta do galpão. A origem é a doca (linha 1, coluna 1). O corredor da
 * direita na linha 3 é um beco: o AGV entra, não acha saída e volta. */
static const char planta[LIN][COL + 1] = {
    "#############",
    "#.....#.....#",
    "####.##.###.#",
    "#....#....#.#",
    "#.####.##.#.#",
    "#......#..#E#",
    "#############",
};

typedef struct {
    int l, c;
} Casa;

/* Desenha o mapa com a pilha ao lado (topo em cima). O(LIN*COL + altura). */
static void desenha(char mapa[LIN][COL + 1], const Casa pilha[], int topo) {
    printf("\n");
    for (int i = 0; i < LIN; i++) {
        printf("      %s      ", mapa[i]);
        int idx = topo - 1 - i; /* linha i mostra o i-ésimo a partir do topo */
        if (i == ALTURA_VISIVEL - 1 && topo > ALTURA_VISIVEL)
            printf("|   ...   |  (+%d casas abaixo)", topo - ALTURA_VISIVEL + 1);
        else if (idx >= 0)
            printf("| (%d,%2d)  |%s", pilha[idx].l, pilha[idx].c, i == 0 ? " <- topo" : "");
        else if (i == 0)
            printf("  (pilha vazia)");
        printf("\n");
    }
    printf("      %*s      +---------+ altura = %d\n", COL, "", topo);
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    char mapa[LIN][COL + 1];
    memcpy(mapa, planta, sizeof mapa);
    int visitada[LIN][COL] = {{0}};

    Casa pilha[LIN * COL]; /* cabe a grade inteira: nunca transborda */
    int topo = 0;
    long pushes = 0, pops = 0;
    int max_altura = 0;

    /* direita, baixo, esquerda, cima */
    const int dl[4] = {0, 1, 0, -1};
    const int dc[4] = {1, 0, -1, 0};
    const char *nome_dir[4] = {"dir", "baixo", "esq", "cima"};

    Casa origem = {1, 1};
    pilha[topo++] = origem;
    pushes++;
    max_altura = 1;
    visitada[origem.l][origem.c] = 1;
    mapa[origem.l][origem.c] = 'A';

    printf("AGV procurando a estação E por backtracking (pilha explícita)\n");
    desenha(mapa, pilha, topo);
    pausar();

    int passo = 0, achou = 0;
    while (topo > 0) {
        Casa atual = pilha[topo - 1];
        if (planta[atual.l][atual.c] == 'E') {
            achou = 1;
            break;
        }
        passo++;
        char tentativas[96] = "";
        int avancou = 0;
        for (int d = 0; d < 4; d++) {
            int nl = atual.l + dl[d], nc = atual.c + dc[d];
            const char *motivo;
            if (planta[nl][nc] == '#')
                motivo = "#";
            else if (visitada[nl][nc])
                motivo = "visitada";
            else
                motivo = "livre";
            size_t usado = strlen(tentativas);
            snprintf(tentativas + usado, sizeof tentativas - usado, "%s:%s ", nome_dir[d],
                     motivo);
            if (motivo[0] == 'l') {
                visitada[nl][nc] = 1;
                mapa[atual.l][atual.c] = 'o';
                if (planta[nl][nc] != 'E') mapa[nl][nc] = 'A';
                pilha[topo++] = (Casa){nl, nc};
                pushes++;
                if (topo > max_altura) max_altura = topo;
                printf("\n  Passo %2d: em (%d,%2d) tenta %s-> push (%d,%2d)\n", passo, atual.l,
                       atual.c, tentativas, nl, nc);
                avancou = 1;
                break; /* um passo = um push; os outros vizinhos ficam para depois */
            }
        }
        if (!avancou) {
            /* Beco sem saída: desempilha. O novo topo vira a posição do AGV. */
            topo--;
            pops++;
            mapa[atual.l][atual.c] = 'x';
            printf("\n  Passo %2d: em (%d,%2d) tenta %s-> BECO: pop, volta", passo, atual.l,
                   atual.c, tentativas);
            if (topo > 0) {
                Casa volta = pilha[topo - 1];
                mapa[volta.l][volta.c] = 'A';
                printf(" para (%d,%2d)", volta.l, volta.c);
            }
            printf("\n");
        }
        desenha(mapa, pilha, topo);
        pausar();
    }

    printf("\n================ RESULTADO ================\n");
    if (achou) {
        printf("  Estação alcançada! A rota é a própria pilha, da base ao topo:\n  ");
        for (int i = 0; i < topo; i++)
            printf("(%d,%d)%s", pilha[i].l, pilha[i].c, i < topo - 1 ? " -> " : "\n");
        printf("  Comprimento da rota: %d movimentos.\n", topo - 1);
    } else {
        printf("  A pilha esvaziou: não existe rota até a estação.\n");
    }
    int livres = 0;
    for (int i = 0; i < LIN; i++)
        for (int j = 0; j < COL; j++)
            if (planta[i][j] != '#') livres++;
    printf("  Custo: %d passos, %ld push, %ld pop, altura máxima %d; casas livres V = %d.\n"
           "  Cada casa entra na pilha no máximo uma vez: push <= V, pop <= V -> O(V).\n"
           "  As casas 'x' foram becos: o AGV entrou, não achou saída e voltou.\n"
           "  A rota encontrada não é necessariamente a mais curta (isso exigiria BFS).\n",
           passo, pushes, pops, max_altura, livres);
    return 0;
}
