# 05 — Pilhas em C

Ementa da disciplina UFSM00064, **Unidade 2 — Pilhas**:

| Item da ementa | Onde está nesta pasta |
| --- | --- |
| 2.1 Conceito de pilha (LIFO, push/pop/peek, overflow/underflow, aplicações) | `00`, `01`, `05`, `07`–`16` |
| 2.2 Implementação de pilhas (vetor estático, lista encadeada, vetor que dobra) | `01`–`04`, `06` |

Material do Google Classroom e como a pasta o complementa:

- **Material didático, vídeo e exemplo em C**: os arquivos `00` a `05` seguem a mesma sequência
  (pilha mínima → TAD → vetor → encadeada → strings → aplicação).
- **L03** (lista não avaliada sobre pilhas, vetores e matrizes) e **T03** (tarefa avaliada sobre
  manipulação de pilhas): os enunciados estão no Classroom. Este repositório não publica soluções
  de tarefas avaliadas; os exemplos daqui usam outros cenários.
- **Pesquisa e implementação "Notação Polonesa Reversa (RPN) e Avaliação de Expressões com
  Pilhas"**: veja a seção [Material de apoio para a pesquisa RPN](#material-de-apoio-para-a-pesquisa-rpn).
  Os arquivos `12` e `13` mostram os algoritmos executando, mas **não** são a resposta da pesquisa.

## Conceito

Uma pilha é uma sequência em que **só o topo** é acessível. O último elemento que entra é o
primeiro que sai: **LIFO** (*Last In, First Out*).

```text
   push(30)          pop() -> 30         peek() -> 20
   +------+          +------+            +------+
   |  30  | <- topo  |      |            |      |
   |  20  |          |  20  | <- topo    |  20  | <- topo (não sai)
   |  10  |          |  10  |            |  10  |
   +------+          +------+            +------+
```

| Operação | O que faz | Erro possível |
| --- | --- | --- |
| `push(x)` | coloca `x` no topo | **overflow**: vetor cheio, ou `malloc` devolve `NULL` |
| `pop()` | remove e devolve o topo | **underflow**: pilha vazia |
| `peek()` / `topo()` | devolve o topo sem remover | pilha vazia |
| `vazia()` / `cheia()` | consultas | — |

Há duas implementações clássicas, e as duas fazem push e pop em O(1):

```text
 VETOR (estática)                         ENCADEADA (dinâmica)
      +------+
  [3] |      |  <- posição livre            topo
  [2] |  30  |  <- topo (n = 3)              |
  [1] |  20  |                               v
  [0] |  10  |                             [30|*]-->[20|*]-->[10|/]--> NULL
      +------+
 push: dados[n++] = x                     push: novo->prox = topo; topo = novo
 pop : x = dados[--n]                     pop : x = topo->valor; topo = topo->prox; free
```

- **Vetor**: memória contígua e previsível (bom para firmware de CLP e microcontroladores), mas
  com capacidade fixa. Para não ter limite, o vetor pode **dobrar** quando enche (custo O(1)
  *amortizado*).
- **Encadeada**: sem limite fixo, só aloca o que usa, mas cada elemento carrega um ponteiro e
  custa um `malloc`. O topo fica no **início** da lista; no fim, o pop seria O(n).
- **Convenção do topo**: `topo = -1` (índice do último) ou `topo = 0` (quantidade). As duas
  servem; misturá-las no mesmo código causa erro de *off-by-one* (`02` e `01` usam uma cada).

Introdução à notação Big O: [`../01_revisao_c_basico/complexidade_big_o.md`](../01_revisao_c_basico/complexidade_big_o.md).

