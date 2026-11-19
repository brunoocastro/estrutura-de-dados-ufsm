/*
 * ============================================================================
 * Arquivo : 14_desfazer_refazer_ihm.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidades 2.1 e 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Desfazer/Refazer (Ctrl+Z / Ctrl+Y) com DUAS pilhas, na tela de parâmetros
 *   de uma IHM de uma estufa de secagem:
 *     alterar(campo, novo) -> push em DESFAZER {campo, antigo, novo};
 *                             esvazia REFAZER (a "linha do tempo" mudou)
 *     desfazer()           -> pop de DESFAZER, volta ao valor antigo,
 *                             push em REFAZER
 *     refazer()            -> pop de REFAZER, aplica o valor novo,
 *                             push em DESFAZER
 *   As duas pilhas e o painel são redesenhados a cada ação. A pilha guarda
 *   AÇÕES (o que mudou), não cópias da tela inteira: cada entrada é pequena
 *   e a operação é O(1).
 *   No fim do roteiro, quem estiver num terminal pode testar livremente.
 *
 * EXEMPLO REAL
 *   Editores de receita/parâmetros de IHMs e supervisórios, editores de
 *   programas de CLP, editores de texto e CAD; botões Voltar/Avançar do
 *   navegador (mesmas duas pilhas, com páginas no lugar de ações).
 *
 * COMPLEXIDADE (n = ações guardadas, C = CAP_HIST)
 *   Operação                Melhor   Médio    Pior     Espaço
 *   alterar                 O(1)     O(1)     O(C)*    O(1)
 *   desfazer / refazer      O(1)     O(1)     O(1)     O(1)
 *   memória                 2 pilhas de C ações
 *   * com o histórico cheio, a ação mais ANTIGA (na base) é descartada
 *     deslocando o vetor: O(C). Remover da base não é operação de pilha; com
 *     um deque/buffer circular (../06_filas/) esse descarte também seria O(1).
 *
 * COMO EXECUTAR
 *   ./run 14_desfazer_refazer_ihm               (ENTER avança; depois modo livre)
 *   ./run 14_desfazer_refazer_ihm < /dev/null   (só o roteiro, sem pausas)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <stdlib.h>
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

/* ------------------------------------------------------------------------ */
/* Dados da IHM                                                              */
/* ------------------------------------------------------------------------ */
enum { SETPOINT, ESTEIRA, ALARME, N_CAMPOS };
static const char *nome_campo[N_CAMPOS] = {"Setpoint", "Esteira", "Alarme"};
static const char *unidade[N_CAMPOS] = {"°C", "%", "°C"};

typedef struct {
    int campo;
    int antigo, novo;
} Acao;

/* ------------------------------------------------------------------------ */
/* Pilha de ações (vetor de capacidade fixa)                                 */
/* ------------------------------------------------------------------------ */
#define CAP_HIST 4

typedef struct {
    Acao dados[CAP_HIST];
    int n;
} PilhaAcoes;

/* O(1), ou O(C) se cheia: descarta a ação mais antiga para abrir espaço.
 * Retorna 1 se precisou descartar. */
static int push(PilhaAcoes *p, Acao a) {
    int descartou = 0;
    if (p->n == CAP_HIST) {
        memmove(&p->dados[0], &p->dados[1], (CAP_HIST - 1) * sizeof p->dados[0]);
        p->n--;
        descartou = 1;
    }
    p->dados[p->n++] = a;
    return descartou;
}

/* O(1). Retorna 0 se vazia (o botão ficaria desabilitado na IHM). */
static int pop(PilhaAcoes *p, Acao *a) {
    if (p->n == 0) return 0;
    *a = p->dados[--p->n];
    return 1;
}

/* ------------------------------------------------------------------------ */
/* Estado completo do editor                                                 */
/* ------------------------------------------------------------------------ */
typedef struct {
    int valor[N_CAMPOS];
    PilhaAcoes desfazer, refazer;
} Editor;

static void formata_acao(char *buf, size_t tam, const Acao *a) {
    snprintf(buf, tam, "%-8s %4d -> %-4d", nome_campo[a->campo], a->antigo, a->novo);
}

/* O(C): painel + as duas pilhas na vertical, lado a lado. */
static void desenha(const Editor *e) {
    printf("\n   PAINEL:  ");
    for (int c = 0; c < N_CAMPOS; c++)
        printf("%s = %d %s%s", nome_campo[c], e->valor[c], unidade[c],
               c < N_CAMPOS - 1 ? "  |  " : "\n\n");
    printf("      DESFAZER (%d/%d)%14s REFAZER (%d/%d)\n", e->desfazer.n, CAP_HIST, "",
           e->refazer.n, CAP_HIST);
    for (int i = CAP_HIST - 1; i >= 0; i--) {
        char a[40] = "", b[40] = "";
        if (i < e->desfazer.n) formata_acao(a, sizeof a, &e->desfazer.dados[i]);
        if (i < e->refazer.n) formata_acao(b, sizeof b, &e->refazer.dados[i]);
        printf("   | %-22s |%-7s | %-22s |%s\n", a,
               i == e->desfazer.n - 1 ? " <-topo" : "", b,
               i == e->refazer.n - 1 ? " <-topo" : "");
    }
    printf("   +------------------------+        +------------------------+\n");
}

