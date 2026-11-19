/*
 * ============================================================================
 * Arquivo : 13_infixa_posfixa_shunting_yard.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Converter uma expressão INFIXA (a que escrevemos: 2 + 3 * 4) para
 *   PÓS-FIXA/RPN (2 3 4 * +) com o algoritmo "shunting-yard" (pátio de
 *   manobras) de Edsger Dijkstra (1961). Cada token gera uma linha de tabela:
 *       Passo | Token | Ação | Pilha de operadores | Saída
 *   Regras:
 *     operando         -> vai direto para a saída
 *     '('              -> push
 *     ')'              -> pop para a saída até achar '('; descarta o '('
 *     operador o1      -> enquanto o topo o2 for operador e
 *                         prec(o2) > prec(o1), ou prec igual e o1 associativo
 *                         à esquerda: pop o2 para a saída. Depois push(o1).
 *     fim              -> pop de tudo para a saída ('(' sobrando = erro)
 *   Precedência: ^ (3, associativo à DIREITA)  >  * / (2)  >  + - (1).
 *   A pilha guarda os operadores que "ainda esperam" o operando da direita.
 *
 * MELHORIAS EM RELAÇÃO À VERSÃO ANTERIOR (04_conversao_posfixa.c)
 *   - números com vários dígitos e nomes de variáveis (antes "12" virava "1 2");
 *   - operador ^ associativo à direita;
 *   - detecção de parênteses desbalanceados e de operador/operando fora de
 *     lugar ("2 + * 3"), que o algoritmo clássico sozinho não detecta;
 *   - limites da pilha e da saída verificados (antes podia escrever fora do
 *     vetor com expressões longas).
 *
 * EXEMPLO REAL
 *   Compiladores e interpretadores (inclusive os de Texto Estruturado de
 *   CLPs) convertem expressões para uma forma pós-fixa ou para árvore antes
 *   de gerar código; planilhas e supervisórios fazem o mesmo com fórmulas de
 *   tags calculadas (ex.: a lei de controle PID abaixo).
 *
 * COMPLEXIDADE (n = tokens)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   converte        O(n)        O(n)        O(n)        O(n) pilha + saída
 *   Por quê: o laço "enquanto" parece aninhado, mas cada operador entra na
 *   pilha UMA vez e sai UMA vez; somando todos os passos, o total é O(n)
 *   (análise amortizada). O contador de push/pop no fim confirma.
 *
 * COMO EXECUTAR
 *   ./run 13_infixa_posfixa_shunting_yard               (ENTER avança cada passo)
 *   ./run 13_infixa_posfixa_shunting_yard < /dev/null   (executa direto)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CAP_PILHA 64
#define TAM_TOKEN 32
#define TAM_SAIDA 256

static int modo_interativo = 0; /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* printf("%-34s") conta BYTES, mas em UTF-8 "í" ocupa 2 bytes e só 1
 * coluna na tela. Esta função completa com espaços contando caracteres. */
static void imprime_coluna(const char *s, int largura) {
    int colunas = 0;
    for (const unsigned char *q = (const unsigned char *)s; *q; q++)
        if ((*q & 0xC0) != 0x80) colunas++; /* bytes 10xxxxxx continuam um caractere */
    printf("%s%*s", s, largura > colunas ? largura - colunas : 0, "");
}

static int precedencia(char op) {
    switch (op) {
    case '^': return 3;
    case '*': case '/': return 2;
    case '+': case '-': return 1;
    default: return 0; /* '(' fica com 0: nenhum operador o tira da pilha */
    }
}

static int associativo_a_direita(char op) {
    return op == '^'; /* 2^3^2 = 2^(3^2) = 512, não (2^3)^2 = 64 */
}

/* ------------------------------------------------------------------------ */
/* Estado da conversão                                                       */
/* ------------------------------------------------------------------------ */
typedef struct {
    char pilha[CAP_PILHA];
    int topo; /* quantidade de operadores na pilha */
    char saida[TAM_SAIDA];
    size_t tam_saida;
    long pushes, pops;
    int erro; /* 1 se a saída estourou */
} Conversor;

