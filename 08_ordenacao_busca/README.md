# 08 — Ordenação e Busca

Este módulo mostra como **encontrar** um dado (pesquisa sequencial e binária) e como **colocar dados em ordem** (bolha, seleção, inserção, quicksort e merge sort). Os programas desenham cada passo no terminal e contam comparações e trocas. Assim o aluno vê o algoritmo trabalhando e confere na prática a complexidade estudada em [Big O](../01_revisao_c_basico/complexidade_big_o.md).

## 1. O que a ementa pede e onde está

| Ementa (Unidade 4) | Arquivos |
| --- | --- |
| 4.1 Pesquisa sequencial | `01_pesquisa_sequencial.c`: versão simples e com sentinela; custo médio de (n+1)/2 medido |
| 4.2 Pesquisa binária | `02_pesquisa_binaria.c`: versões iterativa e recursiva, limite inferior e `bsearch` |
| 4.3 Ordenação | `03` (bolha, seleção, inserção), `04` (quicksort), `05` (merge sort), `06` (benchmark) |
| Aplicações | `07_filtro_mediana_sensor.c` (ordenar uma janela de leituras), `08_ranking_alarmes_estavel.c` (estabilidade) |

No Classroom, o tópico **Ordenação** tem como núcleo a **bolha** e o **quicksort** (`quicksort.c`, "Rápida - QuickSort", "Bolha" e Schildt, *C Completo e Total*, capítulo de ordenação). A partição de Hoare em `04_quicksort_visual.c` segue a versão `qs()` desse livro: o pivô é o elemento do meio, e `i` e `j` andam um em direção ao outro.

A versão recursiva mínima da busca binária, criada na revisão de recursão, está em [`../03_revisao_sistematica_c/04_recursao/04_busca_binaria.c`](../03_revisao_sistematica_c/04_recursao/04_busca_binaria.c). Este módulo parte dela e acrescenta a visualização, o custo e as aplicações. Os slides [`../AULA_ARVORES_E_GRAFOS.md`](../AULA_ARVORES_E_GRAFOS.md) (slides 4 e 11) fazem a ponte para a [árvore binária de busca](../09_arvores/), que mantém os dados ordenados e ainda permite inserções em O(log n).

## 2. Conceitos em uma página

**Pesquisa sequencial.** Olha os elementos um a um e funciona em qualquer vetor ou lista, mesmo fora de ordem.

```text
  chave = 25     42   17    8   93   25   61
                  ^    ^    ^    ^    ^ achou na 5ª comparação     média: (n+1)/2
  com sentinela: copia a chave para v[n], e o laço não precisa testar i < n
```

**Pesquisa binária.** Exige vetor **ordenado**. A cada sondagem, descarta metade da janela.

```text
  procura 68:  [ 3  8 12 17 25 31 42 47 53 60 68 74 81 89 95 99]   meio = 47 < 68
                                          [53 60 68 74 81 89 95 99]   meio = 74 > 68
                                          [53 60 68]                  meio = 60 < 68
                                                [68]                  achou: 4 sondagens
  n = 1.000.000  ->  ~20 sondagens (log2 n), contra ~500.000 da sequencial
```

**Ordenação elementar (O(n²)).**

```text
  BOLHA     compara vizinhos e leva o maior para o fim       9 4 12 2 -> 4 9 2 | 12
  SELEÇÃO   procura o menor do resto e põe na frente         2 | 4 12 9
  INSERÇÃO  insere cada novo item na parte já ordenada       2 4 9 | 12   (como cartas na mão)
```

**Dividir para conquistar (O(n log n)).**

```text
  QUICKSORT: particiona em torno de um pivô e ordena cada lado
     [38 27 43 3 9 82 10 <25>]  ->  [3 9 10] <25> [27 82 43 38]
  MERGE SORT: divide ao meio até sobrar 1 e depois intercala as metades ordenadas
     [27 38] + [3 43]  ->  3 27 38 43
```

