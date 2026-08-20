# Tipos compostos de C

Esta pasta mostra como agrupar dados em C, com foco na representação em memória.
Cada arquivo é um programa independente e possui comentários explicativos.

## Arquivos

| Arquivo | Conteúdo |
| --- | --- |
| `00_visao_geral.c` | demonstração geral de arrays, matrizes, strings, structs, unions e enums |
| `01_vetores.c` | arrays unidimensionais, tamanho explícito e implícito, inicialização e memória contígua |
| `02_matrizes.c` | arrays multidimensionais, dimensões, índices e ordem de linhas |
| `03_strings.c` | arrays de `char`, `\\0`, `strlen`, `sizeof` e literais de string |
| `03_strings_maiores.c` | `fgets`, buffers maiores, cópia/concatenação seguras, busca e capacidade |
| `04_structs.c` | formas de declaração, campos heterogêneos, `typedef`, padding, alinhamento e `offsetof` |
| `05_unions.c` | formas de declaração, membros compartilhando a mesma região de memória e sobrescrita |
| `06_enums.c` | formas de declaração, tags, `typedef` e valores automáticos |
| `07_structs_aninhadas.c` | structs compostas, structs dentro de structs, acesso e memória |

## Como usar o GCC

`gcc` é o compilador da coleção GNU Compiler Collection. A estrutura básica
para transformar um arquivo C em um executável é:

```bash
gcc [opcoes] arquivo.c -o nome_do_executavel
```

Neste material usamos:

- `-std=c11`: seleciona o padrão C11 da linguagem, evitando que o resultado
  dependa de extensões específicas do compilador.
- `-Wall`: ativa a maioria dos avisos importantes do compilador.
- `-Wextra`: ativa avisos adicionais que ajudam a encontrar erros e código
  suspeito.
- `-pedantic`: exige maior conformidade com o padrão C11 e avisa sobre
  construções não portáveis.
- `-o nome`: define o nome do arquivo executável gerado. Sem `-o`, o GCC
  normalmente cria um arquivo chamado `a.out`.

Os avisos não impedem necessariamente a compilação, mas devem ser analisados.
Usar `-Wall -Wextra -pedantic` desde o início ajuda a encontrar problemas antes
de executar o programa.

Exemplo completo:

```bash
gcc -std=c11 -Wall -Wextra -pedantic 01_vetores.c -o 01_vetores
./01_vetores
```

O primeiro comando compila; o segundo executa. O prefixo `./` informa ao shell
que o executável está no diretório atual.

## Compilar e executar um exemplo

Na raiz do repositório, copie e cole o bloco desejado:

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
gcc -std=c11 -Wall -Wextra -pedantic 01_vetores.c -o 01_vetores
./01_vetores
```

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
gcc -std=c11 -Wall -Wextra -pedantic 02_matrizes.c -o 02_matrizes
./02_matrizes
```

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
gcc -std=c11 -Wall -Wextra -pedantic 03_strings.c -o 03_strings
./03_strings
```

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
gcc -std=c11 -Wall -Wextra -pedantic 03_strings_maiores.c -o 03_strings_maiores
./03_strings_maiores
```

O exemplo solicita nome, cidade e uma palavra. Ele demonstra entrada com espaços,
remoção da quebra de linha, montagem de uma frase, busca de substrings e limites
de capacidade dos buffers.

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
gcc -std=c11 -Wall -Wextra -pedantic 04_structs.c -o 04_structs
./04_structs
```

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
gcc -std=c11 -Wall -Wextra -pedantic 05_unions.c -o 05_unions
./05_unions
```

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
gcc -std=c11 -Wall -Wextra -pedantic 06_enums.c -o 06_enums
./06_enums
```

## Compilar todos

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
for arquivo in *.c; do
    executavel="${arquivo%.c}"
    gcc -std=c11 -Wall -Wextra -pedantic "$arquivo" -o "$executavel" || exit 1
done
```

O comando também compila `00_visao_geral.c`. Para executar tudo:

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
for executavel in 00_visao_geral 01_vetores 02_matrizes 03_strings 03_strings_maiores 04_structs 05_unions 06_enums 07_structs_aninhadas; do
    echo "=== $executavel ==="
    ./$executavel
