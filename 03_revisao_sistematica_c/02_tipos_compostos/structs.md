# Structs em C

## O que é uma `struct`?

Uma `struct` agrupa campos que podem ter tipos diferentes. Ela é útil para
representar uma entidade composta, como uma pessoa, sensor ou produto.

```c
struct Sensor {
    int id;
    char unidade;
    double leitura;
};
```

Uma variável possui todos os campos simultaneamente:

```c
struct Sensor sensor = {1, 'C', 23.5};
```

O acesso é feito com o operador ponto:

```c
sensor.id
sensor.leitura
```

Se tivermos um ponteiro para a estrutura, usamos `->`:

```c
struct Sensor *ponteiro = &sensor;
printf("%d", ponteiro->id);
```

A expressão `ponteiro->id` equivale a `(*ponteiro).id`.

## Formas de declaração

### Struct com tag

A tag é o nome usado junto com a palavra-chave `struct`:

```c
struct Ponto {
    int x;
    int y;
};

struct Ponto ponto;
```

A tag `Ponto` sozinha não é, necessariamente, um nome de tipo em C. Para
declarar uma variável, escrevemos `struct Ponto`.

### Struct anônima com `typedef`

```c
typedef struct {
    int x;
    int y;
} Ponto;

Ponto ponto;
```

A estrutura não possui tag, mas `Ponto` passa a ser um alias para o tipo.

### Tag e `typedef` juntos

```c
typedef struct Ponto {
    int x;
    int y;
} Ponto;

Ponto a;
struct Ponto b;
```

Essa forma permite usar tanto `Ponto` quanto `struct Ponto`. É útil quando a
estrutura precisa referenciar a si mesma ou quando queremos oferecer as duas
formas de escrita.

### Struct anônima em uma declaração

```c
struct {
    int codigo;
    float valor;
} item;
```

A variável `item` existe, mas não há um nome reutilizável para o tipo dessa
estrutura.

## Memória dos campos

Uma struct é armazenada como uma região de memória que contém seus campos na
ordem declarada, embora possam existir bytes de preenchimento (*padding*) entre
eles ou no final.

Considere:

```c
struct Exemplo {
    char letra;
    int numero;
    double valor;
};
```

O layout conceitual pode ser:

```text
[ letra ][ padding ][ numero ][ padding ][ valor ]
```

O `padding` existe para que cada campo respeite os requisitos de alinhamento da
arquitetura. Por isso, a soma de `sizeof(char) + sizeof(int) + sizeof(double)`
pode ser menor que `sizeof(struct Exemplo)`.

Nunca calcule manualmente o tamanho de uma struct apenas somando os campos.
Use:

```c
sizeof(struct Exemplo)
```

Para verificar o deslocamento de um campo em relação ao início da estrutura,
use `offsetof`:

```c
#include <stddef.h>
printf("%zu", offsetof(struct Exemplo, numero));
```

Os endereços são observáveis com:

```c
printf("%p", (void *)&objeto);
printf("%p", (void *)&objeto.campo);
```

## Inicialização

### Inicialização posicional

Os valores seguem a ordem dos campos:

```c
struct Ponto ponto = {10, 20};
```

### Inicialização designada

Os nomes dos campos tornam a intenção explícita e permitem mudar a ordem:

```c
struct Ponto ponto = {
    .y = 20,
    .x = 10
};
```

Campos não informados são inicializados com zero quando a estrutura é
inicializada por uma lista.

## Structs aninhadas

Uma struct pode conter outra struct:

```c
typedef struct {
    char rua[60];
    int numero;
} Endereco;

typedef struct {
    char nome[40];
    Endereco endereco;
} Pessoa;
```

O acesso encadeia os operadores:

```c
pessoa.endereco.numero
```

Com ponteiro:

```c
pessoa_ptr->endereco.numero
```

A estrutura externa contém fisicamente a estrutura interna. O `sizeof(Pessoa)
considera todos os campos internos e o possível padding.

## Arrays de structs

É possível armazenar vários registros em um array:

```c
struct Ponto pontos[3] = {
    {0, 0},
    {10, 20},
    {30, 40}
};
```

Os elementos do array ficam contíguos, e cada elemento ocupa `sizeof(struct Ponto)`
bytes. Assim, `&pontos[i + 1]` avança o tamanho completo de uma struct, incluindo
padding.

## Cópia e atribuição

Structs podem ser copiadas diretamente:

```c
struct Ponto origem = {1, 2};
struct Ponto copia = origem;
```

Em uma struct contendo arrays, os elementos dos arrays são copiados. Em uma
struct contendo ponteiros, os endereços são copiados, não os objetos apontados:

```c
struct ComPonteiro {
    int *valor;
};
```

Nesse segundo caso, duas structs podem apontar para o mesmo `int`. Isso é uma
cópia superficial (*shallow copy*), e deve ser considerada ao gerenciar memória.

## Structs e funções

Por valor, a função recebe uma cópia da estrutura:

```c
void mostra(struct Ponto ponto);
```

Para evitar a cópia e permitir alteração do objeto original, use ponteiro:

```c
void move(struct Ponto *ponto) {
    ponto->x++;
    ponto->y++;
}
```

Quando a função apenas consulta a estrutura, combine ponteiro e `const`:

```c
void mostra(const struct Ponto *ponto);
```

## Diferença entre `struct` e `union`

- Em uma `struct`, todos os campos existem ao mesmo tempo e possuem regiões
  próprias de memória.
- Em uma `union`, os campos compartilham o mesmo espaço e escrever um membro
  sobrescreve os demais.
- O tamanho de uma `struct` é influenciado por todos os campos e padding.
- O tamanho de uma `union` é suficiente para acomodar seu maior membro, com o
  alinhamento necessário.

## Custo das operações

- Acessar um campo (`s.campo`, `p->campo`): O(1), pois o deslocamento é fixo e calculado na
  compilação (`offsetof`).
- Copiar uma struct (`a = b`) ou passá-la por valor: O(sizeof(struct)). Para structs grandes,
  passe `const T *` (O(1)).
- Percorrer um vetor de n structs: O(n).

## Exemplos executáveis

Os arquivos [`07_structs.c`](07_structs.c) e
[`10_structs_aninhadas.c`](10_structs_aninhadas.c) mostram tags, `typedef`,
structs anônimas, padding, offsets, a influência da ordem dos campos, arrays de
structs e estruturas aninhadas. Na raiz do repositório:

```sh
./run 07_structs
./run 10_structs_aninhadas
```
