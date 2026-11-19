/*
 * ============================================================================
 * Arquivo : 10_delimitadores_passo_a_passo.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidades 2.1 e 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Degrau 4 da progressão de validadores (07 -> 08 -> 09 -> 10 -> 11):
 *   o mesmo algoritmo do 09, agora VISUAL. A cada delimitador o programa
 *   marca o caractere lido com ^, narra a decisão e redesenha a pilha na
 *   vertical. Em caso de erro, aponta a coluna como um compilador faz.
 *   Também é a primeira versão SEM variáveis globais: a pilha é local à
 *   função, então a função pode ser chamada de vários lugares sem uma chamada
 *   estragar a pilha da outra.
 *
 * EXEMPLO REAL
 *   Mensagens do tipo "error: expected ')' at column 14" do compilador.
 *   Validação de fórmulas digitadas pelo operador num supervisório SCADA
 *   antes de aceitá-las como tag calculada.
 *
 * COMPLEXIDADE (n = caracteres da expressão)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   balanceada      O(1)*       O(n)        O(n)        O(n) pilha
 *   * para no primeiro erro.
 *   Por quê: cada caractere é lido 1 vez; cada abertura sofre no máximo
 *   1 push e 1 pop. Os contadores no fim de cada exemplo confirmam:
 *   push + pop <= n.
 *
 * COMO EXECUTAR
 *   ./run 10_delimitadores_passo_a_passo               (ENTER avança cada passo)
 *   ./run 10_delimitadores_passo_a_passo < /dev/null   (executa direto)
 * ============================================================================
 */
#define _POSIX_C_SOURCE 200809L /* antes de qualquer #include: libera isatty() em -std=c11 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CAPACIDADE 64

static int modo_interativo = 0; /* 1 quando stdin é um terminal */

/* Pausa até o aluno teclar ENTER; não faz nada se a entrada não for um terminal. */
static void pausar(void) {
    if (!modo_interativo) return;
    printf("    [ENTER] próximo passo...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* Contadores para evidenciar o custo O(n). */
typedef struct {
    size_t lidos, pushes, pops;
} Contadores;

static int corresponde(char abertura, char fechamento) {
    return (abertura == '(' && fechamento == ')') ||
           (abertura == '[' && fechamento == ']') ||
           (abertura == '{' && fechamento == '}');
}

/* Desenha a pilha na vertical, topo em cima. O(altura). */
static void desenha_pilha(const char pilha[], size_t topo) {
    printf("        |     |\n");
    for (size_t i = topo; i > 0; i--)
        printf("        |  %c  |%s\n", pilha[i - 1], i == topo ? "  <- topo" : "");
    printf("        +-----+   altura = %zu\n", topo);
}

/* Imprime a expressão e um ^ embaixo da posição `pos`. */
static void marca_posicao(const char *texto, size_t pos) {
    printf("\n    %s\n    %*s^\n", texto, (int)pos, "");
}

/* O(n). Percorre `texto` desenhando cada passo. Retorna 1 se balanceada.
 * A pilha é LOCAL: cada chamada tem a sua (função reentrante). */
static int balanceada(const char *texto, Contadores *cont) {
    char pilha[CAPACIDADE];
    size_t topo = 0;
    size_t passo = 0;

    for (size_t i = 0; texto[i] != '\0'; i++) {
        char c = texto[i];
        cont->lidos++;
        if (strchr("([{", c) != NULL) {
            passo++;
            marca_posicao(texto, i);
            if (topo == CAPACIDADE) {
                printf("    Passo %zu: '%c' mas a pilha está cheia (%d) -> ERRO\n", passo, c,
                       CAPACIDADE);
                return 0;
            }
            pilha[topo++] = c;
            cont->pushes++;
            printf("    Passo %zu: '%c' é abertura -> push('%c')\n", passo, c, c);
            desenha_pilha(pilha, topo);
            pausar();
        } else if (strchr(")]}", c) != NULL) {
            passo++;
            marca_posicao(texto, i);
            if (topo == 0) {
                printf("    Passo %zu: '%c' é fechamento, mas a pilha está VAZIA\n"
                       "    -> ERRO na coluna %zu: '%c' fecha algo que nunca foi aberto\n",
                       passo, c, i + 1, c);
                desenha_pilha(pilha, topo);
                return 0;
            }
            char aberta = pilha[--topo];
            cont->pops++;
            if (!corresponde(aberta, c)) {
                printf("    Passo %zu: '%c' é fechamento; pop() devolveu '%c'\n"
                       "    -> ERRO na coluna %zu: '%c' não fecha '%c'\n",
                       passo, c, aberta, i + 1, c, aberta);
                desenha_pilha(pilha, topo);
                return 0;
            }
            printf("    Passo %zu: '%c' é fechamento; pop() devolveu '%c' -> par correto\n",
                   passo, c, aberta);
            desenha_pilha(pilha, topo);
            pausar();
        }
        /* demais caracteres: não mexem na pilha */
    }

    if (topo != 0) {
        printf("    Fim do texto com a pilha NÃO vazia -> ERRO: falta fechar '%c'\n",
               pilha[topo - 1]);
        desenha_pilha(pilha, topo);
        return 0;
    }
    printf("    Fim do texto com a pilha vazia -> tudo pareado.\n");
    return 1;
}

int main(void) {
    modo_interativo = isatty(STDIN_FILENO);

    const char *exemplos[] = {
        "{[(a+b)*c]-d}", /* correta */
        "{[(])}",        /* ordem errada: ']' tenta fechar '(' */
        "((x+1)*2",      /* sobra '(' */
        "y-3)",          /* ')' sem abertura */
    };
    size_t total = sizeof exemplos / sizeof exemplos[0];

    for (size_t k = 0; k < total; k++) {
        Contadores cont = {0, 0, 0};
        printf("\n==================== Exemplo %zu de %zu: \"%s\" ====================\n",
               k + 1, total, exemplos[k]);
        int ok = balanceada(exemplos[k], &cont);
        printf("    Resultado: %s\n", ok ? "BALANCEADA" : "INVÁLIDA");
        printf("    Custo: %zu caracteres lidos, %zu push, %zu pop "
               "(push + pop <= n = %zu)\n",
               cont.lidos, cont.pushes, cont.pops, strlen(exemplos[k]));
        pausar();
    }

    printf("\nResumo: o algoritmo lê cada caractere uma vez e faz no máximo um push e\n"
           "um pop por delimitador: tempo O(n). No pior caso (só aberturas) a pilha\n"
           "guarda n elementos: espaço O(n).\n");
    return 0;
}
