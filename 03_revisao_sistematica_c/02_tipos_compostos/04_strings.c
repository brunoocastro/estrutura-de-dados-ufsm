/*
 * ============================================================================
 * Arquivo : 04_strings.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.5 Strings)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   C não tem tipo "string": uma string é um vetor de char terminado por '\0'.
 *   Aqui o aluno vê byte a byte onde fica o terminador e a diferença entre
 *   comprimento lógico (strlen, O(n)) e capacidade do vetor (sizeof, O(1)).
 *
 * EXEMPLO REAL
 *   Mensagens de texto de uma IHM ou comandos AT de um modem GSM: o buffer tem
 *   capacidade fixa e o texto ocupa só uma parte, marcada pelo '\0'.
 *
 * COMPLEXIDADE (n = comprimento do texto)
 *   Operação                Custo   Por quê
 *   strlen(s)               O(n)    anda caractere a caractere até achar '\0'
 *   sizeof vetor            O(1)    conhecido na compilação (não serve p/ ponteiro)
 *   acesso s[i]             O(1)    como em qualquer vetor
 *   Espaço: capacidade do vetor (ao menos n + 1 bytes, por causa do '\0').
 *
 * COMO EXECUTAR
 *   ./run 04_strings
 * ============================================================================
 */
#include <stdio.h>
#include <string.h>

/* O(n): a mesma ideia de strlen, escrita à mão para deixar o custo visível.
 * Pré-condição: s termina em '\0' (senão o laço lê memória alheia). */
static size_t meu_strlen(const char *s) {
    size_t n = 0;
    while (s[n] != '\0')
        n++;
    return n;
}

int main(void) {
    char texto[] = "C"; /* tamanho implícito: {'C', '\0'}, portanto 2 bytes. */
    /* Inicialização caractere a caractere: o '\0' é responsabilidade NOSSA. */
    char explicita[6] = {'O', 'l', 'a', '!', '\0', '\0'};
    char capacidade[20] = "mensagem"; /* capacidade 20, conteúdo 8 + '\0'; o resto vira 0. */
    /* Literal de string fica em memória somente leitura: alterar é UB.
     * const char * faz o compilador recusar escritas acidentais. */
    const char *literal = "nao altere este literal";

    printf("=== Tamanho logico e capacidade ===\n");
    printf("texto = %s | strlen = %zu | sizeof = %zu\n", texto, strlen(texto), sizeof texto);
    printf("explicita = %s | strlen = %zu | sizeof = %zu\n", explicita, strlen(explicita), sizeof explicita);
    printf("capacidade = %s | strlen = %zu | sizeof = %zu\n", capacidade, strlen(capacidade), sizeof capacidade);
    printf("literal = %s | strlen = %zu | sizeof = %zu (tamanho do PONTEIRO, nao do texto)\n",
           literal, meu_strlen(literal), sizeof literal);

    printf("\n=== Bytes, enderecos e terminador ===\n");
    /* strlen calculado UMA vez fora do laço: chamá-lo dentro custaria O(n) a
     * cada volta (veja 06_strings_custo_oculto.c). */
    size_t comprimento = strlen(capacidade);
    for (size_t i = 0; i < sizeof capacidade; i++) {
        printf("capacidade[%2zu] = %3d ('%c') | endereco = %p%s\n",
               i, capacidade[i], capacidade[i] == '\0' ? '.' : capacidade[i],
               (void *)&capacidade[i], i == comprimento ? "  <- '\\0' que encerra a string" : "");
    }

    /* Trocar um caractere por '\0' "encurta" a string sem mexer no resto. */
    capacidade[4] = '\0';
    printf("\nDepois de capacidade[4] = '\\0': \"%s\" (strlen = %zu, sizeof continua %zu)\n",
           capacidade, strlen(capacidade), sizeof capacidade);

    printf("\nstrlen conta caracteres antes de \\0 (custo O(n)); sizeof conta a capacidade (O(1)).\n");
    printf("Para ler texto com espacos, fgets e mais segura que scanf(\"%%s\").\n");
    printf("Um literal de string nao deve ser alterado; const char * documenta isso.\n");
    return 0;
}
