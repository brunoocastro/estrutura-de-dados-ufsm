/*
 * ============================================================================
 * Arquivo : 11_valida_codigo_c.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidades 2.1 e 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Degrau 5 (último) da progressão de validadores (07 -> 08 -> 09 -> 10 -> 11):
 *   um "mini compilador" que analisa um trecho real de código C e usa uma
 *   pilha de structs (símbolo + linha + coluna) para validar ( ) [ ] { }.
 *   Guardar a POSIÇÃO de cada abertura permite mensagens como as de um
 *   compilador: "a '{' aberta na linha 2, coluna 21 não foi fechada".
 *   Delimitadores dentro de strings ("..."), de caracteres ('(') e de
 *   comentários não fazem parte da estrutura do código e são ignorados; para
 *   isso o laço é uma pequena MÁQUINA DE ESTADOS (código normal, string,
 *   caractere, comentário de linha, comentário de bloco).
 *   Não é um compilador completo: valida apenas o aninhamento.
 *
 * EXEMPLO REAL
 *   Primeira etapa (análise léxica/sintática) de compiladores e de editores
 *   de programas de CLP em Texto Estruturado, que acusam blocos não fechados
 *   antes mesmo de enviar o programa ao controlador.
 *
 * COMPLEXIDADE (n = caracteres do código)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   valida_codigo   O(1)*       O(n)        O(n)        O(CAPACIDADE)
 *   * para no primeiro erro.
 *   Por quê: um único passe sobre o texto; push/pop O(1). A pilha é um vetor
 *   de struct reservado na pilha de chamadas da função (CAPACIDADE fixa).
 *
 * CORREÇÃO EM RELAÇÃO À VERSÃO ANTERIOR
 *   Literais de caractere não eram reconhecidos: em  if (c == '(')  o '('
 *   entre aspas simples era empilhado e o código válido era rejeitado.
 *
 * COMO EXECUTAR
 *   ./run 11_valida_codigo_c
 * ============================================================================
 */
#include <stdio.h>
#include <string.h>

#define CAPACIDADE 500
#define TAMANHO_LINHA 200

typedef struct {
    char simbolo;
    int linha;
    int coluna;
} Delimitador;

typedef struct {
    Delimitador dados[CAPACIDADE];
    size_t topo;
} PilhaDelimitadores;

void inicializa(PilhaDelimitadores *pilha) {
    pilha->topo = 0;
}

int coloca(PilhaDelimitadores *pilha, char simbolo, int linha, int coluna) {
    if (pilha->topo == CAPACIDADE)
        return 0;
    pilha->dados[pilha->topo++] = (Delimitador){simbolo, linha, coluna};
    return 1;
}

int retira(PilhaDelimitadores *pilha, Delimitador *delimitador) {
    if (pilha->topo == 0)
        return 0;
    *delimitador = pilha->dados[--pilha->topo];
    return 1;
}

int corresponde(char abertura, char fechamento) {
    return (abertura == '(' && fechamento == ')') ||
           (abertura == '[' && fechamento == ']') ||
           (abertura == '{' && fechamento == '}');
}

void mostra_erro(int linha, int coluna, const char *mensagem) {
    printf("ERRO DE SINTAXE na linha %d, coluna %d: %s\n", linha, coluna, mensagem);
}

/* O(n): um único passe pelo código. Retorna 1 se os delimitadores estão
 * corretamente aninhados fora de strings, caracteres e comentários. */