## Ordem de estudo recomendada

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `00_pilha_basica.c` | Pilha mínima: vetor global, sem `struct` nem ponteiros; `coloca`/`retira` | push/pop O(1) |
| `01_pilha_visao_geral.c` | TAD com `struct`, `push`/`pop`/`peek`, endereços contíguos | push/pop/peek O(1) |
| `02_pilha_array.c` | Vetor de capacidade 3 com `topo = -1`: overflow e underflow | O(1); memória O(C) fixa |
| `03_pilha_dinamica.c` | Encadeada com `malloc`/`free`, topo no início da lista | O(1); memória O(n) |
| `04_pilha_strings.c` | Pilha de strings `char[CAP][TAM]`, truncamento seguro com `strncpy` | O(L) por cópia de string |
| `05_historico_navegador.c` | Aplicação: botão "voltar" | O(1) por página |
| `06_pilha_vetor_vs_encadeada.c` | **Passo a passo**: vetor e encadeada lado a lado; tabela n = 10…10000 com vetor que dobra e versão ingênua O(n²) | O(1) × O(1) amortizado × O(n) |
| `07_valida_chaves.c` | Validadores, degrau 1: só `{ }`; mostra que um contador basta | O(n) |
| `08_valida_colchetes.c` | Validadores, degrau 2: só `[ ]`, com `coloca`/`retira` (exemplo de aula) | O(n) |
| `09_valida_delimitadores.c` | Validadores, degrau 3: `( ) [ ] { }`; mostra onde os contadores falham | O(n) tempo, O(n) pilha |
| `10_delimitadores_passo_a_passo.c` | Validadores, degrau 4: **passo a passo**, pilha vertical, `^` no caractere, sem globais | O(n); push + pop ≤ n |
| `11_valida_codigo_c.c` | Validadores, degrau 5: código C real, com strings, caracteres, comentários, linha e coluna | O(n) |
| `12_rpn_passo_a_passo.c` | **Passo a passo**: avaliação de RPN (estilo HP, 4–20 mA) e lista de instruções de CLP (LD/AND/ORB) | O(n) |
| `13_infixa_posfixa_shunting_yard.c` | **Passo a passo**: infixa → pós-fixa (Dijkstra), em tabela token/ação/pilha/saída | O(n) amortizado |
| `14_desfazer_refazer_ihm.c` | **Passo a passo**: desfazer/refazer de IHM com duas pilhas; histórico limitado | O(1) por ação |
| `15_agv_backtracking.c` | **Passo a passo**: AGV numa grade, backtracking com pilha explícita; mapa redesenhado | O(V) |
| `16_pilha_de_chamadas.c` | **Passo a passo**: quadros de `fatorial(4)`, a versão com pilha explícita e o tamanho de cada quadro | O(n) tempo e espaço |

### Progressão dos validadores de delimitadores (07 → 11)

Cinco arquivos tratam do mesmo problema. Eles formam uma escada, e cada degrau acrescenta uma ideia:

1. **`07` — só chaves.** Com um único tipo de delimitador, todos os elementos da pilha são `{` e
   só a altura importa. O programa compara a pilha com um contador e mostra que dão o mesmo
   resultado.
2. **`08` — só colchetes, como TAD.** As operações ficam em `coloca`/`retira`, que tratam
   overflow e underflow e indicam o sucesso pelo retorno. As mensagens distinguem "fechou sem
   abrir" de "abriu sem fechar".
3. **`09` — três tipos.** Ao fechar, é preciso saber **qual** foi o último aberto. Em `"{[a}]"`,
   três contadores dizem "correto" e a pilha diz "erro". É aqui que a pilha se torna
   indispensável.
4. **`10` — visual e reentrante.** Mesmo algoritmo, com a pilha desenhada a cada passo e
   variáveis locais em vez de globais. Os contadores de push e pop confirmam o custo O(n).
5. **`11` — código de verdade.** Ignora delimitadores dentro de `"..."`, `'('` e comentários
   (uma máquina de estados) e guarda linha e coluna de cada abertura para dar mensagens de
   compilador.

## Complexidade por implementação

| Operação | Vetor fixo | Vetor que dobra | Encadeada | "Ingênua": topo em `v[0]` |
| --- | --- | --- | --- | --- |
| `push` | O(1) (falha se cheio) | O(1) amortizado, O(n) no pior caso | O(1) + `malloc` | O(n): desloca tudo |
| `pop` | O(1) | O(1) | O(1) + `free` | O(n): desloca tudo |
| `peek`, `vazia` | O(1) | O(1) | O(1) | O(1) |
| buscar um valor qualquer | O(n) | O(n) | O(n) | O(n) |
| memória para n elementos | C posições, sempre | entre n e 2n posições | n × (valor + ponteiro) | C posições |

Contagens reais (de `06_pilha_vetor_vs_encadeada.c`, n push seguidos de n pop):

| n | vetor fixo | vetor que dobra (cópias) | encadeada | topo em `v[0]` |
| ---: | ---: | ---: | ---: | ---: |
| 10 | 20 | 35 (15) | 20 | 110 |
| 100 | 200 | 327 (127) | 200 | 10 100 |
| 1 000 | 2 000 | 3 023 (1 023) | 2 000 | 1 001 000 |
| 10 000 | 20 000 | 36 383 (16 383) | 20 000 | 100 010 000 |

