/*
 * ============================================================================
 * Arquivo : 12_rpn_passo_a_passo.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Ver a pilha avaliando expressões em NOTAÇÃO PÓS-FIXA (RPN, Notação
 *   Polonesa Reversa), um token por vez, com a pilha redesenhada a cada passo:
 *     - número            -> push
 *     - operador binário  -> pop do 2º operando (b), pop do 1º (a), push(a op b)
 *     - fim               -> deve sobrar exatamente 1 valor: o resultado
 *   Em RPN não há parênteses nem regras de precedência: a ORDEM dos tokens já
 *   diz o que calcular primeiro. Parte A: calculadora no estilo HP. Parte B:
 *   uma lista de instruções de CLP, em que a "pilha" guarda resultados lógicos
 *   (1/0) de blocos de contatos.
 *
 *   Este arquivo é MATERIAL DE APOIO para a pesquisa "Notação Polonesa
 *   Reversa (RPN) e Avaliação de Expressões com Pilhas". De propósito, as
 *   expressões já chegam separadas em tokens dentro do código: ler a
 *   expressão como texto, separar os tokens e validá-los faz parte do
 *   trabalho de pesquisa e implementação.
 *
 * EXEMPLO REAL
 *   - Calculadoras HP (HP-12C, HP-48...): digita-se 3 ENTER 4 + 5 x.
 *   - CLPs: a lista de instruções (IL) da família Mitsubishi FX usa LD para
 *     abrir um novo bloco lógico e ORB/ANB para combinar os dois blocos do
 *     topo; o compilador de Ladder gera exatamente essa sequência pós-fixa.
 *   - Máquinas virtuais de pilha (Java bytecode, WebAssembly, PostScript).
 *
 * COMPLEXIDADE (n = tokens)
 *   Operação         Melhor      Médio       Pior        Espaço
 *   cada token       O(1)        O(1)        O(1)        -
 *   avaliar tudo     O(n)        O(n)        O(n)        O(n) pilha
 *   Por quê: cada token é lido uma vez e causa no máximo 2 pops e 1 push.
 *   A altura máxima da pilha é no máximo o número de operandos.
 *
 * COMO EXECUTAR
 *   ./run 12_rpn_passo_a_passo               (ENTER avança cada passo)
 *   ./run 12_rpn_passo_a_passo < /dev/null   (executa direto)
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
/* Pilha de double em vetor                                                  */
/* ------------------------------------------------------------------------ */
#define CAPACIDADE 16

typedef struct {
    double dados[CAPACIDADE];
    int n;        /* quantidade de elementos */
    int max_n;    /* maior altura atingida (espaço realmente usado) */
    long pushes, pops;
} Pilha;

/* O(1). Retorna 0 em overflow. */
static int push(Pilha *p, double v) {
    if (p->n == CAPACIDADE) return 0;
    p->dados[p->n++] = v;
    p->pushes++;
    if (p->n > p->max_n) p->max_n = p->n;
    return 1;
}

/* O(1). Retorna 0 em underflow. */
static int pop(Pilha *p, double *v) {
    if (p->n == 0) return 0;
    *v = p->dados[--p->n];
    p->pops++;
    return 1;
}

/* O(n): pilha na vertical, topo em cima. `como_bool` imprime os valores como
 * 0/1 (parte B, CLP) em vez de números reais (parte A). */
static void desenha(const Pilha *p, int como_bool) {
    if (p->n == 0) {
        printf("          (pilha vazia)\n");
        return;
    }
    for (int i = p->n - 1; i >= 0; i--) {
        if (como_bool)
            printf("          | %8d |%s\n", (int)p->dados[i], i == p->n - 1 ? "  <- topo" : "");
        else
            printf("          | %8g |%s\n", p->dados[i], i == p->n - 1 ? "  <- topo" : "");
    }
    printf("          +----------+\n");
}

/* ------------------------------------------------------------------------ */
/* Parte A: expressões aritméticas em RPN                                    */
/* ------------------------------------------------------------------------ */
/* printf("%-26s") conta BYTES, mas em UTF-8 "ã" ocupa 2 bytes e só 1
 * coluna na tela. Esta função completa com espaços contando caracteres. */
static void imprime_coluna(const char *s, int largura) {
    int colunas = 0;
    for (const unsigned char *q = (const unsigned char *)s; *q; q++)
        if ((*q & 0xC0) != 0x80) colunas++; /* bytes 10xxxxxx continuam um caractere */
    printf("%s%*s", s, largura > colunas ? largura - colunas : 0, "");
}

static int e_operador(const char *t) {
    return strlen(t) == 1 && strchr("+-*/", t[0]) != NULL;
}

/* O(n). Avalia `n_tokens` tokens já separados, desenhando cada passo.
 * Retorna 1 e grava *resultado se a expressão é válida. */
