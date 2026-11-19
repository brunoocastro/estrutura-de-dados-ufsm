# 03 — Revisão sistemática de C

Este módulo revisa a linguagem C com foco no que as estruturas de dados vão exigir: **memória**.
Em todos os exemplos o aluno vê onde cada dado fica (endereço), quanto ocupa (`sizeof`) e quanto
custa cada operação (Big O). A introdução geral a Big O está em
[`../01_revisao_c_basico/complexidade_big_o.md`](../01_revisao_c_basico/complexidade_big_o.md).

## O que a ementa pede e onde está

| Ementa | Tópico | Onde |
| --- | --- | --- |
| 0.4 | Vetores e matrizes | [`02_tipos_compostos/`](02_tipos_compostos/) (01–03), [`05_alocacao_dinamica/04`](05_alocacao_dinamica/) |
| 0.5 | Strings | [`02_tipos_compostos/`](02_tipos_compostos/) (04–06), [`05_alocacao_dinamica/07`](05_alocacao_dinamica/) |
| 0.6 | Funções e recursividade | [`03_funcoes/`](03_funcoes/), [`04_recursao/`](04_recursao/) |
| 0.7 | Ponteiros e alocação dinâmica | [`02_tipos_compostos/`](02_tipos_compostos/) (11–13), [`05_alocacao_dinamica/`](05_alocacao_dinamica/) |
| 1.1 | Tipos primitivos, structs | [`01_tipos_simples/`](01_tipos_simples/), [`02_tipos_compostos/`](02_tipos_compostos/) (07–10) |

Os materiais do Classroom **"Revisão sobre funções, recursividade e ponteiros"** e
**"Recursividade"** correspondem às pastas `03_funcoes/`, `04_recursao/` e aos arquivos de
ponteiros de `02_tipos_compostos/`.

## Ordem de estudo das 5 subpastas

| # | Pasta | O que mostra | Complexidade-chave |
| --- | --- | --- | --- |
| 1 | [`01_tipos_simples/`](01_tipos_simples/) | `char`, inteiros, reais, conversões, constantes, `<stdint.h>` | tudo O(1) |
| 2 | [`02_tipos_compostos/`](02_tipos_compostos/) | vetores, matrizes, strings, structs, unions, enums e ponteiros | `v[i]` O(1); busca O(n); `strlen` O(n); inserir no meio O(n) |
| 3 | [`03_funcoes/`](03_funcoes/) | protótipos, escopo, passagem por valor/ponteiro, `const`, ponteiro para função | passar vetor para função: O(1) |
| 4 | [`04_recursao/`](04_recursao/) | pilha de chamadas desenhada, Fibonacci com memoização, Hanói, potência rápida, cauda, estouro de pilha | O(2^n) × O(n) × O(log n) |
| 5 | [`05_alocacao_dinamica/`](05_alocacao_dinamica/) | stack × heap, `malloc/calloc/realloc/free`, vetor que dobra, matriz dinâmica, erros clássicos, log de sensores | inserção amortizada O(1) |

Cada subpasta tem um `README.md` com o conceito, a tabela de arquivos, exemplos reais e exercícios.

## A memória de um programa C (o mapa que guia o módulo)

```text
 endereços altos
 +------------------------+
 | PILHA (stack)          |  locais e parâmetros; um quadro por chamada de função;
 |   quadro de main       |  criada/destruída automaticamente; pequena (~8 MB)
 |   quadro de f          |
 |   quadro de g   <- topo|  cresce para BAIXO
 |          ...           |
 |          ...           |  cresce para CIMA
 | HEAP                   |  malloc/calloc/realloc ... free; grande; você gerencia
 +------------------------+
 | dados estáticos        |  globais e static: existem o programa inteiro
 | código e literais      |  somente leitura
 +------------------------+
 endereços baixos
```

## Tabela-resumo de custos

| Operação | Custo | Exemplo |
| --- | --- | --- |
| acessar `v[i]`, `m[l][c]`, `s->campo`, `*p` | O(1) | `02_tipos_compostos/01_arrays.c` |
| procurar valor em vetor não ordenado | O(n) | `01_arrays.c` |
| inserir/remover no meio de vetor | O(n) | `02_vetor_insercao_remocao.c` |
| `strlen`, `strcpy`, `strcat` | O(n) | `06_strings_custo_oculto.c` |
| busca binária (vetor ordenado) | O(log n) | `04_recursao/04_busca_binaria.c` |
| Fibonacci ingênuo × memoizado | O(2^n) × O(n) | `04_recursao/07_fibonacci_memoizacao.c` |
| Torres de Hanói | O(2^n) | `04_recursao/08_torres_de_hanoi.c` |
| potência rápida | O(log e) | `04_recursao/09_potencia_rapida.c` |
| inserir no fim de vetor dinâmico (dobrando) | O(1) amortizado | `05_alocacao_dinamica/03_vetor_dinamico.c` |
| recursão de profundidade n | O(n) de pilha | `04_recursao/11_estouro_de_pilha.c` |

## Exemplos reais usados

- **Controle e automação:** registradores Modbus (`uint16_t`, cursor de quadro com `**`), leitura
  de ADC, encoder que "dá a volta", tabela de calibração, mapa de temperaturas de um forno,
  matriz de distâncias de um AGV, data logger de sensores com buffer crescente, troca de chaves
  Diffie-Hellman entre SCADA e RTU, firmware com pilha de poucos KB.
- **Cotidiano:** fila de pedidos, montagem de linhas CSV, leitura de arquivos de configuração.

## Como executar

Na raiz do repositório (o script compila para `build/`):

```sh
./run 01_fatorial_passos                    # compila se preciso e executa
./run 07_fibonacci_memoizacao < /dev/null   # sem pausas (roteiro automático)
./run list 03_revisao_sistematica_c         # lista os arquivos do módulo
```

Programas com demonstração passo a passo pausam a cada etapa (tecle ENTER) quando executados no
terminal; com a entrada redirecionada (`< /dev/null`) rodam direto. Programas que pedem dados
usam valores de exemplo quando a entrada está vazia.

Para conferir erros de memória, compile com o sanitizer:

```sh
CFLAGS="-std=c11 -Wall -Wextra -pedantic -g -fsanitize=address,undefined" ./run 05_erros_classicos_memoria
```

## Próximos módulos

Os conceitos daqui são usados diretamente em [`../04_tads/`](../04_tads/) (structs + ponteiros),
[`../05_pilhas/`](../05_pilhas/) (a pilha de chamadas é uma pilha!), [`../06_filas/`](../06_filas/),
[`../07_listas/`](../07_listas/) (nós alocados com `malloc`) e
[`../08_ordenacao_busca/`](../08_ordenacao_busca/).