Aplicações (n = tamanho da entrada): validar delimitadores, avaliar RPN e fazer o shunting-yard
custam O(n) de tempo e O(n) de pilha no pior caso. A busca em profundidade numa grade com V casas
livres custa O(V).

## Exemplos reais

**Engenharia de controle e automação**

- **CLP**: listas de instruções usam uma pilha lógica (`LD` abre bloco, `ORB`/`ANB` combinam os
  dois do topo; Mitsubishi tem também `MPS`/`MRD`/`MPP`). O compilador de Ladder gera essa forma
  pós-fixa (`12`).
- **IHM e supervisório**: desfazer/refazer na edição de receitas e parâmetros (`14`); botão
  "Voltar" entre telas (`05`); validação de fórmulas de tags calculadas (`10`, `13`).
- **AGVs e robôs móveis**: exploração e backtracking de rotas (`15`).
- **Firmware embarcado**: a pilha de chamadas tem poucos KB. Por isso a MISRA C proíbe recursão
  e o código usa pilha explícita de tamanho conhecido (`16`).
- **Instrumentação**: conversão 4–20 mA → unidade de engenharia avaliada em RPN (`12`).

**Cotidiano e computação**

- Ctrl+Z / Ctrl+Y, voltar/avançar do navegador, pilha de pratos ou bandejas.
- Compiladores (parênteses, expressões, pilha de chamadas), calculadoras HP, máquinas virtuais de
  pilha (Java bytecode, WebAssembly, PostScript).

## Como executar

Da raiz do repositório (o script compila para `build/`):

```bash
./run 06_pilha_vetor_vs_encadeada          # ENTER avança cada passo
./run 06_pilha_vetor_vs_encadeada < /dev/null   # roda direto, sem pausas
./run 13_infixa_posfixa_shunting_yard
./run list pilhas                          # lista os arquivos desta pasta
./run build 05_pilhas/15_agv_backtracking.c
```

Os programas "passo a passo" (`06`, `10`, `12`–`16`) só pausam quando a entrada é um terminal. Com
`< /dev/null` (ou redirecionando a saída para um arquivo) eles rodam de uma vez. O `14` abre um
modo livre (comandos `a`, `u`, `r`, `q`) depois do roteiro quando é executado num terminal.

Compilação manual com as flags usadas na verificação:

```bash
cc -std=c11 -Wall -Wextra -pedantic -g -fsanitize=address,undefined \
   05_pilhas/10_delimitadores_passo_a_passo.c -o /tmp/x -lm && /tmp/x
```

## Material de apoio para a pesquisa RPN

O enunciado da pesquisa está no Classroom. O que segue é material de estudo, não a resposta.

**Conceitos para pesquisar e explicar com suas palavras**

- Notação infixa, prefixa (polonesa, Jan Łukasiewicz, década de 1920) e pós-fixa (polonesa
  reversa). Por que a pós-fixa dispensa parênteses e regras de precedência?
- Por que a **pilha** é a estrutura natural para avaliar pós-fixa? O que fica guardado nela a cada
  passo?
- A ordem dos operandos em operações não comutativas (`-`, `/`, `^`): quem é o topo?
- Como detectar uma expressão inválida (operandos de menos, operandos sobrando, token
  desconhecido, divisão por zero)?
- O algoritmo *shunting-yard* (Dijkstra, 1961): precedência, associatividade (por que `^` é
  diferente de `-`?) e tratamento de parênteses.
- Complexidade de tempo e de espaço da avaliação e da conversão. Por que o laço "enquanto"
  dentro do shunting-yard não torna o algoritmo O(n²)?
- Onde a RPN aparece: calculadoras HP (HP 9100A de 1968, HP-35 de 1972), linguagens Forth e
  PostScript, máquinas virtuais de pilha, listas de instruções de CLP.

**Experimentos com os arquivos desta pasta**

1. Rode `./run 12_rpn_passo_a_passo` e, antes de cada ENTER, tente prever a próxima pilha.
2. Rode `./run 13_infixa_posfixa_shunting_yard` e confira cada linha da tabela com as regras do
   cabeçalho. Converta à mão `a - b - c` e `a ^ b ^ c` e compare.
