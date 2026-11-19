# Tarefa 1 (T01) — Programação em C

Tarefa **avaliada**. Este arquivo traz apenas o enunciado; as soluções não são publicadas.
Material de apoio: [README do módulo](../README.md), [Lista 01](../lista_01/README.md) e os
exemplos `01_` a `05_` da pasta [`01_revisao_c_basico/`](../).

## Problemas

### 1. Números inteiros ímpares entre dois valores

Leia dois números inteiros `a` e `b` fornecidos pela usuária ou pelo usuário e mostre todos os números inteiros ímpares compreendidos entre esses valores.

### 2. Divisores de um número

A partir de um valor inteiro `x`, mostre todos os números inteiros pelos quais `x` é divisível.

### 3. Verificação de número primo

Leia um número inteiro `x` e verifique se ele é ou não um número primo. Considere primo o número divisível apenas por `1` e por ele mesmo.

### 4. Menor primo maior que um valor

Leia um número inteiro `g` e mostre o menor número primo que seja maior do que `g`.

### 5. Situação acadêmica a partir de notas

Leia duas notas bimestrais de uma aluna ou de um aluno, calcule a média aritmética simples e mostre sua situação:

- **Aprovada/aprovado**, caso a média seja maior ou igual a `7`;
- **Em exame**, caso a média seja menor que `7`.

Quando a situação for exame, leia também a nota obtida no exame, calcule a média aritmética simples entre a média anterior e a nota do exame e mostre:

- **Aprovada/aprovado**, caso a média final seja maior ou igual a `5`;
- **Reprovada/reprovado**, caso a média final seja menor que `5`.

### 6. Menor e maior valor de um vetor real

Leia um vetor com `10` valores reais e determine:

- o menor valor armazenado no vetor;
- o maior valor armazenado no vetor.

### 7. Validação e moda de um vetor inteiro

Leia um vetor com `20` valores inteiros e verifique se todos pertencem ao intervalo fechado `[0, 100]`. Se todos os valores estiverem nesse intervalo, determine o valor mais frequente do vetor, isto é, sua moda.

### 8. Dias decorridos desde uma data

Leia uma data fornecida pela usuária ou pelo usuário e calcule o número de dias decorridos desde essa data. O programa deve considerar corretamente a quantidade de dias de cada mês e os anos bissextos.

### 9. Conversão e validação de número binário

Considere um número binário `b` composto por `n` dígitos, em que `d1` é o dígito menos significativo. O valor decimal correspondente deve ser calculado pela expressão:

$$
v = \sum_{i=1}^{n} 2^{i-1}d_i
$$

O programa deve:

#### a) Exibir os dígitos

Ler um número inteiro decimal em uma variável chamada `x`, usando `scanf("%d", &x)`, e exibir seus `n` dígitos separadamente, completando com zeros à esquerda quando necessário. Para essa saída, deve ser utilizado um laço `for`.

Por exemplo, para `x = 110` e `n = 5`, exibir:

```text
d5=0 d4=0 d3=1 d2=1 d1=0
```

> Observação: no enunciado original o exemplo aparece como `d2=2`, erro de transcrição. Pela
> definição de número binário, cada dígito deve ser `0` ou `1`.

#### b) Validar a entrada

Verificar se a quantidade de dígitos não excede o máximo estipulado e se cada dígito pertence ao conjunto `{0, 1}`. A validação deve utilizar um laço `for` e uma estrutura `if`. Caso a entrada contenha um dígito inválido, como o `4` em `410`, informe que o número não está no formato binário válido.

#### c) Converter para decimal

Calcular e mostrar o valor em base decimal correspondente ao número binário armazenado na variável `x`.

### 10. Soma da diagonal principal de uma matriz

Leia uma matriz quadrada com `n` linhas e `n` colunas, com `n` no máximo igual a `100`, e calcule e mostre a soma de todos os elementos da diagonal principal.

## Requisitos gerais

- Cada problema deve ser um programa C independente, com função `main`, que compile sem avisos
  com `cc -std=c11 -Wall -Wextra -pedantic`.
- Os dados são lidos da entrada padrão (`scanf`). Mostre uma mensagem antes de cada leitura e
  trate entradas inválidas (por exemplo, divisão por zero, tamanho fora da faixa permitida).
- Problema 8: a data é lida no formato `dia mês ano`. Valide a data (mês de 1 a 12, dia
  compatível com o mês e com anos bissextos) antes de calcular.
- Problema 9: o número máximo de dígitos `n` deve ser definido em um único lugar do programa (por
  exemplo, uma constante), para poder ser alterado sem mexer no resto do código. O exemplo usa
  `n = 5`.
- Problema 10: a matriz tem no máximo `100 × 100`; valide o `n` digitado.

Para testar seus programas use o script da raiz: salve o arquivo em uma pasta sua e rode
`./run caminho/do/arquivo.c`.

## Soluções

As soluções de referência estão nesta pasta, em arquivos `.c` numerados de `01_` a `10_`. Tente resolver antes de consultar. Para executar: `./run tarefa_1/01_impares.c`.
