# Listas encadeadas em C

Uma lista encadeada armazena uma sequência em nós. Cada nó guarda um valor e um ponteiro para o próximo nó; a lista guarda o endereço do primeiro. Diferente de um array, os nós não precisam ocupar posições contíguas de memória e a lista não precisa reservar uma capacidade fixa antecipadamente.

## Representação

O exemplo [`01_lista_encadeada.c`](01_lista_encadeada.c) implementa uma lista simplesmente encadeada de inteiros:

```c
typedef struct No {
    int valor;
    struct No *proximo;
} No;

typedef struct {
    No *inicio;
    size_t tamanho;
} Lista;
```

`inicio` aponta para o primeiro nó. O campo `proximo` liga um nó ao seguinte. No último nó, `proximo` vale `NULL`; em uma lista vazia, `inicio` vale `NULL` e `tamanho` vale zero.

```text
inicio
  |
  v
[5 | •] -> [10 | •] -> [20 | NULL]
```

Os nós são criados individualmente com `malloc`. Como eles podem estar em endereços diferentes, para chegar ao elemento de índice `i` é necessário seguir os ponteiros desde o início. Ao remover um nó, o programa deve religar os vizinhos e chamar `free` para devolver a memória.

## Operações e custo

O exemplo oferece inserção e remoção por índice, busca, impressão e liberação da lista. Os índices começam em zero. Inserir aceita índices de `0` a `tamanho` inclusive; remover aceita índices de `0` a `tamanho - 1`. Índices inválidos e falhas de alocação são informados sem alterar a lista.

| Operação | Custo | Observação |
| --- | --- | --- |
| Consultar o elemento de índice `i` | `O(n)` | É preciso percorrer os nós anteriores. |
| Buscar um valor | `O(n)` | No pior caso, visita todos os nós. |
| Inserir/remover no início | `O(1)` | Atualiza o ponteiro `inicio`. |
| Inserir/remover em índice arbitrário | `O(n)` | O percurso até a posição domina o custo. |
| Inserir no fim nesta implementação | `O(n)` | A lista não mantém um ponteiro para o último nó. |

Uma lista encadeada evita deslocar vários elementos ao inserir ou remover perto do início, como pode ocorrer em um array. Em contrapartida, não oferece acesso direto por índice, usa memória extra para os ponteiros e depende de alocações individuais. A escolha depende das operações mais frequentes; não existe uma estrutura melhor para todos os casos.

## Lista e fila são conceitos diferentes

Uma **lista** organiza elementos em sequência e permite operações em diferentes posições. Uma **fila** define uma regra de acesso FIFO (*First In, First Out*): insere-se no fim e remove-se do início. Uma fila pode ser implementada com array circular, como em [`5_filas/lista_04`](../5_filas/lista_04/README.md), ou com nós encadeados. A implementação encadeada da pilha em [`4_pilhas/02_pilha_dinamica.c`](../4_pilhas/02_pilha_dinamica.c) também usa nós, mas aplica a regra LIFO da pilha. O encadeamento é uma forma de organizar memória; fila e pilha são comportamentos definidos pelas operações permitidas.

Este material cobre a lista simplesmente encadeada. Em uma lista duplamente encadeada, cada nó também aponta para o anterior; em uma lista circular, o último nó volta a apontar para outro nó da lista em vez de terminar em `NULL`. Essas variantes mudam os percursos e as operações de fronteira.

## Compilar e executar

A partir da raiz do repositório:

```bash
gcc -std=c11 -Wall -Wextra -pedantic \
  6_listas/01_lista_encadeada.c \
  -o /tmp/01_lista_encadeada
/tmp/01_lista_encadeada
```

O programa demonstra inserção no início, no meio e no fim, busca, remoção, rejeição de índice inválido e liberação dos nós. A saída imprime os valores em ordem, conectados por setas, para tornar o encadeamento visível.

## Tarefa Avaliada 05

O arquivo [`02_t05_listas.c`](02_t05_listas.c) reúne a solução em C. Inclui a explicação passo a passo da remoção recursiva em uma lista de quatro nós; as funções recursivas `tamanho`, `vazia`, `conta_item` e `insere_item_final`; e pilha e fila dinâmicas com `push`/`pop` e inserção/remoção FIFO. O código corrige a propagação do retorno recursivo que falta na função `remove_item` do enunciado.

Compile e execute a partir da raiz do repositório:

```bash
gcc -std=c11 -Wall -Wextra -pedantic \
  6_listas/02_t05_listas.c \
  -o /tmp/02_t05_listas
/tmp/02_t05_listas
```

`vazia` percorre recursivamente os nós para cumprir o enunciado; em código de produção, o teste `inicio == NULL` seria mais eficiente e não consumiria a pilha de chamadas. As funções recursivas de lista usam espaço de chamadas `O(n)`.