3. Pegue a saída pós-fixa do `13` e avalie no papel com o método do `12`.
4. Observe nos dois programas os contadores de push e pop: como eles crescem com o número de
   tokens?

**O que os exemplos não fazem (fica para a sua implementação)**

Os tokens do `12` já chegam separados dentro do código, não há leitura de expressões digitadas
pelo usuário, e os dois programas trabalham isolados. Projetar a entrada, a separação em tokens,
as mensagens de erro e os testes faz parte da atividade.

**Para ler**

- E. W. Dijkstra, *Algol 60 translation*, relatório MR 35, Mathematisch Centrum, 1961 (origem do
  shunting-yard).
- Livros-texto de estruturas de dados (Tenenbaum, Langsam e Augenstein; Celes, Cerqueira e
  Rangel; Cormen et al.): capítulos de pilhas e de expressões.
- Manuais das calculadoras HP (seção *RPN* e *stack*) e o manual de programação de uma família de
  CLP, procurando as instruções de pilha lógica (ex.: `ANB`, `ORB`, `MPS`).

## Exercícios propostos

Enunciados para praticar (sem solução):

1. **Pilha com mínimo em O(1).** Um supervisório registra leituras de pressão numa pilha. Implemente
   `push`, `pop` e `minimo()` com as três em O(1). Dica: uma segunda pilha.
2. **Duas pilhas num vetor só.** Use um único vetor de capacidade C para duas pilhas que crescem a
   partir das pontas. O overflow só pode acontecer quando o vetor inteiro estiver cheio.
3. **Pilha que dobra.** Modifique `02_pilha_array.c` para usar `malloc`/`realloc` e dobrar a
   capacidade quando encher. Conte as cópias para n = 10…10000 e compare com a tabela do `06`.
4. **Validador de tags.** Estenda o `11` para validar marcações do tipo `<motor><vel>10</vel></motor>`,
   em que a pilha guarda nomes (strings) em vez de caracteres.
5. **Menos unário.** O `13` rejeita `-3 + 4`. Proponha uma forma de distinguir o `-` unário do
   binário e de representá-lo na pós-fixa.
6. **AGV: pilha × fila.** Troque a pilha do `15` por uma fila (veja [`../06_filas/`](../06_filas/)) e compare
   o comprimento da rota e o número de casas visitadas. Qual encontra a rota mais curta? Por quê?
7. **Histórico circular.** No `14`, o descarte da ação mais antiga custa O(C). Reescreva o
   histórico como um buffer circular para que todas as operações fiquem O(1).
8. **Torre de Hanói sem recursão.** Resolva Hanói com n discos usando uma pilha explícita de
   "tarefas", na linha da Parte 2 do `16`, e mostre a altura máxima da pilha em função de n.

## Nomes anteriores dos arquivos

A pasta foi renumerada para seguir a ordem de estudo. Os exemplos citados em aula com os nomes
antigos correspondem a:

| Nome antigo | Nome atual |
| --- | --- |
| `00_visao_geral.c` | `01_pilha_visao_geral.c` |
| `01_pilha_array.c` | `02_pilha_array.c` |
| `02_pilha_dinamica.c` | `03_pilha_dinamica.c` |
| `03_parenteses.c` | `10_delimitadores_passo_a_passo.c` (reescrito, agora visual) |
| `04_conversao_posfixa.c` | `13_infixa_posfixa_shunting_yard.c` (reescrito) |
| `05_pilha_portugues.c` | `05_historico_navegador.c` |
| `06_valida_codigo_c.c` | `11_valida_codigo_c.c` |
| `07_valida_chaves.c` | `07_valida_chaves.c` |
| `08_pilha_basica.c` | `00_pilha_basica.c` |
| `09_pilha_strings.c` | `04_pilha_strings.c` |
| `10_valida_colchetes.c` | `08_valida_colchetes.c` |
| `11_valida_delimitadores.c` | `09_valida_delimitadores.c` |

## Módulos relacionados

- Recursão e pilha de chamadas: [`../02_revisao_funcoes_recursao/`](../02_revisao_funcoes_recursao/)
- TADs: [`../04_tads/`](../04_tads/)
- Filas, filas circulares e deque: [`../06_filas/`](../06_filas/)
- Listas encadeadas (base da pilha dinâmica): [`../07_listas/`](../07_listas/)
- Busca em profundidade (DFS) com pilha em grafos: [`../10_grafos/`](../10_grafos/)
