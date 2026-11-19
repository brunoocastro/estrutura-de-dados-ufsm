/*
 * ============================================================================
 * Arquivo : 05_historico_navegador.c
 * Módulo  : 05 — Pilhas                        (Ementa: Unidade 2.1)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Primeira aplicação: o botão "voltar" de um navegador. Cada página
 *   visitada é colocada no topo; "voltar" retira a página mais recente.
 *   Nomes em português:
 *     coloca -> push      retira -> pop
 *   Para ter também o botão "avançar" são necessárias DUAS pilhas; esse é
 *   o assunto do 14_desfazer_refazer_ihm.c.
 *
 * EXEMPLO REAL
 *   Histórico de navegação do navegador; navegação entre telas de uma IHM
 *   (o botão "Voltar" da tela de alarmes retorna à tela anterior).
 *
 * COMPLEXIDADE (n = páginas, L = TAMANHO_PAGINA)
 *   Operação        Melhor      Médio       Pior        Espaço
 *   coloca/retira   O(L)        O(L)        O(L)        O(1)
 *   mostra_pilha    O(n)        O(n)        O(n)        O(1)
 *   Por quê: O(1) em número de páginas; o custo L vem da cópia do texto.
 *
 * COMO EXECUTAR
 *   ./run 05_historico_navegador
 * ============================================================================
 */
#include <stdio.h>
#include <string.h>

#define CAPACIDADE 5
#define TAMANHO_PAGINA 80

typedef struct {
    char paginas[CAPACIDADE][TAMANHO_PAGINA];
    int topo;
} PilhaPaginas;

void inicializa(PilhaPaginas *pilha) {
    pilha->topo = -1;
}

int esta_vazia(const PilhaPaginas *pilha) {
    return pilha->topo == -1;
}

int esta_cheia(const PilhaPaginas *pilha) {
    return pilha->topo == CAPACIDADE - 1;
}

/* coloca insere uma nova pagina no topo da pilha. */
int coloca(PilhaPaginas *pilha, const char pagina[]) {
    if (esta_cheia(pilha))
        return 0;
    pilha->topo++;
    strncpy(pilha->paginas[pilha->topo], pagina, TAMANHO_PAGINA - 1);
    pilha->paginas[pilha->topo][TAMANHO_PAGINA - 1] = '\0';
    return 1;
}

/* retira copia a pagina do topo para `pagina` e a remove da pilha.
 * Pré-condição: `pagina` tem pelo menos TAMANHO_PAGINA bytes. */
int retira(PilhaPaginas *pilha, char pagina[]) {
    if (esta_vazia(pilha))
        return 0;
    strcpy(pagina, pilha->paginas[pilha->topo]);
    pilha->topo--;
    return 1;
}

void mostra_pilha(const PilhaPaginas *pilha) {
    printf("\nHistorico (topo para base):\n");
    if (esta_vazia(pilha)) {
        printf("  [vazio]\n");
        return;
    }
    for (int i = pilha->topo; i >= 0; i--)
        printf("  %d: %s | endereco = %p%s\n",
               i, pilha->paginas[i], (const void *)pilha->paginas[i],
               i == pilha->topo ? " <- topo" : "");
}

int main(void) {
    PilhaPaginas historico;
    char pagina_retirada[TAMANHO_PAGINA];
    inicializa(&historico);

    printf("=== Historico do navegador ===\n");
    printf("Cada nova pagina e colocada no historico.\n");
    const char *visitas[] = {"portal.ufsm.br", "biblioteca.ufsm.br", "sistemas.ufsm.br"};
    for (size_t i = 0; i < sizeof visitas / sizeof visitas[0]; i++)
        if (!coloca(&historico, visitas[i]))
            printf("Historico cheio: %s nao foi registrada.\n", visitas[i]);
    mostra_pilha(&historico);

    printf("\nAcao: voltar\n");
    if (retira(&historico, pagina_retirada))
        printf("Pagina retirada: %s\n", pagina_retirada);
    mostra_pilha(&historico);

    printf("\nAcao: voltar novamente\n");
    if (retira(&historico, pagina_retirada))
        printf("Pagina retirada: %s\n", pagina_retirada);
    mostra_pilha(&historico);

    printf("\nAplicacao: o botao voltar remove sempre a pagina mais recentemente visitada.\n");
    return 0;
}
