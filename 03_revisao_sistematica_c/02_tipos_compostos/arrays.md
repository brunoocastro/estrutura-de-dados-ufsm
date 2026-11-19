# Arrays em C

## O que é um array?

Um array (*array*) é uma sequência de elementos do **mesmo tipo**, armazenados
em posições consecutivas de memória. O tamanho é definido quando o array é
criado e, em um array comum, não muda durante sua existência.

```c
int valores[5];
```

Essa declaração reserva espaço para cinco valores `int`, com índices de `0` a
`4`:

```text
valores[0]  valores[1]  valores[2]  valores[3]  valores[4]
     |           |           |           |           |
   base      base+1      base+2      base+3      base+4
```

O primeiro índice é sempre `0`. Se `n` é a quantidade de elementos, o último
índice válido é `n - 1`. Acessar `valores[5]` nesse exemplo ultrapassa o array
e causa **comportamento indefinido**.

## Tamanho explícito e implícito

### Tamanho explícito

O tamanho aparece entre colchetes:

```c
int a[5] = {10, 20, 30, 40, 50};
```

O array sempre terá cinco elementos. Se houver menos inicializadores, os
restantes serão inicializados com zero:

```c
int b[5] = {10, 20};
/* b contém: 10, 20, 0, 0, 0 */
```

### Tamanho implícito

Quando há inicializadores, o compilador pode contar os elementos:

```c
int c[] = {10, 20, 30};
/* O compilador determina que c possui 3 elementos. */
```

O tamanho implícito só pode ser usado quando o compilador consegue deduzi-lo
na declaração. Não é possível declarar um array local sem tamanho e preenchê-lo
posteriormente sem usar alocação dinâmica.

### Inicialização com zero

```c
int zeros[5] = {0};
```

O primeiro elemento recebe zero e todos os demais também recebem zero por causa
da regra de inicialização parcial.

## Tamanho ocupado na memória

O operador `sizeof` retorna o tamanho em bytes:

```c
int valores[5];

sizeof valores       /* tamanho total: 5 * sizeof(int) */
sizeof valores[0]    /* tamanho de um elemento */
```

A quantidade de elementos pode ser calculada enquanto `valores` ainda é um
array:

```c
size_t quantidade = sizeof valores / sizeof valores[0];
```

Exemplo: se `sizeof(int)` for `4`, então:

```text
sizeof valores[0] = 4 bytes
sizeof valores    = 5 * 4 = 20 bytes
```

O tamanho de `int` varia conforme a implementação. Nunca assuma que todo `int`
possui exatamente quatro bytes; use `sizeof` para verificar.

## Memória contígua e endereços

Os elementos ficam lado a lado. Se o primeiro elemento começa no endereço `B`,
o endereço conceitual de `array[i]` é:

```text
B + i * sizeof(array[0])
```

Em C, a expressão `array[i]` é equivalente a:

```c
*(array + i)
```

O nome do array normalmente converte-se em um ponteiro para o primeiro elemento
quando usado em uma expressão. Por isso estas expressões representam o mesmo
endereço:

```c
&array[0]
array
```

Para imprimir um endereço com `printf`, use `%p` e converta para `void *`:

```c
printf("%p\n", (void *)&array[0]);
```

Para uma demonstração numérica, pode-se usar `uintptr_t` de `<stdint.h>` e
`PRIuPTR` de `<inttypes.h>`, mas `%p` é a forma padrão para exibir ponteiros.

## Arrays e funções

Ao passar um array para uma função, ele normalmente sofre conversão para um
ponteiro para seu primeiro elemento. A função não recebe automaticamente o
tamanho total:

```c
void imprime(const int array[], size_t n) {
    for (size_t i = 0; i < n; i++)
        printf("%d\n", array[i]);
}
```

A chamada fornece o tamanho separadamente:

```c
int valores[] = {1, 2, 3};
imprime(valores, sizeof valores / sizeof valores[0]);
```

Dentro de `imprime`, `sizeof array` não calcula o tamanho do array original;
`array` é tratado como ponteiro. O tamanho deve ser passado como parâmetro.

Use `const` quando a função não deve alterar os elementos:

```c
void mostra(const int array[], size_t n);
```

## Arrays multidimensionais

Uma matriz é um array cujos elementos também são arrays:

```c
int matriz[2][3] = {
    {1, 2, 3},
    {4, 5, 6}
};
```

Ela possui duas linhas e três colunas, totalizando seis `int`. Os elementos são
armazenados em ordem de linha (*row-major*):

```text
1  2  3  4  5  6
```

O tamanho total é:

```c
sizeof matriz == 2 * 3 * sizeof(int)
```

## Limitações e cuidados

- O tamanho de um array comum é fixo depois da declaração.
- Índices fora do intervalo não são verificados automaticamente.
- Não confunda quantidade de elementos com tamanho em bytes.
- Não use `sizeof` para calcular o tamanho depois que o array foi passado a uma função.
- Arrays locais normalmente ficam na área de memória automática; arrays globais e
  `static` possuem duração diferente.
- Para tamanho definido em tempo de execução, use alocação dinâmica com `malloc`
  (veja [`../05_alocacao_dinamica/`](../05_alocacao_dinamica/)). Arrays de comprimento
  variável (VLA) são opcionais em C11 e arriscados: um tamanho grande estoura a pilha.

## Custo das operações (Big O)

| Operação | Custo | Por quê |
| --- | --- | --- |
| ler/escrever `v[i]` | O(1) | endereço = base + i·sizeof(elemento): uma conta só |
| procurar um valor (vetor não ordenado) | O(n) | no pior caso compara com todos |
| procurar em vetor ordenado (busca binária) | O(log n) | descarta metade a cada passo |
| inserir/remover na posição i | O(n − i) | desloca os elementos seguintes |
| inserir/remover no fim | O(1) | basta ajustar o contador de ocupados |
| percorrer, somar, achar o máximo | O(n) | visita cada elemento uma vez |
| percorrer matriz L×C | O(L·C) | visita cada célula; prefira linha por linha (cache) |

## Exemplos executáveis

- [`01_arrays.c`](01_arrays.c): inicialização, endereços, conta `base + i*tamanho`, busca
  sequencial contando comparações e decaimento para ponteiro.
- [`02_vetor_insercao_remocao.c`](02_vetor_insercao_remocao.c): deslocamentos passo a passo.
- [`03_matrizes.c`](03_matrizes.c): ordem de linha (row-major) e deslocamento `l*C + c`.

Na raiz do repositório:

```sh
./run 01_arrays
./run 02_vetor_insercao_remocao
./run 03_matrizes
```
