# Tarefa 1

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

\[
v = \sum_{i=1}^{n} 2^{i-1}d_i
\]

O programa deve:

#### a) Exibir os dígitos

Ler um número inteiro decimal em uma variável chamada `x`, usando `scanf("%d", &x)`, e exibir seus `n` dígitos separadamente, completando com zeros à esquerda quando necessário. Para essa saída, deve ser utilizado um laço `for`.

Por exemplo, para `x = 110` e `n = 5`, exibir:

```text
d5=0 d4=0 d3=1 d2=2 d1=0
```

> Observação: seguindo a definição de número binário, a entrada válida deve conter apenas os dígitos `0` e `1`; portanto, o exemplo acima parece conter um erro de transcrição em `d2` e deve ser interpretado conforme o formato binário especificado.

#### b) Validar a entrada

Verificar se a quantidade de dígitos não excede o máximo estipulado e se cada dígito pertence ao conjunto `{0, 1}`. A validação deve utilizar um laço `for` e uma estrutura `if`. Caso a entrada contenha um dígito inválido, como o `4` em `410`, informe que o número não está no formato binário válido.

#### c) Converter para decimal

Calcular e mostrar o valor em base decimal correspondente ao número binário armazenado na variável `x`.

### 10. Soma da diagonal principal de uma matriz

Leia uma matriz quadrada com `n` linhas e `n` colunas, com `n` no máximo igual a `100`, e calcule e mostre a soma de todos os elementos da diagonal principal.

## Arquivos e execução

Cada solução está em um arquivo C independente:

| Problema | Arquivo |
| --- | --- |
| 1 | `01_impares.c` |
| 2 | `02_divisores.c` |
| 3 | `03_primo.c` |
| 4 | `04_proximo_primo.c` |
| 5 | `05_notas.c` |
| 6 | `06_vetor_reais.c` |
| 7 | `07_moda.c` |
| 8 | `08_dias.c` |
| 9 | `09_binario.c` |
| 10 | `10_diagonal.c` |

No terminal, entre nesta pasta e compile o arquivo desejado com `gcc`:

```bash
cd 0_revisao/tarefa_1
gcc -std=c11 -Wall -Wextra -pedantic 01_impares.c -o 01_impares
./01_impares
```

Para executar qualquer outro problema, substitua o nome do arquivo e do executável. Por exemplo:

```bash
gcc -std=c11 -Wall -Wextra -pedantic 05_notas.c -o 05_notas
printf "6 8 6\n" | ./05_notas
```

Os programas leem os valores pela entrada padrão (`stdin`). Assim, os dados podem ser digitados diretamente ou redirecionados de um arquivo:

```bash
./06_vetor_reais < entrada.txt
```

O programa do problema 8 recebe a data no formato `dia mês ano` (por exemplo, `1 1 2026`) e calcula os dias até a data atual. O programa do problema 9 usa `N=5` dígitos como máximo; esse limite pode ser alterado na constante `N` do arquivo.
