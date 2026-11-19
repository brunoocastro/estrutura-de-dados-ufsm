# Listas encadeadas em C

Uma **lista encadeada** guarda uma sequência em **nós** espalhados pela memória. Cada nó tem o
dado e um ponteiro para o próximo nó. É o exemplo clássico de **TAD de definição recorrente**: o
tipo `struct No` contém um ponteiro para outro `struct No`.

## O que a ementa pede e onde está

A ementa da disciplina (UFSM00064) não tem uma unidade só para listas. Elas aparecem como o TAD
recorrente da **Unidade 1.3 (definição recorrente de TADs)** e usam **ponteiros e alocação dinâmica
(0.7)**. Também são a base das pilhas e filas dinâmicas (Unidades 2 e 3) e das árvores e grafos
(Unidades 5 e 6).

| Item | Onde está nesta pasta |
| --- | --- |
| 1.3 TAD recorrente: nó que aponta para nó | `01_lista_encadeada.c` (TAD completo com índice) |
| 0.7 Ponteiros, `malloc`/`free`, ponteiro para ponteiro (`**`) | `02_lista_passo_a_passo.c` |
| Custo das operações: vetor × lista | `03_vetor_vs_lista.c` |
| Lista duplamente encadeada | `04_lista_dupla_alarmes_ihm.c` |
| Lista circular (e detecção de ciclo) | `05_lista_circular_carrossel_ihm.c` |
| Nó sentinela / nó-cabeça | `06_lista_sentinela.c` |
| Tarefa Avaliada 05 | [enunciado abaixo](#tarefa-avaliada-05), sem solução |

Pré-requisitos: ponteiros e alocação dinâmica em
[`../03_revisao_sistematica_c/`](../03_revisao_sistematica_c/README.md), TADs em
[`../04_tads/`](../04_tads/) e notação Big O em
[`../01_revisao_c_basico/complexidade_big_o.md`](../01_revisao_c_basico/complexidade_big_o.md).

## Conceito

### Lista simplesmente encadeada

```text
inicio
  |
  v
[10|*]-->[20|*]-->[30|*]-->NULL
 @a010    @a030    @a050          endereços (exemplo): os nós NÃO são vizinhos na memória
```

```c
typedef struct No {
    int valor;
    struct No *prox;   /* endereço do próximo nó; NULL no último */
} No;
```

- Lista vazia: `inicio == NULL`.
- Só se conhece o endereço do primeiro nó. Para chegar ao i-ésimo é preciso seguir `i` ponteiros.
- Inserir ou remover é **religar ponteiros** (O(1)). O custo O(n) vem de **achar a posição**.

### Inserir e remover no meio

```text
inserir 25 entre [20] e [30]
  antes:   [20|*]------------->[30|*]
  depois:  [20|*]-->[25|*]-->[30|*]
           1º novo->prox = anterior->prox   (o 25 aponta para o 30)
           2º anterior->prox = novo         (o 20 aponta para o 25)
           Na ordem inversa, o endereço do [30] se perderia.

remover [20]
  antes:   [10|*]-->[20|*]-->[30|*]
  depois:  [10|*]------------->[30|*]      anterior->prox = alvo->prox;  free(alvo);
```

### Ponteiro para o elo (`No **`)

O ponteiro que precisa ser alterado ao inserir ou remover pode ser a variável `inicio` ou o campo `prox`
de algum nó. Um `No **elo` aponta para **esse ponteiro**, seja ele qual for, e por isso o mesmo
código vale para o início, o meio e o fim, sem `if (anterior == NULL)`. A demonstração completa,
com endereços, está em `02_lista_passo_a_passo.c`.

### Variantes

```text
Dupla:      NULL<--[*|10|*]<-->[*|20|*]<-->[*|30|*]-->NULL      (ant e prox)

Circular:   +-->[10|*]-->[20|*]-->[30|*]--+                      (último aponta para o primeiro)
            +-----------------------------+

Sentinela:  sentinela:[ S|*]-->[10|*]-->[20|*]-->NULL            (nó fantasma no início)

Dupla circular com sentinela:
            +->[ S]<->[10]<->[20]<-+                             (nenhum ponteiro vale NULL)
            +----------------------+
```

## Ordem de estudo recomendada

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `01_lista_encadeada.c` | TAD de referência: inserir/remover/obter por índice, buscar, liberar; índices inválidos | inserir no início O(1); por índice O(n) |
| `02_lista_passo_a_passo.c` | Cada operação desenhada com endereços: inserção no início, no fim e ordenada; remoção do início, do meio e do fim; `No **elo`; `free` e ponteiro pendente | religar O(1), buscar a posição O(n) |
| `03_vetor_vs_lista.c` | Vetor desloca × lista religa; acesso direto × caminhar; tabelas para n = 10…10000; armadilha do `for (i) acessa(i)` | início: O(n) × O(1); i-ésimo: O(1) × O(n) |
| `04_lista_dupla_alarmes_ihm.c` | Histórico de alarmes de uma IHM: registrar, navegar `[<]`/`[>]`, reconhecer (remover dado o nó), descartar o mais antigo; menu livre | remover dado o nó O(1) |
| `05_lista_circular_carrossel_ihm.c` | Carrossel de telas com ponteiro só para a cauda; um nó apontando para si; bônus: Floyd (tartaruga e lebre) e liberação segura de lista com ciclo | avançar O(1); Floyd O(n) tempo, O(1) espaço |
| `06_lista_sentinela.c` | Mesmo roteiro com e sem sentinela (contador de casos especiais); dupla circular com 0 `if`; sentinela como guarda na busca | mesmo Big O, menos casos e menos testes |

## Complexidade: operações × implementações

| Operação | Vetor | Simples | Simples + cauda | Dupla (início e fim) | Circular (só cauda) |
| --- | --- | --- | --- | --- | --- |
| Acessar o i-ésimo | O(1) | O(n) | O(n) | O(n) | O(n) |
| Buscar valor | O(n) | O(n) | O(n) | O(n) | O(n) |
| Inserir no início | O(n) | O(1) | O(1) | O(1) | O(1) |
| Inserir no fim | O(1)* | O(n) | O(1) | O(1) | O(1) |
| Remover do início | O(n) | O(1) | O(1) | O(1) | O(1) |
| Remover do fim | O(1) | O(n) | O(n) | O(1) | O(n) |
| Remover dado o ponteiro do nó | — | O(n) | O(n) | **O(1)** | O(n) |
| Voltar ao anterior | O(1) | O(n) | O(n) | **O(1)** | O(n) |
| Memória extra por elemento | 0 | 1 ponteiro | 1 ponteiro | 2 ponteiros | 1 ponteiro |

(*) amortizado, se houver capacidade sobrando ou `realloc` com dobra. Na simples com cauda,
remover do fim continua O(n) porque é preciso achar o penúltimo para atualizar a cauda.

## Exemplos reais

- **Automação:** lista de alarmes ativos de um SCADA, que entram e saem de qualquer posição;
  histórico de alarmes navegável numa IHM (lista dupla); carrossel de telas e anel de passagem de
  token Profibus (circular); listas de tarefas prontas do FreeRTOS (dupla circular com
  marcador de fim, uma sentinela); fila de ordens de produção reordenada por prioridade.
- **Computação:** `struct list_head` do kernel Linux (dupla circular com nó-cabeça); alocadores
  de memória (lista de blocos livres); cache LRU (lista dupla com tabela hash); histórico
  voltar/avançar do navegador e desfazer/refazer de editores.
- **Base de outras estruturas:** pilha dinâmica ([`../05_pilhas/`](../05_pilhas/)), fila
  encadeada ([`../06_filas/`](../06_filas/)), listas de adjacência de grafos
  ([`../10_grafos/`](../10_grafos/)) e nós de árvores ([`../09_arvores/`](../09_arvores/)).

**Lista não é fila nem pilha.** Uma lista permite operar em qualquer posição. Fila (FIFO) e pilha
(LIFO) são **regras de acesso**, que podem ser implementadas com vetor ou com nós encadeados. O
encadeamento é uma forma de organizar a memória.

## Como executar

Na raiz do repositório (o script compila para `build/`):

```sh
./run list listas                    # lista os arquivos desta pasta
./run 02_lista_passo_a_passo         # ENTER avança cada passo
./run 03_vetor_vs_lista < /dev/null  # executa direto, sem pausas
./run 04_lista_dupla_alarmes_ihm     # roteiro e, depois, menu: < > r n l q
```

Para conferir vazamentos e acessos inválidos, como na verificação do repositório:

```sh
cc -std=c11 -Wall -Wextra -pedantic -g -fsanitize=address,undefined \
   07_listas/02_lista_passo_a_passo.c -o /tmp/lista && /tmp/lista < /dev/null
```

## Exercícios propostos (para praticar, sem solução)

1. **Cauda O(1).** Acrescente à `Lista` de `01_lista_encadeada.c` um ponteiro `fim`. Quais funções
   precisam atualizá-lo? Teste inserir e remover quando a lista tem 0, 1 e 2 nós.
2. **Inverter no lugar.** Inverta uma lista simples sem alocar nós, só religando ponteiros, em O(n)
   de tempo e O(1) de memória. Desenhe os três ponteiros a cada passo.
3. **Remover todas as ocorrências.** Escreva uma função **iterativa** com `No **` que remova todas
   as ocorrências de um valor em uma única passada.
4. **Intercalar ordenadas.** Dadas duas listas ordenadas, produza uma terceira, também ordenada,
   reaproveitando os nós (sem `malloc`), em O(n + m).
5. **k-ésimo a partir do fim.** Encontre o k-ésimo nó a partir do fim em uma única passada, usando
   dois ponteiros separados por k nós.
6. **Carrossel com prioridade.** Em `05_lista_circular_carrossel_ihm.c`, faça a tela `ALARM`
   aparecer a cada duas telas comuns, sem duplicar o nó.
7. **Vetor × lista no relógio.** Em `03_vetor_vs_lista.c`, meça com `clock()` o tempo da tabela (b)
   e compare o crescimento com o das contagens. Os números batem? Por quê?
8. **Ponteiros soltos.** Para cada linha a seguir, diga o que dá errado e como o sanitizer acusa:
   `free(p); p = p->prox;`; `inicio = novo; novo->prox = inicio;`;
   `void insere(No *inicio, int v) { ... inicio = novo; }`.

## Tarefa Avaliada 05

> Enunciado resumido para orientação. A versão oficial, com prazo e critérios, está no Google
> Classroom e prevalece sobre este texto.

Considere uma lista simplesmente encadeada de inteiros com nós do tipo
`struct lista { int valor; struct lista *prox; }` e a função recursiva
`int remove_item(struct lista **L, int item)` fornecida no enunciado.

1. **Explique** passo a passo a execução de `remove_item` em uma lista com **quatro nós**: o que é
   `L` e `*L` em cada chamada, quando a recursão para e como a lista fica ao final.
2. Implemente **recursivamente**:
   - `tamanho`: número de nós da lista;
   - `vazia`: indica se a lista está vazia;
   - `conta_item`: quantas vezes um valor aparece;
   - `insere_item_final`: insere um valor no fim da lista.
3. Implemente uma **pilha dinâmica** (nós alocados com `malloc`) com `push` e `pop`.
4. Implemente uma **fila dinâmica** com inserção no fim e remoção do início (FIFO).

**Orientações**

- Antes de programar cada função recursiva, escreva o **caso base** (geralmente a lista vazia) e o
  **passo recursivo** (o que fazer com o primeiro nó e com o resto da lista).
- Na explicação do item 1, desenhe a lista e a pilha de chamadas lado a lado, uma linha por
  chamada. O desenho dos nós com endereços de `02_lista_passo_a_passo.c` ajuda a entender o que um
  `struct lista **` guarda.
- Pense em por que algumas funções recebem `struct lista *` e outras `struct lista **`.
- Teste cada função com lista vazia, com um nó e com vários nós. Libere toda a memória ao final e
  rode com `-fsanitize=address,undefined`.
- Pilha e fila também aparecem em [`../05_pilhas/`](../05_pilhas/) e
  [`../06_filas/`](../06_filas/). Use esse material para revisar o conceito, mas escreva a sua
  própria implementação.

### Exercício de lista duplamente encadeada (Classroom)

O enunciado e o prazo estão no Google Classroom. `04_lista_dupla_alarmes_ihm.c` e a Parte 3 de
`06_lista_sentinela.c` mostram a **ideia** da lista dupla em outro cenário. Não servem de solução
pronta: o exercício pede as suas próprias funções e os seus testes.

## Solução

A solução de referência é [`t05/02_t05_listas.c`](t05/02_t05_listas.c). Tente resolver antes de consultar. Para executar: `./run t05/02_t05_listas.c`.
