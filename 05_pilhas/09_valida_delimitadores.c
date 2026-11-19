/*
 * ============================================================================
 * Arquivo : 09_valida_delimitadores.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Degrau 3 da progressão de validadores (07 -> 08 -> 09 -> 10 -> 11):
 *   evolução do 08 (que via só '[' e ']'), agora com UMA pilha para validar
 *       parênteses ( )     colchetes [ ]     chaves { }
 *   Ao achar um fechamento, o programa retira o topo e confere se é o PAR
 *   correto. A ordem importa: "{[a]}" é válida, mas "{[a}]" não é.
 *   É aqui que a pilha se torna indispensável: três contadores (um por tipo)
 *   aceitariam "{[a}]", porque só contam quantidades e não lembram a ORDEM em
 *   que os delimitadores foram abertos. O programa mostra isso lado a lado.
 *
 * EXEMPLO REAL
 *   Verificação de sintaxe feita por compiladores, editores de código, leitores
 *   de JSON/XML (mensagens de equipamentos IoT, arquivos de configuração).
 *
 * COMPLEXIDADE (n = caracteres)
 *   Operação          Melhor      Médio       Pior        Espaço
 *   valida_codigo     O(1)*       O(n)        O(n)        O(n) pilha
 *   * para no primeiro erro.
 *   Por quê: cada caractere é lido uma vez; push/pop/consulta ao topo, O(1).
 *
 * CORREÇÕES EM RELAÇÃO À VERSÃO ANTERIOR
 *   - retira() em pilha vazia devolvia '\0' e o erro aparecia como "não
 *     corresponde a símbolo desconhecido"; agora o underflow é tratado como
 *     "fechamento sem abertura".
 *   - coloca() ignorava em silêncio o símbolo quando a pilha enchia, o que
 *     podia dar resultado errado; agora a falha é informada.
 *
 * COMO EXECUTAR
 *   ./run 09_valida_delimitadores
 * ============================================================================
 */
#include <stdio.h>

#define MAX 10000

char pilha[MAX];
int tamanho = 0;

/* O(1). Retorna 0 se a pilha estiver cheia (overflow). */
int coloca(char simbolo) {
    if (tamanho == MAX)
        return 0;
    pilha[tamanho++] = simbolo;
    return 1;
}

/* O(1). Retorna 0 se a pilha estiver vazia (underflow); senão grava o topo
 * em *simbolo e o remove. */
int retira(char *simbolo) {
    if (tamanho == 0)
        return 0;
    *simbolo = pilha[--tamanho];
    return 1;
}

int e_abertura(char simbolo) {
    return simbolo == '(' || simbolo == '[' || simbolo == '{';
}

int e_fechamento(char simbolo) {
    return simbolo == ')' || simbolo == ']' || simbolo == '}';
}

int corresponde(char abertura, char fechamento) {
    return (abertura == '(' && fechamento == ')') ||
           (abertura == '[' && fechamento == ']') ||
           (abertura == '{' && fechamento == '}');
}

/* O(n). Retorna 1 se todos os delimitadores estão pareados e aninhados. */
int valida_codigo(const char codigo[]) {
    tamanho = 0; /* permite reutilizar a pilha para outra expressão */

    for (int i = 0; codigo[i] != '\0'; i++) {
        char simbolo = codigo[i];

        if (e_abertura(simbolo)) {
            if (!coloca(simbolo)) {
                printf("ERRO: mais de %d delimitadores abertos (pilha cheia).\n", MAX);
                return 0;
            }
            printf("posicao %2d: coloca '%c' na pilha (tamanho = %d)\n", i, simbolo,
                   tamanho);
        } else if (e_fechamento(simbolo)) {
            char abertura;
            if (!retira(&abertura)) {
                printf("posicao %2d: ERRO: '%c' fecha algo que nunca foi aberto.\n", i,
                       simbolo);
                return 0;
            }
            printf("posicao %2d: retira '%c' para comparar com '%c'\n", i, abertura,
                   simbolo);
            if (!corresponde(abertura, simbolo)) {
                printf("ERRO: '%c' nao fecha '%c'.\n", simbolo, abertura);
                return 0;
            }
        }
    }

    if (tamanho != 0) {
        printf("ERRO: ainda falta fechar o delimitador '%c'.\n", pilha[tamanho - 1]);
        return 0;
    }
    return 1;
}

/* Abordagem INGÊNUA, O(n) e espaço O(1): três contadores. Detecta excesso e
 * falta de cada tipo, mas não a ORDEM de fechamento. */
int valida_com_contadores(const char codigo[]) {
    int par = 0, col = 0, cha = 0;
    for (int i = 0; codigo[i] != '\0'; i++) {
        switch (codigo[i]) {
        case '(': par++; break;
        case ')': par--; break;
        case '[': col++; break;
        case ']': col--; break;
        case '{': cha++; break;
        case '}': cha--; break;
        default: break;
        }
        if (par < 0 || col < 0 || cha < 0)
            return 0;
    }
    return par == 0 && col == 0 && cha == 0;
}

void testa(const char expressao[]) {
    printf("\nExpressao: %s\n", expressao);
    int com_pilha = valida_codigo(expressao);
    int com_contadores = valida_com_contadores(expressao);
    printf("Resultado com pilha:      %s\n", com_pilha ? "correta" : "erro de sintaxe");
    printf("Resultado com contadores: %s%s\n",
           com_contadores ? "correta" : "erro de sintaxe",
           com_pilha != com_contadores ? "   <-- os contadores ERRARAM" : "");
}

int main(void) {
    /* Exemplos do 08 (só colchetes), agora validados pela mesma pilha. */
    testa("[[a+1][b-1]]");
    testa("[[[a]");

    /* Exemplos com os três tipos de delimitadores. */
    testa("int main() { int valores[2] = {10, 20}; }");
    testa("{[a + 1] * (b - 1)}");
    testa("{[a + 1})");
    testa("int main( { return 0; }");
    testa("a + b)");

    /* O caso que justifica a pilha: as quantidades batem, a ordem não. */
    testa("{[a}]");
    testa("v[(i]) + 1");
    return 0;
}
