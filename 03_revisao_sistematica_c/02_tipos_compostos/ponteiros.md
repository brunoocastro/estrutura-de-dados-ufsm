# Ponteiros em C — resumo

Um ponteiro é uma variável que armazena o endereço de outra variável. O tipo do
ponteiro indica como interpretar os bytes encontrados nesse endereço.

```c
int numero = 42;
int *ponteiro = &numero;
```

- `&numero` obtém o endereço de `numero`;
- `int *ponteiro` declara um ponteiro para `int`;
- `*ponteiro` acessa o valor armazenado no endereço apontado.

```c
printf("valor = %d\n", numero);
printf("endereco = %p\n", (void *)&numero);
printf("valor via ponteiro = %d\n", *ponteiro);
```

## Alteração por ponteiro

Como o ponteiro conhece o endereço, uma função pode alterar a variável original:

```c
void dobra(int *valor) {
    *valor *= 2;
}

int numero = 10;
dobra(&numero); /* numero passa a valer 20 */
```

C não possui passagem por referência nativa. Esse padrão usa passagem por valor
do endereço para permitir acesso ao objeto original.

## Ponteiros e arrays

O nome de um array geralmente converte-se em ponteiro para o primeiro elemento:

```c
int valores[3] = {10, 20, 30};
int *p = valores;

printf("%d\n", p[1]);       /* 20 */
printf("%d\n", *(p + 2));   /* 30 */
```

`valores[i]` e `*(valores + i)` são formas equivalentes de acessar o elemento.
A aritmética de ponteiros considera o tamanho do tipo: `p + 1` avança
`sizeof(int)` bytes, não apenas um byte.

## Ponteiro e `const`

```c
const int *p;       /* não altera o int através de p */
int *const p;       /* o endereço armazenado em p não muda */
const int *const p; /* nem o endereço nem o valor podem ser alterados */
```

Use `const` em parâmetros quando uma função apenas consulta os dados:

```c
void mostra(const int valores[], size_t tamanho);
```

## Ponteiro nulo

`NULL` representa a ausência de um endereço válido:

```c
int *p = NULL;
if (p != NULL)
    printf("%d\n", *p);
```

Nunca desreferencie um ponteiro nulo ou não inicializado. Isso causa
comportamento indefinido.

## Memória dinâmica

`malloc` reserva memória no *heap* e retorna um ponteiro para o bloco:

```c
int *valores = malloc(3 * sizeof *valores);
if (valores != NULL) {
    valores[0] = 10;
    free(valores);
}
```

Toda memória obtida com `malloc`, `calloc` ou `realloc` deve ser liberada com
`free` quando não for mais necessária. Depois de liberar, não use o ponteiro
como se ele ainda apontasse para um objeto válido.

## Ponteiros para structs

Com uma variável de struct usamos `.`; com um ponteiro usamos `->`:

```c
struct Ponto { int x; int y; } ponto = {1, 2};
struct Ponto *p = &ponto;

ponto.x = 10;
p->y = 20;          /* equivalente a (*p).y = 20 */
```

## Aritmética de ponteiros e custo

| Expressão | Significado | Custo |
| --- | --- | --- |
| `p + k` | endereço k elementos à frente (`k * sizeof(*p)` bytes) | O(1) |
| `*(p + k)` ou `p[k]` | o elemento nessa posição | O(1) |
| `q - p` | distância em elementos (mesmo vetor) | O(1) |
| `v + n` | "um depois do último": pode comparar, nunca desreferenciar | O(1) |
| percorrer com `for (q = v; q != v + n; q++)` | visita cada elemento | O(n) |

Ponteiro para ponteiro (`T **`) é usado quando a função precisa **alterar o ponteiro** do
chamador (por exemplo, avançar um cursor de leitura ou fazer `*pp = malloc(...)`).

## Exemplos executáveis

```sh
./run 11_ponteiros_simples       # &x, p e *p com desenho das caixas
./run 12_ponteiros               # **, ponteiro em função, vetor, struct, NULL, malloc
./run 13_aritmetica_ponteiros    # ponteiro andando no vetor; cursor em quadro Modbus
```

Memória dinâmica em profundidade: [`../05_alocacao_dinamica/`](../05_alocacao_dinamica/).

## Cuidados principais

- Inicialize ponteiros antes de usá-los;
- Verifique `malloc` antes de acessar a memória;
- Não acesse além dos limites de um array;
- Não retorne o endereço de uma variável local que deixará de existir;
- Não use memória depois de `free` (*use-after-free*);
- Não confunda o endereço (`p`) com o valor apontado (`*p`).