static int avalia_rpn(const char *const tokens[], int n_tokens, double *resultado) {
    Pilha p = {.n = 0};
    printf("    Passo  Token   Ação (a pilha é desenhada logo abaixo, topo em cima)\n");
    for (int i = 0; i < n_tokens; i++) {
        const char *t = tokens[i];
        printf("\n    %-6d %-7s ", i + 1, t);
        if (e_operador(t)) {
            double a, b;
            /* A ORDEM dos pops importa: o topo é o SEGUNDO operando.
             * Em "10 4 -" o topo é 4, então o cálculo é 10 - 4, não 4 - 10. */
            if (!pop(&p, &b) || !pop(&p, &a)) {
                printf("ERRO: '%s' precisa de 2 operandos e a pilha não tem\n", t);
                return 0;
            }
            double r = 0;
            switch (t[0]) {
            case '+': r = a + b; break;
            case '-': r = a - b; break;
            case '*': r = a * b; break;
            case '/':
                if (b == 0) {
                    printf("ERRO: divisão por zero (%g / %g)\n", a, b);
                    return 0;
                }
                r = a / b;
                break;
            default: break;
            }
            printf("pop %g (b), pop %g (a), push %g %s %g = %g\n", b, a, a, t, b, r);
            push(&p, r); /* nunca falha: acabamos de liberar 2 posições */
        } else {
            char *fim;
            double v = strtod(t, &fim);
            if (fim == t || *fim != '\0') {
                printf("ERRO: token inválido\n");
                return 0;
            }
            if (!push(&p, v)) {
                printf("ERRO: pilha cheia (%d níveis)\n", CAPACIDADE);
                return 0;
            }
            printf("é número -> push %g\n", v);
        }
        desenha(&p, 0);
        pausar();
    }
    printf("\n    Fim dos tokens: %d valor(es) na pilha.\n", p.n);
    printf("    Custo: %d tokens, %ld push, %ld pop, altura máxima %d.\n", n_tokens, p.pushes,
           p.pops, p.max_n);
    if (p.n != 1) {
        printf("    ERRO: deveria sobrar exatamente 1 valor (faltam operadores).\n");
        return 0;
    }
    return pop(&p, resultado);
}

typedef struct {
    const char *titulo;
    const char *infixa;   /* só para comparação na tela */
    const char *teclas;   /* como seria digitado numa HP */
    const char *tokens[16];
    int n;
} ExemploRPN;

static void parte_a(void) {
    const ExemploRPN exemplos[] = {
        {"Clássico da HP", "(3 + 4) * 5", "3 ENTER 4 + 5 x",
         {"3", "4", "+", "5", "*"}, 5},
        {"Transmissor 4-20 mA -> temperatura (faixa 0 a 250 °C), I = 12 mA",
         "(12 - 4) * 250 / 16", "12 ENTER 4 - 250 x 16 /",
         {"12", "4", "-", "250", "*", "16", "/"}, 7},
        {"Ordem dos operandos importa", "10 - (6 / 2)", "10 ENTER 6 ENTER 2 / -",
         {"10", "6", "2", "/", "-"}, 5},
        {"Expressão inválida: operandos insuficientes", "(?) 5 +", "5 +",
         {"5", "+"}, 2},
        {"Expressão inválida: sobram operandos", "(?) 1 2 3 +", "1 ENTER 2 ENTER 3 +",
         {"1", "2", "3", "+"}, 4},
    };
    const int total = (int)(sizeof exemplos / sizeof exemplos[0]);

    printf("PARTE A — Avaliação de RPN com pilha (estilo calculadora HP)\n");
    for (int k = 0; k < total; k++) {
        const ExemploRPN *e = &exemplos[k];
        printf("\n==== A.%d %s ====\n", k + 1, e->titulo);
        printf("    infixa : %s\n    RPN    :", e->infixa);
        for (int i = 0; i < e->n; i++) printf(" %s", e->tokens[i]);
        printf("\n    na HP  : %s\n\n", e->teclas);
        double r;
        if (avalia_rpn(e->tokens, e->n, &r))
            printf("    RESULTADO = %g\n", r);
        else
            printf("    Expressão rejeitada.\n");
        pausar();
    }
}

/* ------------------------------------------------------------------------ */
/* Parte B: lista de instruções de CLP (lógica booleana pós-fixa)            */
/* ------------------------------------------------------------------------ */
typedef enum { LD, LDI, AND, ANI, OR, ORI, ANB, ORB, OUT } Opcode;

typedef struct {
    Opcode op;
    int endereco; /* índice da entrada X ou saída Y; -1 se não usa */
    const char *comentario;
} Instrucao;

static const char *nome_op[] = {"LD", "LDI", "AND", "ANI", "OR", "ORI", "ANB", "ORB", "OUT"};

