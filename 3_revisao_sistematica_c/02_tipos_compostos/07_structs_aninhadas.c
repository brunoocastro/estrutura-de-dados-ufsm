/*
 * STRUCTS ANINHADAS (COMPOSTAS)
 *
 * Uma struct pode conter outra struct como campo. Isso permite modelar dados
 * relacionados em níveis: por exemplo, uma Pessoa possui um Endereco, que
 * possui uma Cidade. Os campos internos continuam armazenados dentro do objeto
 * externo, respeitando alinhamento e podendo conter padding.
 *
 * Compile e execute nesta pasta:
 *   gcc -std=c11 -Wall -Wextra -pedantic 07_structs_aninhadas.c -o 07_structs_aninhadas
 *   ./07_structs_aninhadas
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char nome[40];
    char estado[3];
} Cidade;

typedef struct {
    char rua[60];
    int numero;
    Cidade cidade;
} Endereco;

typedef struct {
    int id;
    char nome[40];
    Endereco endereco;
} Pessoa;

void imprime_pessoa(const Pessoa *pessoa) {
    /* O operador -> acessa um campo por meio de um ponteiro para struct. */
    printf("Pessoa #%d: %s\n", pessoa->id, pessoa->nome);
    printf("Endereco: %s, %d — %s/%s\n",
           pessoa->endereco.rua,
           pessoa->endereco.numero,
           pessoa->endereco.cidade.nome,
           pessoa->endereco.cidade.estado);
}

int main(void) {
    /* Inicialização aninhada: cada conjunto de chaves corresponde a uma struct. */
    Pessoa pessoa = {
        .id = 1,
        .nome = "Ana Silva",
        .endereco = {
            .rua = "Rua das Flores",
            .numero = 123,
            .cidade = {
                .nome = "Santa Maria",
                .estado = "RS"
            }
        }
    };

    Pessoa equipe[2] = {
        {
            .id = 2,
            .nome = "Bruno Lima",
            .endereco = {"Avenida Central", 45, {"Porto Alegre", "RS"}}
        },
        {
            .id = 3,
            .nome = "Carla Souza",
            .endereco = {"Rua do Sol", 88, {"Curitiba", "PR"}}
        }
    };

    printf("=== Structs aninhadas ===\n");
    imprime_pessoa(&pessoa);
    printf("\n=== Acesso aos campos internos ===\n");
    printf("pessoa.id = %d\n", pessoa.id);
    printf("pessoa.endereco.numero = %d\n", pessoa.endereco.numero);
    printf("pessoa.endereco.cidade.nome = %s\n", pessoa.endereco.cidade.nome);
    printf("pessoa.endereco.cidade.estado = %s\n", pessoa.endereco.cidade.estado);

    printf("\n=== Memoria dos niveis da estrutura ===\n");
    printf("Pessoa: endereco = %p | tamanho = %zu\n",
           (void *)&pessoa, sizeof pessoa);
    printf("Pessoa.endereco: endereco = %p | tamanho = %zu\n",
           (void *)&pessoa.endereco, sizeof pessoa.endereco);
    printf("Pessoa.endereco.cidade: endereco = %p | tamanho = %zu\n",
           (void *)&pessoa.endereco.cidade, sizeof pessoa.endereco.cidade);
    printf("Pessoa.endereco.cidade.nome: endereco = %p | tamanho = %zu\n",
           (void *)pessoa.endereco.cidade.nome, sizeof pessoa.endereco.cidade.nome);
    printf("offset de endereco em Pessoa = %zu\n",
           offsetof(Pessoa, endereco));
    printf("offset de cidade em Endereco = %zu\n",
           offsetof(Endereco, cidade));

    printf("\n=== Array de structs compostas ===\n");
    for (size_t i = 0; i < 2; i++) {
        printf("equipe[%zu] endereco = %p | nome = %s | cidade = %s\n",
               i, (void *)&equipe[i], equipe[i].nome,
               equipe[i].endereco.cidade.nome);
    }

    /* Uma struct aninhada é copiada por valor com o operador de atribuição. */
    Pessoa copia = pessoa;
    strcpy(copia.nome, "Nome da copia");
    printf("\nCopia independente: %s (original: %s)\n", copia.nome, pessoa.nome);
    printf("Strings sao arrays dentro das structs; neste caso a copia recebeu seus proprios arrays.\n");
    return 0;
}
