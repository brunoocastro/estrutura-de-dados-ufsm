# Recursão em C

Esta pasta reúne exemplos exclusivamente sobre recursão. Cada função mostra o
caso base, o avanço recursivo e, quando útil, a ordem de entrada e retorno das
chamadas.

## Arquivos

| Arquivo | Conteúdo |
| --- | --- |
| `01_fatorial_passos.c` | descida e subida das chamadas de fatorial |
| `02_fibonacci_passos.c` | árvore de chamadas com duas chamadas recursivas |
| `03_soma_vetor.c` | redução recursiva do tamanho de um vetor |
| `04_busca_binaria.c` | descarte recursivo de metade do intervalo |
| `05_palindromo.c` | comparação recursiva das extremidades de uma string |
| `06_mdc.c` | algoritmo de Euclides e resto da divisão |

O exemplo original também está em `../03_funcoes/06_recursao.c`. Ele foi atualizado
para imprimir cada entrada e cada retorno do cálculo de fatorial.

## Compilar e executar

Na raiz do repositório, execute um exemplo:

```bash
cd 3_revisao_sistematica_c/04_recursao
gcc -std=c11 -Wall -Wextra -pedantic 01_fatorial_passos.c -o 01_fatorial_passos
printf "5\n" | ./01_fatorial_passos
```

Fibonacci:

```bash
cd 3_revisao_sistematica_c/04_recursao
gcc -std=c11 -Wall -Wextra -pedantic 02_fibonacci_passos.c -o 02_fibonacci_passos
printf "5\n" | ./02_fibonacci_passos
```

Soma recursiva de vetor:

```bash
cd 3_revisao_sistematica_c/04_recursao
gcc -std=c11 -Wall -Wextra -pedantic 03_soma_vetor.c -o 03_soma_vetor
./03_soma_vetor
```

Busca binária:

```bash
cd 3_revisao_sistematica_c/04_recursao
gcc -std=c11 -Wall -Wextra -pedantic 04_busca_binaria.c -o 04_busca_binaria
printf "25\n" | ./04_busca_binaria
```

Palíndromo:

```bash
cd 3_revisao_sistematica_c/04_recursao
gcc -std=c11 -Wall -Wextra -pedantic 05_palindromo.c -o 05_palindromo
printf "radar\n" | ./05_palindromo
```

MDC:

```bash
cd 3_revisao_sistematica_c/04_recursao
gcc -std=c11 -Wall -Wextra -pedantic 06_mdc.c -o 06_mdc
printf "84 30\n" | ./06_mdc
```

## Compilar todos

```bash
cd 3_revisao_sistematica_c/04_recursao
for arquivo in *.c; do
    executavel="${arquivo%.c}"
    gcc -std=c11 -Wall -Wextra -pedantic "$arquivo" -o "$executavel" || exit 1
done
```

## Como ler o rastreamento

- **ENTRA:** uma nova chamada foi colocada na pilha.
- **CASO BASE:** a função encontrou a condição que encerra a recursão.
- **RETORNA/SAI:** a chamada terminou e devolveu seu resultado para quem a
  chamou.
- A indentação representa o nível da chamada. A execução normalmente desce
  até o caso base e depois sobe calculando os resultados.

Toda recursão precisa de um caso base e de uma mudança que aproxime a entrada
desse caso. Recursões muito profundas consomem a pilha de chamadas e podem
causar *stack overflow*; em alguns problemas uma solução iterativa é preferível.