**Estabilidade.** Um método é estável quando elementos com a mesma chave mantêm a ordem original entre si. Isso importa quando se ordena por um segundo critério, como alarmes por prioridade que precisam continuar em ordem de chegada.

## 3. Ordem de estudo recomendada

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `01_pesquisa_sequencial.c` | Busca passo a passo (`^i`), sentinela e custo médio medido para n = 10…10000; tags Modbus | O(n), média (n+1)/2 |
| `02_pesquisa_binaria.c` | Janela `I M F` encolhendo, versão recursiva indentada, tabela sequencial × binária até 10⁶, estoque de peças, interpolação de calibração ADC→°C | O(log n) |
| `03_ordenacao_elementar.c` | Bolha (com parada antecipada), seleção e inserção em barras; melhor e pior caso medidos; demonstração de estabilidade | O(n²); O(n) no melhor caso de bolha e inserção |
| `04_quicksort_visual.c` | Partições de Lomuto e Hoare (Schildt) com `<pivô>`, `i` e `j`; recursão indentada; pior caso com vetor ordenado; mediana-de-três | O(n log n) em média, O(n²) no pior caso |
| `05_merge_sort_visual.c` | Árvore de divisão, cada escolha da intercalação, junção de dois logs de CLP; contagens em várias entradas | O(n log n) sempre, O(n) de memória extra |
| `06_benchmark_ordenacao.c` | Todos os algoritmos e o `qsort` lado a lado, com n = 100/1000/10000 e entradas aleatória, ordenada e invertida; tempo via `clock()` | Comparação empírica |
| `07_filtro_mediana_sensor.c` | Janela de K leituras ordenada por inserção a cada amostra; mediana × média móvel com picos de EMI | O(N·K²), com K pequeno |
| `08_ranking_alarmes_estavel.c` | Tela de alarmes por prioridade: inserção (estável) × seleção (instável) × `qsort` com chave composta | Estabilidade |

## 4. Tabela comparativa de complexidade

### Busca

| Método | Melhor | Médio | Pior | Espaço | Pré-requisito |
| --- | --- | --- | --- | --- | --- |
| Sequencial simples | O(1) | O(n) ≈ n/2 | O(n) | O(1) | nenhum |
| Sequencial com sentinela | O(1) | O(n) ≈ n/2 (metade dos testes) | O(n) | O(1) + 1 posição | posição livre em `v[n]` |
| Binária iterativa | O(1) | O(log n) | O(log n) | O(1) | vetor ordenado |
| Binária recursiva | O(1) | O(log n) | O(log n) | O(log n) de pilha | vetor ordenado |

### Ordenação

| Algoritmo | Melhor | Médio | Pior | Espaço extra | Estável | In-place |
| --- | --- | --- | --- | --- | --- | --- |
| Bolha (com parada antecipada) | O(n) | O(n²) | O(n²) | O(1) | sim | sim |
| Seleção | O(n²) | O(n²) | O(n²) | O(1) | **não** | sim |
| Inserção | O(n) | O(n²) | O(n²) | O(1) | sim | sim |
| Merge sort | O(n log n) | O(n log n) | O(n log n) | O(n) | sim | **não** |
| Quicksort (pivô = último) | O(n log n) | O(n log n) | O(n²) com vetor ordenado | O(log n) em média, O(n) no pior caso | **não** | sim |
| Quicksort (mediana-de-três / meio) | O(n log n) | O(n log n) | O(n²) só com entradas construídas para isso | O(log n) | **não** | sim |
| `qsort` (stdlib) | depende da implementação | O(n log n) na prática | depende da implementação | depende da implementação | não garantido | sim |

Leitura rápida:

- Para **n pequeno** (até ~20) ou **dados quase ordenados**, use inserção.
- Para **uso geral**, use quicksort ou `qsort`.
- Para **estabilidade ou pior caso garantido**, use merge sort.
- Para **minimizar escritas**, use seleção, que faz no máximo n − 1 trocas.

