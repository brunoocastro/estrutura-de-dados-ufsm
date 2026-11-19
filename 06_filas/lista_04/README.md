# Lista de Exercícios 04 (T04 / L04): filas

> Tarefa avaliada. Este arquivo traz apenas o enunciado e orientações gerais.

## Enunciado

Implemente em C um **sistema de atendimento** com duas filas:

- **fila normal**: clientes em geral;
- **fila prioritária**: idosos, gestantes, pessoas com deficiência (PcD) e demais casos
  previstos em lei.

### Requisitos

1. Cada fila começa com **capacidade inicial `N`**, informada pelo usuário e alocada
   dinamicamente com `malloc`.
2. Quando uma fila estiver cheia e chegar mais um cliente, a capacidade deve **crescer
   automaticamente** com `realloc`, **preservando a ordem de chegada** dos clientes que já
   estavam na fila.
3. Regra de atendimento: podem ser atendidos **até 5 clientes prioritários em sequência**;
   depois disso, se houver alguém na fila normal, o próximo atendido é **um cliente da fila
   normal**. Se uma das filas estiver vazia, atende-se a outra.
4. Menu com as opções:
   1. chegada de cliente normal;
   2. chegada de cliente prioritário;
   3. atender o próximo cliente;
   4. visualizar as filas;
   0. sair.
5. Ao sair, toda a memória alocada deve ser liberada.

## Orientações

- Organize o código em funções (criar, inserir, remover, vazia, cheia, mostrar, liberar).
- Teste com `N = 1` para provocar o crescimento logo na segunda chegada.
- Teste a regra de prioridade com as duas filas cheias de clientes.
- Trate entrada inválida e fim de entrada (`EOF`) no menu.
- Material de apoio: os exemplos de fila circular, fila encadeada e deque da pasta
  [`06_filas`](../README.md).
