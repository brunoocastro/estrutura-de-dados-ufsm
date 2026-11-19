/*
 * ============================================================================
 * Arquivo : 07_valida_chaves.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Degrau 1 da progressão de validadores (07 -> 08 -> 09 -> 10 -> 11):
 *   verificar se cada '{' tem um '}' correspondente.
 *     - '{'  -> coloca na pilha
 *     - '}'  -> retira da pilha (se estiver vazia: '}' sem abertura)
 *     - fim  -> a pilha precisa estar vazia (senão falta fechar)
 *   OBSERVE: com UM único tipo de delimitador, todo elemento da pilha é '{'.
 *   O conteúdo nunca é consultado; só a ALTURA (topo) importa. Por isso o
 *   programa compara a pilha com um simples contador e mostra que dão o mesmo
 *   resultado. A pilha passa a ser indispensável quando há vários tipos de
 *   delimitadores (09_valida_delimitadores.c).
 *
 * EXEMPLO REAL
 *   Editores de código destacam a chave que fecha o bloco atual; blocos
 *   IF/END_IF aninhados no Texto Estruturado (IEC 61131-3) de um CLP seguem a
 *   mesma regra de aninhamento.
 *
 * COMPLEXIDADE (n = caracteres do texto)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   valida_chaves   O(1)*       O(n)        O(n)        O(n) pilha
 *   com contador    O(1)*       O(n)        O(n)        O(1)
 *   * para no primeiro '}' sem abertura.
 *   Por quê: cada caractere é visto uma vez e cada push/pop é O(1).
 *
 * COMO EXECUTAR
 *   ./run 07_valida_chaves
 * ============================================================================
 */
#include <stdio.h>

#define CAPACIDADE 100

/* O(n). Retorna 1 se as chaves estão balanceadas, 0 caso contrário. */
int valida_chaves(const char codigo[]) {
    char pilha[CAPACIDADE];
    int topo = 0; /* próxima posição livre */

    for (size_t i = 0; codigo[i] != '\0'; i++) {
        if (codigo[i] == '{') {
            if (topo == CAPACIDADE) {
                printf("caractere %zu: aninhamento maior que %d (pilha cheia)\n", i,
                       CAPACIDADE);
                return 0;
            }
            pilha[topo++] = '{';
            printf("caractere %2zu: '{' -> coloca na pilha     (altura = %d)\n", i, topo);
        } else if (codigo[i] == '}') {
            if (topo == 0) {
                printf("caractere %2zu: '}' sem abertura correspondente\n", i);
                return 0;
            }
            char abertura = pilha[--topo];
            printf("caractere %2zu: '}' -> retira '%c' da pilha (altura = %d)\n", i,
                   abertura, topo);
        }
    }
    if (topo != 0)
        printf("fim do texto: sobraram %d '{' sem fechamento\n", topo);
    return topo == 0;
}

/* O(n), espaço O(1): a mesma verificação só com um inteiro. Funciona porque
 * todos os elementos da pilha seriam iguais. */
int valida_chaves_contador(const char codigo[]) {
    int abertas = 0;
    for (size_t i = 0; codigo[i] != '\0'; i++) {
        if (codigo[i] == '{')
            abertas++;
        else if (codigo[i] == '}' && --abertas < 0)
            return 0; /* fechou mais do que abriu até aqui */
    }
    return abertas == 0;
}

static void testa(const char *titulo, const char *codigo) {
    printf("\n=== %s ===\n%s\n", titulo, codigo);
    int pela_pilha = valida_chaves(codigo);
    int pelo_contador = valida_chaves_contador(codigo);
    printf("Resultado com pilha:    %s\n", pela_pilha ? "chaves balanceadas" : "ERRO");
    printf("Resultado com contador: %s\n", pelo_contador ? "chaves balanceadas" : "ERRO");
}

int main(void) {
    testa("Codigo valido", "int main() { if (x > 0) { return 1; } return 0; }");
    testa("Falta uma '}'", "int main() { if (x > 0) { return 1; }");
    testa("'}' antes de '{'", "} int main() { return 0; {");

    printf("\nConclusao: com um so tipo de delimitador, um contador basta.\n"
           "Veja 09_valida_delimitadores.c: com ( [ { misturados, o contador falha\n"
           "e a pilha passa a ser necessaria.\n");
    return 0;
}
