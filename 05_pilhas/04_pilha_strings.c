/*
 * ============================================================================
 * Arquivo : 04_pilha_strings.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.2)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Pilha cujos elementos são strings. Cada posição do vetor é, ela mesma,
 *   um vetor de char:   char pilha[CAPACIDADE][TAMANHO_STRING];
 *   A primeira dimensão diz quantas strings cabem; a segunda, quantos bytes
 *   cada uma pode ocupar, INCLUINDO o terminador '\0'. Não usa struct nem
 *   malloc: toda a memória é reservada antecipadamente.
 *
 * EXEMPLO REAL
 *   Pilha de mensagens de diagnóstico de um equipamento (a mais recente é a
 *   primeira exibida no painel).
 *
 * COMPLEXIDADE (n = strings na pilha, L = TAMANHO_STRING)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   coloca          O(L)        O(L)        O(L)        O(1)
 *   retira          O(L)        O(L)        O(L)        O(1)
 *   memória total   O(CAPACIDADE * L), reservada mesmo sem uso.
 *   Por quê: a pilha continua O(1) em número de elementos, mas COPIAR uma
 *   string custa proporcional ao seu tamanho. Guardar ponteiros (char *) em
 *   vez de cópias evitaria a cópia, mas exigiria cuidar de quem é dono da
 *   memória de cada texto.
 *
 * ARMADILHA
 *   strncpy não coloca '\0' quando a origem é longa demais; por isso o
 *   código grava o terminador manualmente na última posição.
 *
 * COMO EXECUTAR
 *   ./run 04_pilha_strings
 * ============================================================================
 */
#include <stdio.h>
#include <string.h>

#define CAPACIDADE 4
#define TAMANHO_STRING 50

char pilha[CAPACIDADE][TAMANHO_STRING];
int topo = 0;

int coloca(const char texto[]) {
    if (topo == CAPACIDADE)
        return 0;
    /*
     * strncpy copia no maximo TAMANHO_STRING - 1 caracteres. Reservamos um
     * byte para o terminador '\0', pois strncpy pode nao adiciona-lo quando
     * a origem e longa demais. A atribuicao seguinte garante que a string
     * armazenada sempre termine corretamente.
     */
    strncpy(pilha[topo], texto, TAMANHO_STRING - 1);
    pilha[topo][TAMANHO_STRING - 1] = '\0';
    topo++;
    return 1;
}

/* O(L): copia a string do topo para `texto` e a retira da pilha.
 * Pré-condição: `texto` tem pelo menos TAMANHO_STRING bytes (strcpy não
 * confere o tamanho do destino). Retorna 0 em pilha vazia. */
int retira(char texto[]) {
    if (topo == 0)
        return 0;
    topo--;
    strcpy(texto, pilha[topo]);
    return 1;
}

void mostra(void) {
    printf("Pilha de strings (topo para base):\n");
    if (topo == 0) {
        printf("  [vazia]\n");
        return;
    }
    for (int i = topo - 1; i >= 0; i--)
        printf("  pilha[%d] = \"%s\" | endereco = %p%s\n",
               i, pilha[i], (void *)pilha[i],
               i == topo - 1 ? " <- topo" : "");
}

int main(void) {
    char retirada[TAMANHO_STRING];

    printf("=== Pilha de strings ===\n");
    coloca("primeira mensagem");
    coloca("segunda mensagem");
    coloca("terceira mensagem");
    mostra();

    if (retira(retirada))
        printf("String retirada: \"%s\"\n", retirada);
    mostra();

    printf("\nColocando um texto maior que %d caracteres (sera truncado):\n",
           TAMANHO_STRING - 1);
    coloca("ALARME 0042: sobretemperatura no motor da esteira principal da linha 3");
    mostra();

    coloca("nova mensagem");
    printf("\nTentando colocar alem da capacidade (%d):\n", CAPACIDADE);
    if (!coloca("mensagem extra"))
        printf("-> overflow: \"mensagem extra\" nao coube.\n");
    mostra();

    printf("\nCada posicao reserva %d bytes para uma string.\n", TAMANHO_STRING);
    printf("Capacidade total: %d strings e %zu bytes.\n",
           CAPACIDADE, sizeof pilha);
    return 0;
}
