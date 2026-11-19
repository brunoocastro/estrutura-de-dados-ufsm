# Filas em C

Unidade 3 da ementa (UFSM00064 — Estrutura de Dados): **3.1 fila simples** e **3.2 fila dupla**.

## O que a ementa pede e onde está

| Item da ementa | Onde está nesta pasta |
| --- | --- |
| 3.1 Fila simples em vetor (linear e circular) | `01_fila_linear_vs_circular.c` |
| 3.1 Fila simples encadeada | `02_fila_encadeada.c` |
| 3.1 Aplicações de fila (buffer, escalonamento) | `03_ring_buffer_uart.c`, `06_round_robin_clp.c` |
| 3.2 Fila dupla (deque) | `04_deque_circular.c` |
| 3.2 Aplicação de deque | `05_janela_deslizante_maximo.c` |
| Lista de exercícios avaliada (T04 / L04) | [`lista_04/`](lista_04/README.md), só o enunciado |

## Conceito

Uma **fila** é FIFO (*First In, First Out*): quem chega primeiro sai primeiro. Insere-se no
**fim** e remove-se da **frente**, como numa fila de banco ou de peças numa esteira.

```
  enfileirar(x) --> [ fim ] ... [ frente ] --> desenfileirar()
```

**Fila em vetor (circular).** Dois índices andam para a direita e, ao chegar ao fim do vetor,
voltam para 0 com `% cap`. As casas liberadas na frente são reaproveitadas.

```
  índice:   0     1     2     3     4     5
          [ 70][ 80][   ][ 40][ 50][ 60]        ordem lógica: 40 50 60 70 80
                     fim   ini                  fim = (fim + 1) % cap
```

Sem o módulo (fila linear) há duas saídas ruins: deslocar todos os elementos a cada remoção,
que custa O(n), ou aceitar o **falso cheio**, com `fim` no final do vetor e casas livres no
início que nunca voltam a ser usadas.

**Fila encadeada.** Os nós ficam ligados por ponteiros, e a fila guarda dois deles: `ini`, de
onde se remove, e `fim`, onde se insere. Sem `fim`, cada inserção teria de percorrer a fila
inteira.

```
  ini                fim
   |                  |
   v                  v
  [10|*]-->[20|*]-->[30|/]
```

**Fila dupla (deque).** Permite inserir e remover nas **duas** pontas. Em vetor circular, as
4 operações são O(1). Usando só uma ponta o deque funciona como pilha, e usando uma ponta para
entrar e outra para sair funciona como fila.

```
  inserir_frente --> [ 1 | 5 | 10 | 20 ] <-- inserir_fim
  remover_frente <--                     --> remover_fim
```

## Ordem de estudo recomendada

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `01_fila_linear_vs_circular.c` | O mesmo roteiro em três filas de vetor: com deslocamento, linear com falso cheio e circular com *wrap-around*. Tabela de cópias para n = 10…10000 | deslocamento O(n) por remoção (n(n-1)/2 no total); circular O(1) |
| `02_fila_encadeada.c` | Nós e ponteiros `ini`/`fim` desenhados a cada operação, a armadilha de não zerar `fim` e o custo de enfileirar sem o ponteiro `fim`. Menu livre | O(1) com `fim`; O(n) sem ele |
| `03_ring_buffer_uart.c` | Buffer circular entre a ISR da UART e o laço principal: máscara `& (cap-1)`, contadores de 8 bits que transbordam, *overflow* com as políticas descartar novo e sobrescrever antigo, dimensionamento | O(1) por byte, sem malloc |
| `04_deque_circular.c` | Deque em vetor circular, armadilha de `(-1) % cap` em C, deque como pilha e como fila, histórico "desfazer" de setpoints com limite. Menu livre | 4 operações O(1) |
| `05_janela_deslizante_maximo.c` | Pico de pressão em janela de k leituras com deque monotônico, comparado ao método ingênuo. Tabela até n = 100000 | O(n) contra O(n·k) |
| `06_round_robin_clp.c` | Escalonador round-robin de tarefas de CLP com fila de prontos, mostrado tick a tick: Gantt, espera, resposta, trocas de contexto e comparação de quanta com FCFS | O(1) por decisão |

Todos os programas desenham o estado a cada passo. Num terminal, ENTER avança; com
`< /dev/null`, rodam direto até o fim.

## Complexidade por implementação