done
```

## Pontos importantes

### Vetores

Um vetor é um conjunto de elementos do mesmo tipo armazenados contiguamente.
Em `int v[5]`, o tamanho é explícito: existem exatamente cinco elementos.
Em `int v[] = {1, 2, 3}`, o compilador infere o tamanho como três.

A capacidade total pode ser calculada enquanto o objeto ainda é um array:

```c
size_t n = sizeof v / sizeof v[0];
```

O primeiro índice é `0` e o último é `n - 1`. Acessar `v[n]` ultrapassa o vetor
e produz comportamento indefinido. Arrays locais têm tamanho fixo; para tamanhos
variáveis, estudaremos alocação dinâmica com ponteiros e `malloc`.

### Matrizes

Uma matriz como `int m[2][3]` é um array de duas linhas, cada uma contendo três
`int`. Os elementos são armazenados em ordem de linha (*row-major*). O tamanho
total é `2 * 3 * sizeof(int)`, sem contar qualquer ponteiro adicional: a matriz
é realmente uma região contígua de elementos.

### Strings

C não possui um tipo primitivo string. Uma string é um array de `char` terminado
por `\\0`. `strlen` conta até o terminador; `sizeof` conta a capacidade total do
array. Um literal como `"C"` ocupa dois `char`: `'C'` e `\\0`.

### Structs

Uma `struct` agrupa campos heterogêneos. Os campos aparecem na ordem declarada,
mas o compilador pode inserir *padding* para alinhamento. Portanto, a soma dos
`sizeof` dos campos pode ser menor que o `sizeof` da struct. `offsetof` mostra o
offset de um campo em relação ao início do objeto.

### Unions

Uma `union` reserva espaço suficiente para seu maior membro. Todos os membros
começam no mesmo endereço. Escrever um membro sobrescreve os bytes usados pelos
outros; normalmente apenas o último membro escrito deve ser lido.

### Formas de declarar `struct`, `union` e `enum`

Os três tipos podem receber uma **tag**, um nome criado por `typedef`, ou ambos.
Uma tag precisa ser escrita com a palavra-chave na declaração:

```c
struct Sensor { int id; };
struct Sensor sensor;

union Valor { int inteiro; float real; };
union Valor valor;

enum Estado { DESLIGADO, LIGADO };
enum Estado estado;
```

Também é possível criar um tipo anônimo diretamente com `typedef`:

```c
typedef struct { int x; int y; } Ponto;
typedef union { int inteiro; float real; } Dado;
typedef enum { PEQUENO, GRANDE } Tamanho;

Ponto ponto;
Dado dado;
Tamanho tamanho;
```

A combinação de tag e `typedef` oferece as duas formas de uso:

```c
typedef struct Ponto { int x; int y; } Ponto;
Ponto a;
struct Ponto b;
```

Uma declaração anônima sem `typedef` pode declarar uma variável, mas o tipo não
pode ser reutilizado:

```c
struct { int codigo; } item;
union { int codigo; float valor; } dado;
```

As diferenças de memória são fundamentais:

- **`struct`:** todos os campos existem simultaneamente, em posições próprias;
  pode haver *padding* entre eles.
- **`union`:** todos os membros começam no mesmo endereço e compartilham espaço;
  escrever um membro sobrescreve os bytes dos demais. Use quando as alternativas
  não precisam existir ao mesmo tempo.
- **`enum`:** define constantes inteiras nomeadas; não armazena textos nem cria
  uma lista automaticamente. Os valores começam em zero por padrão, aumentam de
  um em um e podem ser definidos explicitamente.

Os arquivos `04_structs.c`, `05_unions.c` e `06_enums.c` demonstram cada forma
com comentários diretamente no código.

### Structs aninhadas

Uma struct pode conter outra struct como campo. No exemplo `07_structs_aninhadas.c`,
`Pessoa` contém um `Endereco`, que contém uma `Cidade`:

```c
pessoa.endereco.cidade.nome
```

Quando usamos um ponteiro para a struct, a mesma navegação usa `->` no primeiro
nível e `.` nos campos internos:

```c
pessoa->endereco.cidade.nome
```

A estrutura externa contém fisicamente as estruturas internas. `sizeof(Pessoa)`
considera todos os campos internos e possíveis bytes de padding. Ao copiar uma
struct com `Pessoa copia = pessoa`, os arrays internos também são copiados; isso
é diferente de uma struct que contenha ponteiros, pois nesse caso somente os
endereços seriam copiados.

### Enums

Um `enum` define constantes inteiras nomeadas. Se o primeiro valor não for
informado, começa em zero e os seguintes são incrementados. Atribuir valores
explícitos altera essa sequência. Usar nomes como `LIGADO` é mais legível que
espalhar números mágicos pelo programa.
