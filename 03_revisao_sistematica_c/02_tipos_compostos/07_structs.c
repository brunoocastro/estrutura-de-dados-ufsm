/*
 * ============================================================================
 * Arquivo : 07_structs.c
 * Módulo  : 03 — Revisão sistemática de C     (Ementa: 1.1 Estruturas)
 * ----------------------------------------------------------------------------
 * OBJETIVO
 *   As quatro formas de declarar uma struct (tag, typedef anônimo, tag +
 *   typedef, variável anônima), acesso com . e ->, array de structs e o
 *   padding que o compilador insere para alinhar os campos (offsetof).
 *
 * EXEMPLO REAL
 *   Um registro de sensor {id, unidade, leitura} enviado por rede industrial:
 *   o padding muda o tamanho do pacote, e a ordem dos campos pode economizar
 *   bytes em microcontroladores com pouca RAM.
 *
 * COMPLEXIDADE
 *   Acesso a campo (s.campo, p->campo): O(1), deslocamento fixo (offsetof).
 *   Copiar uma struct (a = b): O(sizeof(struct)), copia todos os bytes.
 *   Percorrer um array de n structs: O(n).
 *   Dica: passe structs grandes por ponteiro (const T *) para evitar a cópia.
 *
 * COMO EXECUTAR
 *   ./run 07_structs
 * ============================================================================
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

    /* Variáveis usando o nome criado por typedef. Os inicializadores
     * designados (.campo = valor) deixam explícito qual campo recebe o quê e
     * continuam corretos se a ordem dos campos mudar. */
    Medicao medicao = {.id = 2, .temperatura = 18.75f};
    Ponto origem = {0, 0};
    struct Ponto destino = {.x = 10, .y = 20};

    /* Struct anônima: sem nome de tipo, só dá para atribuir campo a campo. */
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

    /* Mesmos campos, ordens diferentes: campos maiores primeiro costumam
     * reduzir o padding. Em um vetor de 10 000 registros isso faz diferença. */
    struct Desordenada { char a; double b; char c; };
    struct Ordenada { double b; char a; char c; };
    printf("\n=== A ordem dos campos importa ===\n");
    printf("struct { char; double; char; } -> %zu bytes\n", sizeof(struct Desordenada));
    printf("struct { double; char; char; } -> %zu bytes\n", sizeof(struct Ordenada));
    printf("Em 10000 registros: %zu bytes contra %zu bytes.\n",
           10000 * sizeof(struct Desordenada), 10000 * sizeof(struct Ordenada));

    printf("\n=== Array de structs ===\n");
    for (size_t i = 0; i < 2; i++)
        printf("sensores[%zu] = {%d, %c, %.1f} | endereco = %p\n",
               i, sensores[i].id, sensores[i].unidade, sensores[i].leitura,
               (void *)&sensores[i]);

    printf("\nOperador . acessa uma variavel; operador -> acessa campos via ponteiro.\n");
    return 0;
}
