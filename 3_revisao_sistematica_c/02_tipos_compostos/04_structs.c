/*
 * STRUCTS EM C — formas de declaração, uso e memória
 *
 * Uma struct agrupa campos heterogêneos. Diferentemente de uma union, todos
 * os campos existem ao mesmo tempo e ocupam regiões distintas da estrutura.
 * O compilador pode inserir padding entre campos para respeitar alinhamento.
 *
 * Compile e execute nesta pasta:
 *   gcc -std=c11 -Wall -Wextra -pedantic 04_structs.c -o 04_structs
 *   ./04_structs
 */
#include <stddef.h>
#include <stdio.h>

/* FORMA 1 — struct com tag (nome da estrutura).
 * A tag Sensor não é um typedef: ao declarar uma variável, escrevemos
 * explicitamente "struct Sensor". A declaração pode ser reutilizada. */
struct Sensor {
    int id;
    char unidade;
    double leitura;
};

/* FORMA 2 — struct anônima com typedef.
 * Não existe uma tag para escrever depois; Medicao passa a ser o nome do tipo.
 * Esta forma é conveniente quando a tag não será necessária. */
typedef struct {
    int id;
    float temperatura;
} Medicao;

/* FORMA 3 — struct com tag e typedef ao mesmo tempo.
 * Podemos usar tanto "struct Ponto" quanto "Ponto". */
typedef struct Ponto {
    int x;
    int y;
} Ponto;

/* FORMA 4 — declaração de variável de struct anônima.
 * O tipo só pode ser usado nessa declaração; não há nome reutilizável. */
struct {
    char simbolo;
    int codigo;
} item_anonimo;

void mostra_sensor(const struct Sensor *sensor) {
    printf("sensor {%d, %c, %.1f}\n", sensor->id, sensor->unidade, sensor->leitura);
}

int main(void) {
    /* Variável usando uma struct com tag. */
    struct Sensor sensor = {1, 'C', 23.5};

    /* Variáveis usando o nome criado por typedef. */
    Medicao medicao = {.id = 2, .temperatura = 18.75f};
    Ponto origem = {0, 0};
    struct Ponto destino = {.x = 10, .y = 20};

    /* Inicialização designada explicita o campo que receberá cada valor. */
    item_anonimo.simbolo = '*';
    item_anonimo.codigo = 7;

    /* Array de structs: cada elemento possui todos os campos. */
    struct Sensor sensores[2] = {{3, 'C', 20.0}, {4, 'F', 68.0}};

    printf("=== Formas de declarar structs ===\n");
    mostra_sensor(&sensor);
    printf("Medicao via typedef: id = %d, temperatura = %.2f\n",
           medicao.id, medicao.temperatura);
    printf("Ponto via typedef: origem = (%d, %d)\n", origem.x, origem.y);
    printf("Ponto via tag: destino = (%d, %d)\n", destino.x, destino.y);
    printf("Struct anonima: {%c, %d}\n", item_anonimo.simbolo, item_anonimo.codigo);

    printf("\n=== Memoria e padding ===\n");
    printf("sensor: endereco = %p | tamanho = %zu\n", (void *)&sensor, sizeof sensor);
    printf("sensor.id: offset = %zu | endereco = %p | tamanho = %zu\n",
           offsetof(struct Sensor, id), (void *)&sensor.id, sizeof sensor.id);
    printf("sensor.unidade: offset = %zu | endereco = %p | tamanho = %zu\n",
           offsetof(struct Sensor, unidade), (void *)&sensor.unidade, sizeof sensor.unidade);
    printf("sensor.leitura: offset = %zu | endereco = %p | tamanho = %zu\n",
           offsetof(struct Sensor, leitura), (void *)&sensor.leitura, sizeof sensor.leitura);
    printf("sizeof struct Sensor = %zu; soma dos campos = %zu\n",
           sizeof(struct Sensor), sizeof(int) + sizeof(char) + sizeof(double));
    printf("A diferenca, quando existe, e padding de alinhamento.\n");

    printf("\n=== Array de structs ===\n");
    for (size_t i = 0; i < 2; i++)
        printf("sensores[%zu] = {%d, %c, %.1f} | endereco = %p\n",
               i, sensores[i].id, sensores[i].unidade, sensores[i].leitura,
               (void *)&sensores[i]);

    printf("\nOperador . acessa uma variavel; operador -> acessa campos via ponteiro.\n");
    return 0;
}