/* O(tamanho do texto): acrescenta texto + espaço à saída, com limite. */
static void emite(Conversor *c, const char *texto) {
    size_t len = strlen(texto);
    if (c->tam_saida + len + 2 > TAM_SAIDA) {
        c->erro = 1;
        return;
    }
    memcpy(c->saida + c->tam_saida, texto, len);
    c->tam_saida += len;
    c->saida[c->tam_saida++] = ' ';
    c->saida[c->tam_saida] = '\0';
}

static int empilha(Conversor *c, char op) {
    if (c->topo == CAP_PILHA) return 0;
    c->pilha[c->topo++] = op;
    c->pushes++;
    return 1;
}

static char desempilha(Conversor *c) {
    c->pops++;
    return c->pilha[--c->topo]; /* chamado só com topo > 0 */
}

/* Imprime uma linha da tabela: a pilha vai da base (esquerda) ao topo (direita). */
static void linha_tabela(int passo, const char *token, const char *acao, const Conversor *c) {
    char pilha_txt[CAP_PILHA * 2 + 1];
    size_t k = 0;
    for (int i = 0; i < c->topo; i++) {
        pilha_txt[k++] = c->pilha[i];
        pilha_txt[k++] = ' ';
    }
    pilha_txt[k] = '\0';
    printf("  %5d | %-9s | ", passo, token);
    imprime_coluna(acao, 34);
    printf(" | %-12s| %s\n", pilha_txt, c->saida);
}

/* Lê o próximo token a partir de *pos. Tokens: número (12, 3.5), nome
 * (Kp, erro_1), operador ou parêntese. Retorna 0 no fim do texto. */
static int proximo_token(const char *s, size_t *pos, char token[TAM_TOKEN]) {
    while (s[*pos] == ' ') (*pos)++;
    if (s[*pos] == '\0') return 0;
    size_t k = 0;
    unsigned char ch = (unsigned char)s[*pos];
    if (isalnum(ch) || ch == '_' || ch == '.') {
        while ((isalnum((unsigned char)s[*pos]) || s[*pos] == '_' || s[*pos] == '.') &&
               k < TAM_TOKEN - 1)
            token[k++] = s[(*pos)++];
    } else {
        token[k++] = s[(*pos)++];
    }
    token[k] = '\0';
    return 1;
}

