# Pilhas em C

Uma pilha é uma estrutura de dados **LIFO** (*Last In, First Out*): o último
elemento inserido é o primeiro a sair. A operação de inserção é `push`, a
remoção é `pop` e a consulta do topo sem remover é `peek`. Todos os exemplos
possuem uma função que imprime a pilha do topo para a base, mostrando o valor,
o endereço de memória em formato numérico e, na pilha dinâmica, o ponteiro para
o próximo nó. Também é mostrada a distância em bytes em relação ao primeiro
elemento das pilhas baseadas em array.

## Arquivos

| Arquivo | Conteúdo |
| --- | --- |
| `00_visao_geral.c` | operações básicas de uma pilha LIFO |
| `01_pilha_array.c` | implementação com array e capacidade fixa |
| `02_pilha_dinamica.c` | implementação com lista encadeada e `malloc` |
| `03_parenteses.c` | verificação de delimitadores balanceados |
| `04_conversao_posfixa.c` | uso de pilha para converter expressões |
| `05_pilha_portugues.c` | pilha simples com `coloca` e `retira`, aplicada a histórico de navegador |
| `06_valida_codigo_c.c` | parsing simples de código C e validação de delimitadores |
| `07_valida_chaves.c` | exemplo introdutório que valida somente `{` e `}` |
| `08_pilha_basica.c` | pilha mínima com array global, sem `struct` e sem ponteiros explícitos |
| `09_pilha_strings.c` | pilha básica de strings usando array bidimensional |
| `10_valida_colchetes.c` | exemplo original que valida somente colchetes |
| `11_valida_delimitadores.c` | uma única pilha para validar parênteses, colchetes e chaves |

## Compilar e executar

```bash
cd 4_pilhas
gcc -std=c11 -Wall -Wextra -pedantic 00_visao_geral.c -o 00_visao_geral
./00_visao_geral
```

Para compilar todos os exemplos:

```bash
cd 4_pilhas
for arquivo in *.c; do
    executavel="${arquivo%.c}"
    gcc -std=c11 -Wall -Wextra -pedantic "$arquivo" -o "$executavel" || exit 1
done
```

Para executar todos:

```bash
cd 4_pilhas
for executavel in 00_visao_geral 01_pilha_array 02_pilha_dinamica 03_parenteses 04_conversao_posfixa 05_pilha_portugues 06_valida_codigo_c 07_valida_chaves 08_pilha_basica 09_pilha_strings 10_valida_colchetes 11_valida_delimitadores; do
    echo "=== $executavel ==="
    ./$executavel
done
```

## Endereços numéricos e memória

Os exemplos incluem `<stdint.h>` e `<inttypes.h>` para converter endereços para
`uintptr_t` e imprimi-los com `PRIuPTR`. Isso permite observar números de endereço
e perceber que elementos contíguos avançam `sizeof(tipo)` bytes. Para imprimir
ponteiros em mensagens comuns, `%p` continua sendo a forma recomendada.

Para um array `v` com `n` elementos:

```c
sizeof v == n * sizeof v[0]
```

O limite teórico de elementos pode ser estimado por `SIZE_MAX / sizeof v[0]`, mas
o limite prático depende da memória, do sistema operacional e da pilha disponível.
A capacidade definida por `#define CAPACIDADE` é o limite da implementação
 didática, não o limite máximo absoluto da máquina.

## Implementações

### Array de capacidade fixa

Um array reserva a capacidade inteira antecipadamente. É simples e possui
memória contígua, mas pode ocorrer *overflow* quando a pilha está cheia. Uma
pilha vazia pode ser representada por `topo == -1` ou por `topo == 0`, dependendo
da convenção escolhida.

### Lista encadeada

Cada `push` aloca um novo nó no *heap* e cada `pop` libera o nó removido. A
capacidade cresce conforme a memória disponível, mas cada elemento precisa de
um ponteiro adicional e as alocações têm custo maior.

### Underflow e overflow

- **Underflow:** tentar executar `pop` em uma pilha vazia.
- **Overflow:** tentar executar `push` em uma pilha de capacidade fixa cheia.
- Em uma pilha dinâmica, `push` pode falhar se `malloc` retornar `NULL`.

### Exemplo simples em português

O arquivo `05_pilha_portugues.c` apresenta uma implementação introdutória com
nomes em português:

- `coloca`: insere uma página no topo;
- `retira`: remove a página mais recente;
- `esta_vazia`: verifica se não há páginas;
- `esta_cheia`: verifica se a capacidade foi atingida;
- `mostra_pilha`: imprime o histórico do topo para a base.

A aplicação representa o histórico de páginas de um navegador. Ao visitar uma
página, ela é colocada na pilha. Ao selecionar "voltar", a página do topo é
retirada, reproduzindo o comportamento LIFO.

### Parsing simples de código C

