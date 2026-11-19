/*
 * ============================================================================
 * Arquivo : 05_palindromo.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 0.5 Strings, 0.6 Recursividade)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   Verificar palíndromos recursivamente: compara as pontas e reduz o
 *   intervalo [esquerda, direita] até ele ficar vazio ou com um caractere.
 *   O desenho mostra as duas pontas sendo comparadas a cada nível.
 *
 * EXEMPLO REAL
 *   Verificações de simetria aparecem em validação de dados: conferir se um
 *   quadro recebido é igual ao enviado lido de trás para frente, ou se um
 *   padrão de bits é simétrico.
 *
 * COMPLEXIDADE (n = comprimento)
 *   Tempo: O(n)  -> no máximo n/2 comparações.
 *   Pilha: O(n)  -> até n/2 quadros (é recursão de cauda: um laço usaria O(1)).
 *
 * COMO EXECUTAR
 *   ./run 05_palindromo               (pede uma palavra sem espaços)
 *   ./run 05_palindromo < /dev/null   (testa "reviver" e "automacao")
 * ============================================================================
 */
#include <stdio.h>
#include <string.h>

/* O(n). Pré-condição: 0 <= esquerda e direita < strlen(texto) ou intervalo vazio. */
static int palindromo(const char texto[], int esquerda, int direita, int nivel) {
    if (esquerda >= direita) { /* caso base: 0 ou 1 caractere restante */
        printf("%*sintervalo [%d, %d] com 0 ou 1 caractere -> e palindromo\n",
               nivel * 2, "", esquerda, direita);
        return 1;
    }
    /* desenha a palavra com as duas pontas entre colchetes */
    printf("%*s", nivel * 2, "");
    for (int i = 0; texto[i] != '\0'; i++) {
        if (i == esquerda || i == direita) printf("[%c]", texto[i]);
        else if (i > esquerda && i < direita) printf(" %c ", texto[i]);
        else printf(" . ");
    }
    printf("  compara texto[%d]='%c' com texto[%d]='%c'",
           esquerda, texto[esquerda], direita, texto[direita]);
    if (texto[esquerda] != texto[direita]) {
        printf(" -> diferentes, NAO e palindromo\n");
        return 0;
    }
    printf(" -> iguais\n");
    return palindromo(texto, esquerda + 1, direita - 1, nivel + 1);
}

static void testa(const char *texto) {
    printf("\n=== \"%s\" ===\n", texto);
    int r = palindromo(texto, 0, (int)strlen(texto) - 1, 0);
    printf("Resultado: %s\n", r ? "e palindromo" : "nao e palindromo");
}

int main(void) {
    char texto[80];
    printf("Informe uma palavra: ");
    if (scanf("%79s", texto) == 1) {
        testa(texto);
    } else {
        printf("(entrada vazia: usando exemplos)\n");
        testa("reviver");
        testa("automacao");
    }
    return 0;
}