int valida_codigo(const char *codigo) {
    PilhaDelimitadores pilha;
    inicializa(&pilha);
    int linha = 1;
    int coluna = 0;
    char aspas = 0; /* 0 fora de literal; '"' em string; '\'' em caractere */
    int dentro_comentario_linha = 0;
    int dentro_comentario_bloco = 0;
    int escapado = 0; /* o caractere anterior, dentro do literal, era '\' */

    for (size_t i = 0; codigo[i] != '\0'; i++) {
        char caractere = codigo[i];
        char proximo = codigo[i + 1];
        coluna++;

        if (caractere == '\n') {
            linha++;
            coluna = 0;
            dentro_comentario_linha = 0;
            continue;
        }
        if (dentro_comentario_linha)
            continue;
        if (dentro_comentario_bloco) {
            if (caractere == '*' && proximo == '/') {
                dentro_comentario_bloco = 0;
                i++;
                coluna++;
            }
            continue;
        }
        if (aspas != 0) {
            /* Dentro de "..." ou '...': só procuramos a aspa que fecha. Uma
             * barra invertida "protege" o próximo caractere: em "a\"b" a aspa
             * do meio não fecha a string. */
            if (escapado)
                escapado = 0;
            else if (caractere == '\\')
                escapado = 1;
            else if (caractere == aspas)
                aspas = 0;
            continue;
        }
        if (caractere == '"' || caractere == '\'') {
            aspas = caractere;
            continue;
        }
        if (caractere == '/' && proximo == '/') {
            dentro_comentario_linha = 1;
            i++;
            coluna++;
            continue;
        }
        if (caractere == '/' && proximo == '*') {
            dentro_comentario_bloco = 1;
            i++;
            coluna++;
            continue;
        }

        if (caractere == '(' || caractere == '[' || caractere == '{') {
            if (!coloca(&pilha, caractere, linha, coluna)) {
                mostra_erro(linha, coluna, "pilha de delimitadores cheia");
                return 0;
            }
            printf("linha %d, coluna %d: coloca '%c' na pilha\n",
                   linha, coluna, caractere);
        } else if (caractere == ')' || caractere == ']' || caractere == '}') {
            Delimitador abertura;
            if (!retira(&pilha, &abertura)) {
                mostra_erro(linha, coluna, "fechamento sem abertura correspondente");
                return 0;
            }
            printf("linha %d, coluna %d: retira '%c' da pilha para comparar com '%c'\n",
                   linha, coluna, abertura.simbolo, caractere);
            if (!corresponde(abertura.simbolo, caractere)) {
                mostra_erro(linha, coluna, "delimitadores nao correspondem");
                printf("Abertura '%c' ocorreu na linha %d, coluna %d.\n",
                       abertura.simbolo, abertura.linha, abertura.coluna);
                return 0;
            }
        }
    }

    if (aspas != 0) {
        mostra_erro(linha, coluna, aspas == '"' ? "string nao terminada"
                                                : "caractere nao terminado");
        return 0;
    }
    if (dentro_comentario_bloco) {
        mostra_erro(linha, coluna, "comentario de bloco nao terminado");
        return 0;
    }
    if (pilha.topo != 0) {
        Delimitador abertura;
        retira(&pilha, &abertura);
        mostra_erro(abertura.linha, abertura.coluna, "delimitador aberto nao foi fechado");
        printf("O '%c' aberto ali chegou ao fim do codigo ainda na pilha.\n",
               abertura.simbolo);
        return 0;
    }
    return 1;
}

int main(void) {
    const char *codigo_correto =
        "int main(void) {\n"
        "    int valores[2] = {10, 20}; /* comentario com } solto */\n"
        "    char c = '(';              // caractere '(' nao conta\n"
        "    printf(\"valor = %d {\\n\", valores[0]);\n"
        "    return c == ')';\n"
        "}\n";

    const char *codigo_incorreto =
        "int main(void) {\n"
        "    if (valor[0] > 0) {\n"
        "        printf(\"aberto\\n\");\n"
        "    ]\n"
        "}\n";

    const char *codigo_incompleto =
        "void liga_motor(int habilita) {\n"
        "    if (habilita) {\n"
        "        saida[3] = 1;\n"
        "}\n";

    printf("=== Codigo correto ===\n%s\n", codigo_correto);
    if (valida_codigo(codigo_correto))
        printf("Resultado: sintaxe de delimitadores valida.\n");

    printf("\n=== Codigo incorreto ===\n%s\n", codigo_incorreto);
    if (!valida_codigo(codigo_incorreto))
        printf("Resultado: compilador simulado interrompeu por erro de sintaxe.\n");

    printf("\n=== Codigo incompleto ===\n%s\n", codigo_incompleto);
    if (!valida_codigo(codigo_incompleto))
        printf("Resultado: compilador simulado interrompeu por erro de sintaxe.\n");
    return 0;
}