/* O(n). Executa um "scan" do programa: cada instrução é O(1).
 *   LD/LDI x  : abre bloco novo  -> push(x) / push(!x)
 *   AND/ANI x : topo = topo AND x / topo AND !x
 *   OR/ORI x  : topo = topo OR x  / topo OR !x
 *   ANB/ORB   : pop b, pop a, push(a AND b) / push(a OR b)
 *   OUT y     : y = topo (não remove)                                      */
static void executa_scan(const Instrucao prog[], int n, const int X[], int Y[]) {
    Pilha p = {.n = 0};
    double a, b;
    for (int i = 0; i < n; i++) {
        const Instrucao *in = &prog[i];
        int x = in->endereco >= 0 ? X[in->endereco] : 0;
        printf("\n    %-3s ", nome_op[in->op]);
        if (in->op == ANB || in->op == ORB)
            printf("     ");
        else
            printf("%c%-3d ", in->op == OUT ? 'Y' : 'X', in->endereco);
        printf("; ");
        imprime_coluna(in->comentario, 26);
        printf(" ");
        switch (in->op) {
        case LD:  push(&p, x);  printf("push X%d = %d\n", in->endereco, x); break;
        case LDI: push(&p, !x); printf("push NOT X%d = %d\n", in->endereco, !x); break;
        case AND: case ANI: case OR: case ORI: {
            if (!pop(&p, &a)) { printf("ERRO: pilha vazia\n"); return; }
            int v = in->op == ANI || in->op == ORI ? !x : x;
            int r = in->op == AND || in->op == ANI ? ((int)a && v) : ((int)a || v);
            push(&p, r);
            printf("topo = %d %s %d = %d\n", (int)a,
                   in->op == AND || in->op == ANI ? "AND" : "OR", v, r);
            break;
        }
        case ANB: case ORB:
            if (!pop(&p, &b) || !pop(&p, &a)) { printf("ERRO: faltam blocos\n"); return; }
            push(&p, in->op == ANB ? ((int)a && (int)b) : ((int)a || (int)b));
            printf("pop %d, pop %d, push %d %s %d = %d\n", (int)b, (int)a, (int)a,
                   in->op == ANB ? "AND" : "OR", (int)b, (int)p.dados[p.n - 1]);
            break;
        case OUT:
            if (p.n == 0) { printf("ERRO: pilha vazia\n"); return; }
            Y[in->endereco] = (int)p.dados[p.n - 1];
            printf("Y%d = topo = %d\n", in->endereco, Y[in->endereco]);
            break;
        }
        desenha(&p, 1);
        pausar();
    }
    printf("\n    Scan: %d instruções, %ld push, %ld pop, altura máxima %d.\n", n, p.pushes,
           p.pops, p.max_n);
}

static void parte_b(void) {
    printf("\n\nPARTE B — Lista de instruções de CLP (estilo Mitsubishi FX)\n\n");
    printf("    Bomba de recalque Y0 liga se:\n"
           "      (botão local X0 E chave em LOCAL X1)  OU  (comando SCADA X2 E NÃO falha X3)\n\n");
    printf("    Ladder:          X0       X1\n"
           "           ||-------| |------| |-----+-------( Y0 )\n"
           "           ||        X2       X3     |\n"
           "           ||-------| |------|/|-----+\n\n");
    printf("    RPN equivalente: X0 X1 AND X2 X3 NOT AND OR\n");

    const Instrucao programa[] = {
        {LD, 0, "botão liga local"},
        {AND, 1, "chave em LOCAL"},
        {LD, 2, "comando remoto (SCADA)"},
        {ANI, 3, "contato NF de falha"},
        {ORB, -1, "junta os dois ramos"},
        {OUT, 0, "bomba de recalque"},
    };
    const int n = (int)(sizeof programa / sizeof programa[0]);

    int X[4] = {1, 0, 1, 0}; /* local sem chave em LOCAL; remoto pedindo; sem falha */
    int Y[1] = {0};
    printf("\n    Entradas: X0=%d X1=%d X2=%d X3=%d\n", X[0], X[1], X[2], X[3]);
    executa_scan(programa, n, X, Y);
    printf("    => Bomba %s pelo ramo remoto.\n", Y[0] ? "LIGADA" : "desligada");
    pausar();

    X[3] = 1; /* surge a falha */
    printf("\n    Novo scan com falha ativa: X0=%d X1=%d X2=%d X3=%d\n", X[0], X[1], X[2], X[3]);
    executa_scan(programa, n, X, Y);
    printf("    => Bomba %s.\n", Y[0] ? "LIGADA" : "DESLIGADA (proteção atuou)");
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);
    parte_a();
    parte_b();
    printf("\nResumo: a pilha guarda resultados parciais na ordem em que serão usados;\n"
           "cada token custa O(1), a expressão inteira O(n). Para transformar uma\n"
           "expressão infixa em RPN, veja 13_infixa_posfixa_shunting_yard.c.\n");
    return 0;
}
