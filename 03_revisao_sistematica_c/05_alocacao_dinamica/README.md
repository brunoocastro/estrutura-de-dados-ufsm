# 05 — Alocação dinâmica de memória

**Ementa:** 0.7 (ponteiros e alocação dinâmica de memória).

Vetores comuns têm tamanho fixo, decidido na compilação, e variáveis locais morrem quando a
função retorna. A **alocação dinâmica** resolve as duas limitações: pede memória ao sistema em
**tempo de execução** (no *heap*), no tamanho necessário, e o bloco vive até você chamar `free`.
É a base de todas as estruturas que crescem: listas, pilhas e filas encadeadas, árvores, grafos.

```text
   PILHA (automática)                    HEAP (manual)
  +--------------------+               +-----------------------------+
  | main               |               |                             |
  |   int n = 5        |               |   [ 10 | 20 | 30 | 40 | 50 ] |  <- malloc(5 * sizeof(int))
  |   int *v ----------+-------------->|                             |
  +--------------------+               +-----------------------------+
  some no fim da função                  existe até free(v)

  malloc(bytes)        bloco com lixo          calloc(n, tam)   bloco zerado
  realloc(p, novo)     aumenta/diminui; pode MUDAR de endereço (copia os dados)
  free(p)              devolve; depois disso, p = NULL
```

## Ordem de estudo

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `01_stack_vs_heap.c` | mapa da memória com endereços reais; quadros reaproveitados; heap sobrevive ao `return` | pilha O(1); `malloc` ~O(1) |
| `02_malloc_calloc_realloc_free.c` | cada função num passo, bloco desenhado; `realloc` seguro; overflow em `n*sizeof` | `calloc` O(n); `realloc` O(1) ou O(n) |
| `03_vetor_dinamico.c` | vetor que dobra × cresce +1, **contando cópias** para n = 10…10 000 | O(1) amortizado × O(n) |
| `04_matriz_dinamica.c` | `int **` × bloco contíguo × híbrido; matriz triangular | acesso O(1); L+1 × 1 × 2 mallocs |
| `05_erros_classicos_memoria.c` | vazamento, double free, ponteiro pendurado, uso após free... com um rastreador de alocações | rastreador O(k) |
| `06_log_sensores.c` | **exemplo real**: data logger de 3 sensores com buffer que cresce durante o ensaio | inserção O(1) amortizado |
| `07_leitura_linha_dinamica.c` | linhas de qualquer tamanho + vetor de `char *` (matriz irregular de texto) | O(L) amortizado por linha |

## Por que dobrar? (custo amortizado)

| n inserções | cópias com ×2 | cópias com +1 |
| --- | --- | --- |
| 10 | 15 | 45 |
| 100 | 127 | 4 950 |
| 1 000 | 1 023 | 499 500 |
| 10 000 | 16 383 | 49 995 000 |

Com ×2, as cópias acontecem quando o tamanho chega a 1, 2, 4, 8, …, e a soma 1 + 2 + 4 + … + n/2
é menor que n. Logo o custo médio por inserção é constante (O(1) amortizado), mesmo que uma
inserção isolada custe O(n). Com +1, toda inserção copia tudo: O(n) cada, O(n²) no total.
(Números contados executando `03_vetor_dinamico.c`.)

## Comparação: vetor fixo × vetor dinâmico × matriz

| Aspecto | Vetor fixo `int v[N]` | Vetor dinâmico (heap) | Matriz `int **` | Matriz contígua |
| --- | --- | --- | --- | --- |
| tamanho decidido | na compilação | em execução | em execução | em execução |
| pode crescer | não | sim (`realloc`) | linhas sim | só realocando tudo |
| acesso | O(1) | O(1) | O(1), 2 leituras | O(1), 1 leitura + conta |
| liberação | automática | 1 `free` | L + 1 `free` | 1 `free` |
| risco | estouro do vetor | vazamento, uso após free | vazamento parcial | índice `l*C + c` errado |

## Erros clássicos (todos explicados em `05_erros_classicos_memoria.c`)

| Erro | Sintoma | Prevenção |
| --- | --- | --- |
| vazamento (*leak*) | memória cresce até acabar | um `free` por `malloc`, em todos os caminhos |
| double free | aborto ou corrupção "aleatória" | `free(p); p = NULL;` |
| ponteiro pendurado | dado muda sozinho | um dono por bloco; zere todas as cópias |
| uso após free | funciona no teste, falha em campo | copie o necessário antes do `free` |
| esquecer o `+1` do `'\0'` | escreve 1 byte além | `malloc(strlen(s) + 1)` |
| não testar `NULL` | queda quando falta memória | `if (p == NULL) { ... }` |
| `p = realloc(p, ...)` | perde o bloco se falhar | use um ponteiro temporário |

Ferramentas: compile com `-fsanitize=address,undefined` (ou use `valgrind` no Linux).

## Exemplos reais

- Data logger que não sabe quantas amostras um ensaio vai gerar (`06_log_sensores.c`).
- Gateway que lê do arquivo de configuração quantos dispositivos Modbus vai consultar.
- Mapa de temperaturas L×C de um forno; matriz de distâncias entre estações de um AGV.
- Linhas de receitas de produção e comandos de tamanho variável chegando pela serial.

## Como executar

```sh
./run 03_vetor_dinamico                     # ENTER avança cada inserção
./run 06_log_sensores < /dev/null           # roteiro automático, sem pausas
./run 07_leitura_linha_dinamica < README.md # lê um arquivo qualquer
CFLAGS="-std=c11 -Wall -Wextra -pedantic -g -fsanitize=address,undefined" ./run 05_erros_classicos_memoria
```

## Exercícios propostos

1. Acrescente a `03_vetor_dinamico.c` a remoção do último elemento com a regra "se ficar com
   1/4 da capacidade, reduza à metade". Por que não reduzir já com 1/2?
2. Troque o fator de crescimento por 1,5 e refaça a tabela de cópias e de memória ociosa.
3. Escreva `char *duplica(const char *s)` (como `strdup`) e `char *concatena(const char *a,
   const char *b)` que devolvem blocos novos. Quem deve dar `free`? Documente no comentário.
4. Escreva `int **cria_identidade(size_t n)` e a função de liberação correspondente; trate falha
   no meio da alocação sem vazar memória.
5. Modifique `06_log_sensores.c` para manter apenas as últimas N leituras em um buffer circular
   de tamanho fixo e compare o uso de memória com o log que cresce.
6. Rode `05_erros_classicos_memoria.c` com `-fsanitize=address` e depois escreva um programa
   pequeno com um uso após free **de propósito**. Leia e explique a mensagem do sanitizer.

Anterior: [`../04_recursao/`](../04_recursao/). Próximo módulo: [`../../04_tads/`](../../04_tads/)
e as listas encadeadas em [`../../07_listas/`](../../07_listas/).