/* O(1) (O(C) se o histórico estiver cheio). */
static void alterar(Editor *e, int campo, int novo) {
    Acao a = {campo, e->valor[campo], novo};
    printf("\n>> Operador altera %s: %d -> %d %s\n", nome_campo[campo], a.antigo, novo,
           unidade[campo]);
    e->valor[campo] = novo;
    if (push(&e->desfazer, a))
        printf("   Histórico cheio: a ação mais antiga saiu pela BASE (deslocamento O(C)).\n");
    printf("   push em DESFAZER");
    if (e->refazer.n > 0)
        printf("; REFAZER descartado (%d ação(ões)): a linha do tempo mudou", e->refazer.n);
    printf("\n");
    e->refazer.n = 0; /* esvaziar uma pilha de vetor é O(1): basta zerar n */
}

/* O(1). */
static void desfazer(Editor *e) {
    Acao a;
    printf("\n>> Operador toca DESFAZER\n");
    if (!pop(&e->desfazer, &a)) {
        printf("   Pilha DESFAZER vazia: botão desabilitado, nada acontece.\n");
        return;
    }
    e->valor[a.campo] = a.antigo;
    push(&e->refazer, a);
    printf("   pop de DESFAZER: %s volta para %d; push em REFAZER\n", nome_campo[a.campo],
           a.antigo);
}

/* O(1). */
static void refazer(Editor *e) {
    Acao a;
    printf("\n>> Operador toca REFAZER\n");
    if (!pop(&e->refazer, &a)) {
        printf("   Pilha REFAZER vazia: botão desabilitado, nada acontece.\n");
        return;
    }
    e->valor[a.campo] = a.novo;
    push(&e->desfazer, a);
    printf("   pop de REFAZER: %s vai para %d de novo; push em DESFAZER\n",
           nome_campo[a.campo], a.novo);
}

/* Modo livre: só roda em terminal. Termina com 'q' ou EOF. */
static void modo_livre(Editor *e) {
    char linha[64];
    printf("\n=== Modo livre ===\n"
           "Comandos:  a <campo 0-2> <valor>   (0=Setpoint 1=Esteira 2=Alarme)\n"
           "           u = desfazer   r = refazer   q = sair\n");
    for (;;) {
        printf("comando> ");
        fflush(stdout);
        if (fgets(linha, sizeof linha, stdin) == NULL) break; /* EOF */
        int campo, valor;
        if (linha[0] == 'q') break;
        else if (linha[0] == 'u') desfazer(e);
        else if (linha[0] == 'r') refazer(e);
        else if (sscanf(linha, "a %d %d", &campo, &valor) == 2 && campo >= 0 &&
                 campo < N_CAMPOS)
            alterar(e, campo, valor);
        else {
            printf("Comando não reconhecido.\n");
            continue;
        }
        desenha(e);
    }
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    Editor e = {.valor = {150, 50, 200}};

    printf("IHM da estufa de secagem — desfazer/refazer com duas pilhas\n");
    desenha(&e);
    pausar();

    /* Roteiro: cada linha é uma ação do operador. */
    alterar(&e, SETPOINT, 180); desenha(&e); pausar();
    alterar(&e, ESTEIRA, 60);   desenha(&e); pausar();
    alterar(&e, ALARME, 220);   desenha(&e); pausar();
    desfazer(&e);               desenha(&e); pausar();
    desfazer(&e);               desenha(&e); pausar();
    refazer(&e);                desenha(&e); pausar();
    alterar(&e, SETPOINT, 175); desenha(&e); pausar(); /* apaga o REFAZER */
    refazer(&e);                desenha(&e); pausar(); /* nada a refazer */

    printf("\n--- Histórico limitado a %d ações: o que acontece ao passar disso? ---\n",
           CAP_HIST);
    alterar(&e, ESTEIRA, 65);   desenha(&e); pausar();
    alterar(&e, ESTEIRA, 70);   desenha(&e); pausar(); /* descarta a mais antiga */

    for (int i = 0; i < CAP_HIST + 1; i++) { /* desfaz tudo e tenta mais uma */
        desfazer(&e);
        desenha(&e);
        pausar();
    }
    printf("\nObserve: o Setpoint parou em 180, não em 150. A ação 150 -> 180 foi\n"
           "descartada quando o histórico encheu, então não pode mais ser desfeita.\n");

    if (modo_interativo) modo_livre(&e);
    return 0;
}
