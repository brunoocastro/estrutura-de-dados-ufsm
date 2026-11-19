/*
 * ============================================================================
 * Arquivo : 05_const.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.6 Funções)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   const num parâmetro documenta (e o compilador garante) que a função não
 *   altera os dados apontados. Assim é possível passar vetores e strings
 *   somente leitura, inclusive literais.
 *
 * EXEMPLO REAL
 *   Uma função que calcula o CRC de um quadro recebido: const uint8_t *quadro
 *   garante que o cálculo não estraga a mensagem.
 *
 * COMPLEXIDADE (n = tamanho)
 *   imprime: O(n). comprimento: O(n) (anda até o '\0', como strlen).
 *
 * COMO EXECUTAR
 *   ./run 05_const
 * ============================================================================
 */
#include <stdio.h>

/* O(n). Se tentássemos "valores[0] = 1;" aqui, a compilação falharia. */
void imprime(const int valores[], size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++)
        printf("%d%s", valores[i], i + 1 == tamanho ? "\n" : " ");
}

/* O(n): versão manual de strlen. */
size_t comprimento(const char texto[]) {
    size_t tamanho = 0;
    while (texto[tamanho] != '\0')
        tamanho++;
    return tamanho;
}

int main(void) {
    const int valores[] = {4, 8, 15, 16, 23, 42};
    const char mensagem[] = "Controle e Automacao";
    imprime(valores, sizeof valores / sizeof valores[0]);
    printf("comprimento de \"%s\" = %zu\n", mensagem, comprimento(mensagem));
    printf("comprimento de um literal = %zu\n", comprimento("UFSM"));
    printf("const protege os dados contra alteracoes dentro da funcao.\n");
    return 0;
}