## 5. Exemplos reais

- **Automação:**
  - Tabela de tags Modbus de um gateway, cadastrada fora de ordem (sequencial).
  - Tabela de linearização de termopar/NTC no firmware (binária e interpolação).
  - Estoque de peças de manutenção por código (binária ou `bsearch`).
  - Filtro de mediana contra picos de EMI na entrada analógica do CLP.
  - Tela de alarmes ISA-18.2 ordenada por prioridade e horário (estabilidade).
  - União de logs de vários CLPs por timestamp (intercalação).
  - Percentis do histórico do SCADA (quicksort).
- **Cotidiano:**
  - Achar um nome numa lista de presença (sequencial).
  - Consultar um dicionário ou lista telefônica (binária).
  - Organizar cartas na mão (inserção).
  - Ordenar uma planilha por data e depois por cliente (estabilidade).

## 6. Como executar

Execute a partir da raiz do repositório. O script `run` compila para `build/` e executa:

```sh
./run list ordenacao                         # lista os arquivos deste módulo
./run 01_pesquisa_sequencial                 # ENTER avança cada passo
./run 04_quicksort_visual
./run 06_benchmark_ordenacao < /dev/null     # sem pausas, direto até o fim
```

Quando a entrada não é um terminal (`< /dev/null`, pipe), os programas não pausam. `01`, `02` e `03` também aceitam entrada do aluno: tags, códigos de peça ou um vetor próprio. Com ENTER vazio ou Ctrl+D, eles usam o roteiro padrão.

## 7. Exercícios propostos

A tarefa **T06 — Exercitando bolha e quicksort** está no Classroom. O enunciado e a entrega ficam lá. Os exercícios abaixo servem de prática livre.

1. Na pesquisa sequencial, implemente a heurística **mover para a frente**: ao encontrar um elemento, troque-o com o anterior. Meça as comparações quando 20% das tags recebem 80% das consultas.
2. Escreva `limite_superior` (primeiro índice com `v[i] > x`) e use-o junto com `limite_inferior` para contar, em O(log n), quantas leituras iguais a `x` existem num vetor ordenado.
3. Modifique a bolha para alternar o sentido a cada passada (*cocktail shaker sort*). Compare o número de passadas com o da bolha comum no vetor `{2, 3, 4, 5, 6, 1}`.
4. Na inserção, troque a busca linear do ponto de inserção por uma busca binária. O número de **comparações** cai, mas o de **deslocamentos** muda? Explique com as contagens.
5. Faça o quicksort chamar a inserção quando o trecho tiver menos de 10 elementos. Meça o ganho no benchmark para n = 10000.
6. Construa uma entrada em que o quicksort com pivô = último elemento faça exatamente n(n−1)/2 comparações e que **não** seja nem ordenada nem invertida.
7. Implemente o merge sort **de baixo para cima** (iterativo): intercale blocos de tamanho 1, 2, 4, … e confirme que o número de comparações bate com o da versão recursiva.
8. Altere o filtro de mediana para K = 7. Quantos picos consecutivos ele passa a remover? Quantas amostras de atraso o degrau sofre agora?
9. Ordene os alarmes por **tag** (ordem alfabética, com `strcmp`) e depois, de forma estável, por prioridade. Mostre que dentro de cada prioridade as tags ficam em ordem alfabética.

## Referências

- SCHILDT, H. *C Completo e Total*, capítulo de ordenação e busca (bolha, seleção, inserção, Shell sort, quicksort, busca sequencial e binária).
- Introdução a Big O deste repositório: [`../01_revisao_c_basico/complexidade_big_o.md`](../01_revisao_c_basico/complexidade_big_o.md).
- Próximo módulo: [`../09_arvores/`](../09_arvores/). Uma BST balanceada busca, insere e lista em ordem, combinando as vantagens do vetor ordenado e da lista encadeada.