O arquivo `06_valida_codigo_c.c` simula uma etapa de um compilador. Ele percorre
um pequeno trecho de código, colocando na pilha cada abertura `(`, `[` ou `{`.
Ao encontrar `)`, `]` ou `}`, retira a última abertura e verifica se os símbolos
correspondem. O programa também registra linha e coluna para simular uma
mensagem de erro de sintaxe.

Strings e comentários são ignorados durante a análise, pois delimitadores dentro
de textos ou comentários não devem ser tratados como parte da estrutura do
código. O exemplo não é um compilador completo: ele valida apenas o aninhamento
dos delimitadores.

Compile e execute:

```bash
cd 4_pilhas
gcc -std=c11 -Wall -Wextra -pedantic 05_pilha_portugues.c -o 05_pilha_portugues
./05_pilha_portugues
```

Validação de um pequeno trecho de código C:

```bash
cd 4_pilhas
gcc -std=c11 -Wall -Wextra -pedantic 06_valida_codigo_c.c -o 06_valida_codigo_c
./06_valida_codigo_c
```

Exemplo simplificado, que valida somente chaves:

```bash
cd 4_pilhas
gcc -std=c11 -Wall -Wextra -pedantic 07_valida_chaves.c -o 07_valida_chaves
./07_valida_chaves
```

### Pilha básica de strings

O arquivo `09_pilha_strings.c` armazena textos em uma pilha. Cada posição é um
array de caracteres:

```c
char pilha[CAPACIDADE][TAMANHO_STRING];
```

A primeira dimensão define quantas strings cabem. A segunda define o número
máximo de caracteres reservado para cada string, incluindo o terminador `\\0`.
O exemplo usa `strncpy` para limitar a cópia e garante manualmente o terminador.

`strncpy(destino, origem, limite)` copia no máximo `limite` caracteres da origem
para o destino. Quando a origem é grande demais, `strncpy` pode não acrescentar
`'\\0'`; por isso o programa reserva uma posição e executa explicitamente:

```c
pilha[topo][TAMANHO_STRING - 1] = '\\0';
```

Assim, o conteúdo permanece uma string C válida, mesmo quando a entrada é maior
que o espaço reservado.

```bash
cd 4_pilhas
gcc -std=c11 -Wall -Wextra -pedantic 09_pilha_strings.c -o 09_pilha_strings
./09_pilha_strings
```

Atividade 11, usando uma única pilha para os três tipos de delimitadores:

```bash
cd 4_pilhas
gcc -std=c11 -Wall -Wextra -pedantic 11_valida_delimitadores.c -o 11_valida_delimitadores
./11_valida_delimitadores
```

### Exemplo mais básico: pilha sem `struct`

O arquivo `08_pilha_basica.c` é a versão mais simples da pilha. Ele usa apenas:

- um array global chamado `pilha`;
- uma variável global chamada `topo`;
- a função `coloca`, que insere um valor;
- a função `retira`, que remove o valor do topo;
- a função `mostra`, que imprime os valores.

Não há `struct` nem ponteiros escritos no código. A variável `topo` indica a
próxima posição livre: quando `topo` vale `0`, a pilha está vazia; quando vale
`3`, existem três elementos nas posições `0`, `1` e `2`.

```bash
cd 4_pilhas
gcc -std=c11 -Wall -Wextra -pedantic 08_pilha_basica.c -o 08_pilha_basica
./08_pilha_basica
```

### Exemplo introdutório: validação de chaves

O arquivo `07_valida_chaves.c` é uma versão simplificada do parser. Ele analisa
somente os caracteres `{` e `}`:

- Ao encontrar `{`, executa `coloca` na pilha;
- Ao encontrar `}`, executa `retira` da pilha;
- Se houver `}` sem uma abertura anterior, o código é inválido;
- Se sobrarem `{` no final, falta uma chave de fechamento;
- Se o topo terminar em zero, as chaves estão balanceadas.

Ele é uma boa primeira aplicação antes de estudar o parser mais completo do
arquivo `06_valida_codigo_c.c`.

### Atividade 11: uma pilha para todos os pareamentos

O arquivo `11_valida_delimitadores.c` evolui o exemplo de colchetes da atividade
10. Ele usa uma única pilha para os símbolos `(`, `[`, `{` e verifica os pares
`)`, `]`, `}`.

O algoritmo é:

1. Ao encontrar uma abertura, executa `coloca`;
2. Ao encontrar um fechamento, executa `retira`;
3. Confere se a abertura retirada forma o par correto;
4. Ao final, verifica se a pilha ficou vazia.

Assim, `"{[a]}"` é válida, mas `"{[a}]"` é inválida: mesmo contendo a mesma
quantidade de símbolos, a ordem de fechamento está errada.

### Aplicações

Pilhas aparecem em chamadas de funções, desfazer/refazer operações, histórico
de navegação, análise de expressões, correspondência de parênteses, conversão
de expressões e algoritmos de busca.