/* O(n). Converte `infixa` para pós-fixa, desenhando a tabela. Retorna 1 se ok. */
static int converte(const char *infixa, Conversor *c) {
    char token[TAM_TOKEN], acao[3 * CAP_PILHA];
    size_t pos = 0;
    int passo = 0;
    int espera_operando = 1; /* validação extra: alterna operando/operador */

    printf("  %5s | %-9s | ", "Passo", "Token");
    imprime_coluna("Ação", 34);
    printf(" | %-12s| %s\n", "Pilha(b->t)", "Saída");
    printf("  ------+-----------+------------------------------------+-------------+------\n");

    while (proximo_token(infixa, &pos, token)) {
        char t = token[0];
        passo++;
        if (isalnum((unsigned char)t) || t == '_' || t == '.') {
            if (!espera_operando) {
                printf("  ERRO: operando '%s' sem operador antes\n", token);
                return 0;
            }
            emite(c, token);
            snprintf(acao, sizeof acao, "operando -> saída");
            espera_operando = 0;
        } else if (t == '(') {
            if (!espera_operando) {
                printf("  ERRO: '(' logo após operando (multiplicação implícita?)\n");
                return 0;
            }
            if (!empilha(c, t)) { printf("  ERRO: pilha cheia\n"); return 0; }
            snprintf(acao, sizeof acao, "'(' -> push");
        } else if (t == ')') {
            if (espera_operando) {
                printf("  ERRO: ')' logo após operador ou '('\n");
                return 0;
            }
            int movidos = 0;
            while (c->topo > 0 && c->pilha[c->topo - 1] != '(') {
                char op[2] = {desempilha(c), '\0'};
                emite(c, op);
                movidos++;
            }
            if (c->topo == 0) {
                linha_tabela(passo, token, "pilha esvaziou sem achar '('", c);
                printf("  ERRO: ')' sem '(' correspondente\n");
                return 0;
            }
            desempilha(c); /* descarta o '(' */
            snprintf(acao, sizeof acao, "pop %d op. até '('; descarta '('", movidos);
        } else if (precedencia(t) > 0 && token[1] == '\0') {
            if (espera_operando) {
                printf("  ERRO: operador '%c' sem operando à esquerda\n", t);
                return 0;
            }
            char movidos[2 * CAP_PILHA + 1]; /* operadores tirados, separados por espaço */
            size_t m = 0;
            while (c->topo > 0) {
                char o2 = c->pilha[c->topo - 1];
                int p1 = precedencia(t), p2 = precedencia(o2);
                if (o2 == '(' || !(p2 > p1 || (p2 == p1 && !associativo_a_direita(t))))
                    break;
                desempilha(c);
                movidos[m++] = o2;
                movidos[m++] = ' ';
                char op[2] = {o2, '\0'};
                emite(c, op);
            }
            movidos[m] = '\0';
            char parou = c->topo > 0 ? c->pilha[c->topo - 1] : '\0'; /* o que ficou no topo */
            if (!empilha(c, t)) { printf("  ERRO: pilha cheia\n"); return 0; }
            if (m > 0)
                snprintf(acao, sizeof acao, "pop %s(prec. >= %c); push %c", movidos, t, t);
            else if (parou == '\0')
                snprintf(acao, sizeof acao, "pilha vazia -> push %c", t);
            else if (parou == '(')
                snprintf(acao, sizeof acao, "topo é '(' -> push %c", t);
            else if (precedencia(parou) == precedencia(t))
                snprintf(acao, sizeof acao, "%c assoc. à direita -> push %c", t, t);
            else
                snprintf(acao, sizeof acao, "topo '%c' tem prec. menor -> push %c", parou, t);
            espera_operando = 1;
        } else {
            printf("  ERRO: símbolo desconhecido '%s'\n", token);
            return 0;
        }
        linha_tabela(passo, token, acao, c);
        pausar();
    }

    if (espera_operando) {
        printf("  ERRO: expressão termina sem o último operando\n");
        return 0;
    }
    while (c->topo > 0) {
        char op = desempilha(c);
        if (op == '(') {
            linha_tabela(passo + 1, "(fim)", "achou '(' sem ')'", c);
            printf("  ERRO: '(' sem ')' correspondente\n");
            return 0;
        }
        char txt[2] = {op, '\0'};
        emite(c, txt);
        linha_tabela(++passo, "(fim)", "esvazia: pop para a saída", c);
    }
    if (c->erro) {
        printf("  ERRO: saída maior que %d caracteres\n", TAM_SAIDA);
        return 0;
    }
    return 1;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    const char *exemplos[] = {
        "2 + 3 * (4 - 1)",                            /* precedência e parênteses */
        "Kp * erro + Ki * integral - Kd * derivada",  /* lei de controle PID */
        "10 - 4 - 3",                                 /* associatividade à esquerda */
        "(a + b) * c ^ 2 ^ 3",                        /* ^ associativo à direita */
        "(1 + 2",                                     /* erro: falta ')' */
        "1 + 2) * 3",                                 /* erro: sobra ')' */
        "2 + * 3",                                    /* erro: operador fora de lugar */
    };
    const int total = (int)(sizeof exemplos / sizeof exemplos[0]);

    for (int k = 0; k < total; k++) {
        Conversor c = {.topo = 0, .tam_saida = 0};
        c.saida[0] = '\0';
        printf("\n==== Exemplo %d de %d — infixa: %s ====\n\n", k + 1, total, exemplos[k]);
        if (converte(exemplos[k], &c))
            printf("\n  PÓS-FIXA: %s\n", c.saida);
        else
            printf("\n  Expressão rejeitada.\n");
        printf("  Custo: %ld push e %ld pop na pilha de operadores (cada operador entra e sai\n"
               "  no máximo uma vez -> O(n)).\n",
               c.pushes, c.pops);
        pausar();
    }

    printf("\nPara AVALIAR a expressão pós-fixa gerada, veja 12_rpn_passo_a_passo.c.\n");
    return 0;
}
