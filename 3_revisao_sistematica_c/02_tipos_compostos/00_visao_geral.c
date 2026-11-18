/*
 * Tipos compostos em C: arrays, matrizes, strings, structs, unions e enums.
 *
 * Compile e execute:
 *   gcc -std=c11 -Wall -Wextra -pedantic 02_tipos_compostos.c -o 02_tipos_compostos
 *   ./02_tipos_compostos
 */
#include <stdio.h>
#include <string.h>

typedef struct {
    int codigo;
    char nome[20];
    float nota;
} Aluno;

typedef union {
    int inteiro;
    float real;
    char caractere;
} Valor;

enum Situacao { APROVADO, RECUPERACAO, REPROVADO };

static void mostra_endereco_array(const int array[], size_t tamanho) {
    for (size_t i = 0; i < tamanho; i++)
        printf("array[%zu] = %d | endereco = %p | tamanho = %zu\n",
               i, array[i], (void *)&array[i], sizeof(array[i]));
}

int main(void) {
    /* Array: elementos do mesmo tipo, em posições contíguas. */
    int array[3] = {10, 20, 30};
    int matriz[2][3] = {{1, 2, 3}, {4, 5, 6}};

    printf("=== Array e matriz ===\n");
    printf("array: endereco inicial = %p, tamanho total = %zu\n",
           (void *)array, sizeof(array));
    mostra_endereco_array(array, 3);
    printf("matriz: endereco inicial = %p, tamanho total = %zu\n",
           (void *)matriz, sizeof(matriz));
    printf("matriz[0][0] = %d | endereco = %p\n", matriz[0][0], (void *)&matriz[0][0]);
    printf("matriz[1][2] = %d | endereco = %p\n", matriz[1][2], (void *)&matriz[1][2]);

    /*
     * String literal usa aspas duplas. O compilador cria um array de char
     * e acrescenta automaticamente '\0' ao final. Por isso sizeof(texto)
     * inclui um byte a mais que strlen(texto).
     */
    char texto[] = "C e proximo do hardware";
    printf("\n=== String ===\n");
    printf("texto = \"%s\"\n", texto);
    printf("strlen(texto) = %zu caractere(s)\n", strlen(texto));
    printf("sizeof(texto) = %zu byte(s), incluindo o terminador \\0\n", sizeof(texto));
    printf("endereco de texto = %p | endereco de texto[0] = %p\n",
           (void *)texto, (void *)&texto[0]);

    /*
     * Struct agrupa campos de tipos diferentes; pode conter padding.
     * O sufixo f em 9.5f indica float; sem ele, o literal seria double.
     */
    Aluno aluno = {7, "Ana", 9.5f};
    printf("\n=== Struct ===\n");
    printf("Aluno: codigo = %d, nome = %s, nota = %.1f\n",
           aluno.codigo, aluno.nome, aluno.nota);
    printf("struct: endereco = %p | tamanho = %zu\n", (void *)&aluno, sizeof(aluno));
    printf("codigo: endereco = %p | tamanho = %zu\n", (void *)&aluno.codigo, sizeof(aluno.codigo));
    printf("nome: endereco = %p | tamanho = %zu\n", (void *)aluno.nome, sizeof(aluno.nome));
    printf("nota: endereco = %p | tamanho = %zu\n", (void *)&aluno.nota, sizeof(aluno.nota));

    /* Union: todos os membros ocupam o mesmo espaço e compartilham endereço. */
    /* A union não precisa de sufixo: cada membro define seu próprio tipo. */
    Valor valor;
    printf("\n=== Union ===\n");
    printf("union: endereco = %p | tamanho = %zu\n", (void *)&valor, sizeof(valor));
    valor.inteiro = 42;
    printf("inteiro = %d | endereco = %p\n", valor.inteiro, (void *)&valor.inteiro);
    valor.real = 2.5f; /* Sobrescreve a representação anterior. */
    printf("real = %.2f | endereco = %p\n", valor.real, (void *)&valor.real);
    printf("Os membros da union compartilham o mesmo espaco de memoria.\n");

    /* Enum associa nomes legíveis a constantes inteiras. */
    /* Os identificadores do enum são constantes inteiras nomeadas. */
    enum Situacao situacao = APROVADO;
    printf("\n=== Enum ===\n");
    printf("APROVADO = %d | situacao atual = %d | tamanho = %zu\n",
           APROVADO, situacao, sizeof(situacao));
    printf("Enums tornam estados do programa mais legiveis.\n");
    return 0;
}
