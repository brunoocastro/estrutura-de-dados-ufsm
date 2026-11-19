# Árvores em C

Pilhas, filas e listas organizam elementos em **sequência**: cada elemento tem no máximo um sucessor. Uma **árvore** organiza elementos em **hierarquia**: cada elemento pode ter vários sucessores (filhos), mas um único antecessor (pai). Exemplos do dia a dia: as pastas de um sistema de arquivos, o organograma de uma empresa, o sumário de um livro e a árvore de ativos de uma planta industrial.

## O que a ementa pede e onde está

| Ementa / Classroom | Onde estudar nesta pasta |
| --- | --- |
| **Unidade 5.1 — Árvores binárias**: conceito, terminologia, representação | Seção [Conceitos](#conceitos), `01_arvore.c`, `03_arvore_binaria.c`, `09_formas_arvore.c` |
| Árvore binária de busca: inserção, busca, remoção | `03_arvore_binaria.c` (referência), `04_bst_insercao_busca.c` e `05_bst_remocao.c` (passo a passo) |
| Percursos pré, em, pós-ordem e por nível | `06_percursos_arvore.c` (pilha e fila desenhadas), `02_automacao_scada.c` (aplicações) |
| Classroom: **Árvores AVL e Balanceamento** | `07_bst_degenerada_vs_balanceada.c` (o problema) e `08_avl_rotacoes.c` (a solução) |
| Classroom: exemplo de implementação de árvores binárias em C | `03_arvore_binaria.c`, `09_formas_arvore.c` |
| Árvore em vetor e heap (complemento: fila de prioridade) | `09_formas_arvore.c`, `10_heap_alarmes.c` |
| Classroom: exercícios teóricos | [Exercícios propostos](#exercícios-propostos) |
| Classroom: Tarefa Avaliada 06 | [Enunciado e orientações](#tarefa-avaliada-06) |

A teoria geral de Big O fica em [`../01_revisao_c_basico/complexidade_big_o.md`](../01_revisao_c_basico/complexidade_big_o.md). Árvores usam muita recursão ([`../02_revisao_funcoes_recursao/`](../02_revisao_funcoes_recursao/)), filas no percurso por nível ([`../06_filas/`](../06_filas/)) e listas encadeadas na árvore genérica ([`../07_listas/`](../07_listas/)).

## Conceitos

```text
                 A            <- raiz (nível 0)
              /  |  \
             B   C   D        <- nível 1
            / \      |
           E   F     G        <- nível 2
                     |
                     H        <- nível 3 (folha)
```

| Termo | Definição | No exemplo |
| --- | --- | --- |
| **Nó** | Elemento da árvore; guarda um valor e as ligações para os filhos. | A, B, …, H |
| **Aresta** | Ligação entre um pai e um filho. Uma árvore com `n` nós tem `n - 1` arestas. | A–B, B–E, … |
| **Raiz** | Único nó sem pai; ponto de entrada da árvore. | A |
| **Pai / filho** | Se existe aresta de X para Y, X é pai de Y e Y é filho de X. | B é pai de E e F |
| **Irmãos** | Nós com o mesmo pai. | B, C e D |
| **Folha** (nó externo) | Nó sem filhos. | E, F, C, H |
| **Nó interno** | Nó com pelo menos um filho. | A, B, D, G |
| **Ancestral / descendente** | Qualquer nó no caminho da raiz até X é ancestral de X; X é descendente dele. | A e D são ancestrais de H |
| **Subárvore** | Um nó junto com todos os seus descendentes. Toda subárvore também é uma árvore. | D, G, H |
| **Grau de um nó** | Número de filhos do nó. | grau(A) = 3, grau(B) = 2 |
| **Nível / profundidade** | Número de arestas da raiz até o nó. A raiz está no nível 0. | nível(G) = 2 |
| **Altura** | Arestas no caminho mais longo do nó até uma folha. Árvore vazia: -1; um só nó: 0. | altura(A) = 3 |

Existe **um único caminho** entre a raiz e qualquer nó, por isso não há ciclos. A árvore é **recursiva**: ou é vazia, ou é uma raiz cujos filhos são raízes de subárvores. Por isso quase toda operação (contar, buscar, medir a altura, liberar) sai naturalmente com recursão.

### Árvore genérica: primeiro filho / próximo irmão

Na árvore genérica cada nó pode ter **qualquer número** de filhos. Em vez de um vetor de ponteiros por nó, `01_arvore.c` e `02_automacao_scada.c` guardam os filhos como uma **lista encadeada**: o pai aponta para o primeiro filho, e cada filho aponta para o irmão seguinte.

```text
A
|
B ---> C ---> D        (| = primeiro_filho, ---> = proximo_irmao)
|             |
E ---> F      G
              |
              H
```

### Árvore binária

Cada nó tem **no máximo dois filhos**, e a posição importa: **esquerdo** e **direito**. As duas árvores abaixo são iguais como árvores genéricas, mas diferentes como árvores binárias:

```text
   A          A
  /            \
 B              B
```

| Aspecto | Árvore genérica | Árvore binária |
| --- | --- | --- |
| Número de filhos | Qualquer (0, 1, 2, …) | No máximo 2 |
| Posição dos filhos | Sequência, sem "lado" | Cada filho é esquerdo ou direito |
| Representação típica | Lista de filhos; primeiro filho / próximo irmão | Dois ponteiros fixos `esq`/`dir`, ou vetor (`2i+1`, `2i+2`) |
| Percursos | Pré-ordem, pós-ordem, por nível | Os mesmos, mais a **em-ordem** |
| Uso típico | Diretórios, menus de IHM, ativos ISA-95, árvore DOM | BST, AVL, heap, árvores de expressão, Huffman |

Formas especiais: **cheia** (todo nó tem 0 ou 2 filhos), **completa** (todos os níveis cheios, menos talvez o último, preenchido da esquerda para a direita, que é a forma da heap) e **perfeita** (todas as folhas no mesmo nível; com altura `h` tem `2^(h+1) - 1` nós).

### Árvore binária de busca (BST)

Para **todo** nó: a subárvore esquerda só tem chaves menores e a direita só tem chaves maiores. Cada comparação descarta uma subárvore inteira, como na pesquisa binária ([`../08_ordenacao_busca/`](../08_ordenacao_busca/)).

```text
            50                  buscar 60:  60 > 50 -> direita
          /    \                            60 < 70 -> esquerda
        30      70                          60 == 60 -> achou (3 comparações)
       /  \    /  \
      20  40  60  80            em-ordem: 20 30 40 50 60 70 80 (ordenado!)
```

Remoção em três casos: **folha** (o pai passa a apontar para `NULL`), **um filho** (o filho sobe) e **dois filhos** (copia-se o **sucessor**, o menor da subárvore direita, e remove-se o nó original dele).

O custo de tudo é **O(h)**. Se as chaves chegam em ordem (timestamps, IDs sequenciais), a BST **degenera** numa lista e `h = n - 1`:

```text
 10                    com 100000 chaves em ordem:  altura 99999
   \                   embaralhadas:                altura ~39
    20                 balanceada:                  altura 16
      \
       30 ...
```

### AVL: BST que se mantém balanceada

Cada nó guarda sua altura e o **fator de balanceamento** `fb = altura(esq) - altura(dir)`. A regra da AVL é `fb ∈ {-1, 0, +1}` em todos os nós. Se uma inserção produz `±2`, uma **rotação** corrige a altura em O(1):

```text
 caso LL (rotação à direita)        caso LR (esquerda no filho, depois direita)
       z(+2)          y                 z(+2)        z            x
      /              / \               /            /            / \
     y(+1)   ==>    x   z             y(-1)  ==>   x     ==>    y   z
    /                                  \          /
   x                                    x        y
```

RR e RL são os espelhos. A altura fica abaixo de `1,44·log2(n + 2)`, então busca, inserção e remoção são **O(log n) no pior caso**.

### Heap binária

A heap é uma árvore binária **completa** guardada num **vetor**, sem ponteiros: o nó `i` tem filhos em `2i+1` e `2i+2` e pai em `(i-1)/2`. Na max-heap todo pai é maior ou igual aos filhos, então o máximo está sempre em `v[0]`. Ela não é ordenada como uma BST, mas isso basta para uma **fila de prioridade**.

```text
 vetor: [950][800][600][700][400][200][500]        árvore:        950
 índice:  0    1    2    3    4    5    6                       /     \
                                                              800     600
                                                             /  \    /  \
                                                           700 400  200 500
```

## Ordem de estudo recomendada

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `01_arvore.c` | Árvore genérica (primeiro filho / próximo irmão): contar nós e folhas, altura, grau, busca, liberação | Busca O(n): não há regra de ordem |
| `02_automacao_scada.c` | Árvore de ativos ISA-95/OPC-UA: pré-ordem para exibir, pós-ordem para somar potência, desligar em cascata e liberar | Percursos O(n); armadilha O(n·h) apontada |
| `03_arvore_binaria.c` | Implementação de referência da BST: inserir, buscar, remover (3 casos), 4 percursos, contar, altura | O(h) por operação |
| `04_bst_insercao_busca.c` | **Passo a passo**: cada inserção e busca com comparações narradas e caminho destacado; BST x busca sequencial; menu livre | O(log n) médio, O(n) pior |
| `05_bst_remocao.c` | **Passo a passo**: remoção de folha, de nó com um filho e de nó com dois filhos (sucessor), antes e depois, com verificação da ordem | O(h) |
| `06_percursos_arvore.c` | **Passo a passo**: pré, em e pós-ordem com a pilha de chamadas, por nível com a fila; aplicações (relatório ordenado, backup em pré-ordem, `free` em pós-ordem) | O(n) tempo; O(h) pilha ou O(largura) fila |
| `07_bst_degenerada_vs_balanceada.c` | Timestamps em ordem degeneram a BST; tabela de altura e comparações de n = 10 a 100000 | O(n) contra O(log n) |
| `08_avl_rotacoes.c` | AVL com `fb` em cada nó; rotações LL, RR, LR e RL desenhadas antes e depois; índice de tags SCADA; tabela BST x AVL | O(log n) garantido |
| `09_formas_arvore.c` | Árvore em vetor, min-heap, BST iterativa com `**`, ordem crescente/decrescente, dissolver um nó de árvore genérica | Heap O(log n); inserção iterativa O(h) |
| `10_heap_alarmes.c` | **Passo a passo**: max-heap como fila de prioridade de alarmes, com vetor e árvore lado a lado; heap x vetores | O(log n) contra O(n) |

## Complexidade comparada

`n` = número de elementos, `h` = altura. Médio da BST = chaves em ordem aleatória.

| Operação | Vetor desordenado | Vetor ordenado | Lista encadeada | BST (médio) | BST (pior) | AVL | Heap binária |
| --- | --- | --- | --- | --- | --- | --- | --- |
| Buscar uma chave | O(n) | O(log n) | O(n) | O(log n) | O(n) | O(log n) | O(n) |
| Inserir | O(1) | O(n) | O(1)* | O(log n) | O(n) | O(log n) | O(log n) |
| Remover uma chave | O(n) | O(n) | O(n) | O(log n) | O(n) | O(log n) | O(n)** |
| Mínimo / máximo | O(n) | O(1) | O(n) | O(log n) | O(n) | O(log n) | O(1) (um deles) |
| Remover o máximo | O(n) | O(1) | O(n) | O(log n) | O(n) | O(log n) | O(log n) |
| Listar em ordem | O(n log n) | O(n) | O(n log n) | O(n) | O(n) | O(n) | O(n log n) |
| Altura | — | — | — | ≈ 2,99·log2 n | n - 1 | < 1,44·log2 n | ⌊log2 n⌋ |

\* inserção no início. \*\* a heap só sabe remover a raiz em O(log n); achar uma chave qualquer exige varrer o vetor.

Percursos (pré, em, pós-ordem e por nível) custam **O(n)** em qualquer árvore. A recursão usa O(h) de pilha, que vira O(n) numa árvore degenerada; o percurso por nível usa uma fila do tamanho do nível mais largo.

## Exemplos reais

**Engenharia de controle e automação**

- Hierarquia de ativos ISA-95 / espaço de endereçamento OPC-UA (Planta → Área → CLP → E/S): árvore genérica (`02_automacao_scada.c`).
- Desligamento seguro em cascata: pós-ordem, os atuadores antes do controlador que os comanda.
- Índice de tags de um SCADA ou de pontos Modbus por ID: BST/AVL (`08_avl_rotacoes.c`).
- Historiador de processo indexado por timestamp: o caso clássico de BST degenerada (`07_bst_degenerada_vs_balanceada.c`).
- Gerenciamento de alarmes ISA-18.2 e fila de prontos de um RTOS: heap (`10_heap_alarmes.c`).
- Menus hierárquicos de IHM, árvores de falhas (FTA) e árvores de decisão de diagnóstico.

**Cotidiano e computação**

- Sistemas de arquivos, DOM de páginas web, organogramas.
- `std::map` (C++), `TreeMap` (Java), índices de banco de dados (árvores B, primas da BST).
- Compressão de Huffman (ZIP, JPEG) e árvores de expressão em compiladores.

## Como executar

Na raiz do repositório, use o script `run`, que compila para `build/` e executa:

```sh
./run 04_bst_insercao_busca                # ENTER avança cada passo; no fim há um menu
./run 08_avl_rotacoes                      # rotações da AVL desenhadas
./run 07_bst_degenerada_vs_balanceada < /dev/null   # tudo de uma vez, sem pausas
./run list arvores                         # lista os arquivos desta pasta
```

Os arquivos passo a passo pausam a cada etapa quando executados num terminal. Com a entrada redirecionada (`< /dev/null`, `| less`), rodam direto até o fim.

## Exercícios propostos

Sem solução publicada: use os programas da pasta para conferir as suas respostas.

1. Insira as chaves `45, 20, 70, 10, 30, 60, 90, 25, 35` numa BST vazia. Desenhe a árvore e escreva os percursos pré, em, pós-ordem e por nível. Confira com o menu de `04_bst_insercao_busca`.
2. Na árvore do exercício 1, remova `20` e depois `45`, usando o sucessor. Desenhe a árvore após cada remoção. Repita usando o **predecessor** e compare.
3. Qual é a altura mínima e qual é a altura máxima de uma árvore binária com 31 nós? E com 1000 nós?
4. Dê uma ordem de inserção das chaves 1 a 7 que produza uma BST perfeita, e outra que produza altura 6.
5. A pré-ordem de uma BST é `8 3 1 6 4 7 10 14 13`. Reconstrua a árvore. Por que a em-ordem sozinha não basta?
6. Insira `10, 20, 30, 25, 28, 27` numa AVL vazia. Indique, a cada passo, os fatores de balanceamento e as rotações (LL, RR, LR, RL) aplicadas.
7. Escreva `int eh_avl(const No *r)`, que verifica se uma árvore binária é uma BST **e** se todos os fatores de balanceamento estão em {-1, 0, +1}, em O(n).
8. Escreva o percurso em-ordem **sem recursão**, usando uma pilha explícita ([`../05_pilhas/`](../05_pilhas/)).
9. Em `02_automacao_scada.c`, a impressão custa O(n·h) porque recalcula a potência de cada subárvore. Reescreva para O(n) com uma única pós-ordem.
10. Escreva `int k_esimo(const No *r, int k)`, que devolve a k-ésima menor chave de uma BST. Qual o custo? Que campo extra no nó o deixaria em O(h)?
11. Transforme o vetor `[3, 9, 2, 1, 4, 5]` numa max-heap e mostre o vetor após cada troca. Depois remova o máximo duas vezes.
12. Por que a heap é guardada em vetor e a BST normalmente com ponteiros? O que aconteceria com uma BST degenerada guardada em vetor (`2i+1`, `2i+2`)?

## Tarefa Avaliada 06

Enunciado e orientações. A solução deve ser feita e entregue individualmente, conforme o Classroom.

### Questão 1: função `delete`

Escreva uma função `delete` que, dada uma **árvore binária** e um **valor**, remova da árvore o **ramo** (a subárvore) cuja raiz contém esse valor.

Orientações:

- Leia o enunciado no Classroom com atenção: remover um **ramo** é diferente de remover um único nó, como em `05_bst_remocao.c`.
- Defina e documente o que a função devolve e o que acontece quando o valor não existe, está na raiz ou a árvore está vazia.
- Toda a memória do ramo removido deve ser liberada, e o restante da árvore deve continuar válido.
- Inclua um `main` que monte uma árvore, imprima-a antes e depois de cada chamada e teste os casos acima.

### Questão 2: árvore genealógica

Escreva um programa de **árvore genealógica** em que cada pai ou mãe pode ter um **número arbitrário de filhos(as)**, implementado com uma **lista de filhos(as)** em cada pessoa. O programa deve permitir a **entrada de dados** e, na **consulta**, listar **todos os ascendentes** de cada pessoa.

Orientações:

- Revise as listas encadeadas ([`../07_listas/`](../07_listas/)) e a árvore genérica de `01_arvore.c`.
- Planeje, antes de codificar, quais ligações cada pessoa precisa guardar para que tanto a entrada de dados quanto a consulta sejam possíveis.
- Valide a entrada (nomes repetidos, relações inválidas) e libere toda a memória ao final.

### Questão 3

A terceira questão (mapa rodoviário) é um problema de grafos e é estudada no módulo [`../10_grafos/`](../10_grafos/).

### Critérios gerais

- Compilar sem avisos com `-std=c11 -Wall -Wextra -pedantic`.
- Comentários explicando as decisões e o custo (Big O) de cada função.
- Nenhum vazamento de memória (teste com `-fsanitize=address`).