| Operação | Vetor com deslocamento | Vetor linear simples | Vetor circular | Encadeada (`ini`+`fim`) | Deque circular |
| --- | --- | --- | --- | --- | --- |
| enfileirar / inserir_fim | O(1) | O(1)* | O(1) | O(1) | O(1) |
| desenfileirar / remover_frente | **O(n)** | O(1) | O(1) | O(1) | O(1) |
| inserir_frente | — | — | — | — | O(1) |
| remover_fim | — | — | — | O(n)** | O(1) |
| frente, vazia, tamanho | O(1) | O(1) | O(1) | O(1) | O(1) |
| capacidade | fixa | fixa, desperdiça | fixa, sem desperdício | ilimitada (1 malloc por nó) | fixa |
| memória extra por elemento | 0 | 0 | 0 | 1 ponteiro | 0 |

\* recusa inserções quando `fim` chega ao final do vetor, mesmo com casas livres (falso cheio).
\*\* na lista simplesmente encadeada é preciso achar o penúltimo nó. Um deque encadeado usa
lista **duplamente** encadeada (ver [`../07_listas/`](../07_listas/)).

A notação Big O é explicada em
[`../01_revisao_c_basico/complexidade_big_o.md`](../01_revisao_c_basico/complexidade_big_o.md).

## Exemplos reais

**Engenharia de controle e automação**
- Buffers de recepção e transmissão de UART, CAN, USB e Modbus RTU entre a interrupção e o laço
  principal (`03_ring_buffer_uart.c`).
- Filas de mensagens entre tarefas de um RTOS (FreeRTOS `xQueueSend`/`xQueueReceive`).
- Escalonamento round-robin de tarefas de mesma prioridade (`06_round_robin_clp.c`).
- Peças numa esteira ou lotes à espera de uma máquina (`02_fila_encadeada.c`).
- Fila de alarmes do supervisório, exibidos e reconhecidos em ordem de ocorrência.
- Janela deslizante de leituras para média móvel, pico ou mínimo (`05_janela_deslizante_maximo.c`).
- Histórico de desfazer com limite numa IHM (`04_deque_circular.c`).

**Dia a dia**
- Fila de impressão, atendimento por senha, *buffer* de vídeo em *streaming*.
- Busca em largura em grafos, que usa uma fila (ver [`../10_grafos/`](../10_grafos/)).
- Botão "voltar" do navegador com histórico limitado (deque).

## Como executar

Os comandos são dados a partir da raiz do repositório. O script `run` compila para `build/`.

```sh
./run list filas                          # lista os exemplos desta pasta
./run 01_fila_linear_vs_circular          # passo a passo: ENTER avança
./run 03_ring_buffer_uart < /dev/null     # executa direto, sem pausas
./run 04_deque_circular                   # demonstração seguida de menu interativo
```

Para compilar com as flags estritas e o *sanitizer*:

```sh
CFLAGS="-std=c11 -Wall -Wextra -pedantic -g -fsanitize=address,undefined" ./run build 06_filas/05_janela_deslizante_maximo.c
```

## Exercícios propostos (sem solução)

1. Em `01_fila_linear_vs_circular.c`, troque o contador `tam` pela convenção de deixar sempre uma
   casa vazia. Escreva os novos testes de vazia e cheia e diga quantos elementos cabem num vetor
   de capacidade `cap`.
2. Escreva `fila_mostrar` para a fila circular sem usar `%` dentro do laço. Use dois laços, um de
   `ini` até o fim do vetor e outro de 0 até `fim`.
3. Implemente uma fila com **duas pilhas** (veja [`../05_pilhas/`](../05_pilhas/)). Mostre que,
   embora uma remoção possa custar O(n), o custo amortizado por operação é O(1).
4. Altere `03_ring_buffer_uart.c` para que o laço principal só processe uma linha quando receber
   o `\n` e descarte linhas com tamanho diferente do esperado. Conte quantas linhas foram
   descartadas em cada cenário.
5. Implemente um deque **encadeado** com lista duplamente encadeada e compare o uso de memória com
   o deque em vetor para 1000 elementos.
6. Adapte `05_janela_deslizante_maximo.c` para calcular ao mesmo tempo o **mínimo** e o
   **máximo** de cada janela e disparar um alarme quando a amplitude máx − mín passar de 30 kPa.
7. Em `06_round_robin_clp.c`, acrescente o custo da troca de contexto (1 tick perdido por troca) e
   encontre o quantum que minimiza a espera média para o conjunto de tarefas do programa.
8. Simule uma esteira com uma estação de inspeção: peças chegam a cada 2 s, a inspeção leva de 1 a
   4 s (sequência fixa) e a esteira comporta no máximo 5 peças. Use uma fila circular, conte as
   peças perdidas e calcule o tempo médio de espera.
