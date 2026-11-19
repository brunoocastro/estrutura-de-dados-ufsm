# Trabalho 2 (T02) — Funções e recursão

**DPEE1038 — Estrutura de Dados para Automação**

Tarefa **avaliada**. Este arquivo traz apenas os requisitos; as soluções não são publicadas.
O enunciado completo, com prazos e critérios de avaliação, está no Google Classroom da disciplina.
Material de apoio: [README do módulo](../README.md), [Lista 02](../lista_2/README.md) e
[recursão em `03_revisao_sistematica_c`](../../03_revisao_sistematica_c/04_recursao/).

## Problemas

Cada problema deve ser um programa C independente, com a função pedida e uma `main` que lê os
dados da entrada padrão, mostra uma mensagem antes de cada leitura e imprime o resultado.

### 1. Potência inteira recursiva

Escreva a função recursiva

```c
float potencia_int(float a, int b);
```

que calcula `a` elevado a `b`. A função deve funcionar também para expoentes negativos
(`a⁻ᵇ = 1 / aᵇ`).

Entrada: a base `a` e o expoente `b`.

### 2. Existência de um valor no vetor (recursiva)

Escreva a função recursiva

```c
int existe_vec(const float v[], int n, float x);
```

que devolve `1` se o valor `x` está entre os `n` elementos de `v` e `0` caso contrário.

Entrada: o tamanho do vetor, os elementos e o valor procurado.

### 3. Inserção em vetor ordenado

Escreva a função

```c
void insere_em_ordem(float v[], int n, float x);
```

que recebe um vetor `v` com `n` elementos em ordem crescente e **espaço para `n + 1`
elementos**, e insere `x` de modo que o vetor continue em ordem crescente com `n + 1` elementos.

Entrada: o tamanho inicial `n`, os `n` valores já ordenados e o valor `x`. Saída: o vetor
resultante.

### 4. Mediana

Escreva a função

```c
float mediana(const float v[], int n);
```

que devolve a mediana dos `n` elementos de `v` quando o vetor está em ordem crescente. Se o vetor
estiver vazio ou fora de ordem, a função deve devolver `NAN` (de `<math.h>`), e a `main` deve
informar o problema ao usuário.

Entrada: o tamanho do vetor e seus elementos.

## Requisitos gerais

- Compilar sem avisos com `cc -std=c11 -Wall -Wextra -pedantic` (se usar `<math.h>`, como o
  `NAN` do problema 4, acrescente `-lm`).
- Validar as entradas (tamanho negativo, leitura que falhou) antes de usar os valores.
- Recomendação: nas funções recursivas, indique em comentário o **caso base** e o **passo
  recursivo**.
- Para testar seus programas use o script da raiz: salve o arquivo em uma pasta sua e rode
  `./run caminho/do/arquivo.c`.
