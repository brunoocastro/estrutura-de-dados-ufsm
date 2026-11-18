# DPEE1038 — Lista de Exercícios 04: Filas

## Solução

O arquivo [`01_fila_atendimento.c`](01_fila_atendimento.c) implementa uma fila com capacidade inicial `N`, alocada dinamicamente com `malloc`.

O programa mantém duas filas:

- **fila normal**: clientes em geral;
- **fila prioritária**: idosos, pessoas com necessidades especiais, gestantes e demais casos previstos em lei.

A regra de atendimento é: podem ser atendidas até cinco pessoas prioritárias consecutivamente; havendo clientes na fila normal, o próximo atendimento será então de uma pessoa da fila normal. Se uma das filas estiver vazia, a outra é atendida.

A fila usa um array circular. Quando ela está cheia, a função `insert` chama automaticamente `fila_aumenta`, que preserva a ordem dos elementos e aumenta o espaço com `realloc`.

## Compilação

A partir da raiz do repositório:

```bash
gcc -std=c11 -Wall -Wextra -pedantic \
  5_filas/lista_04/01_fila_atendimento.c \
  -o /tmp/01_fila_atendimento
```

## Execução

```bash
/tmp/01_fila_atendimento
```

Ao iniciar, informe `N`, a capacidade inicial de cada fila. Depois, use o menu:

1. chegada de cliente normal;
2. chegada de cliente prioritário;
3. atendimento do próximo cliente;
4. visualização das filas;
0. encerramento.

## Verificação

Para testar o aumento automático, informe `N = 1` e insira mais de um cliente em qualquer fila. A capacidade será ampliada sem perda da ordem de chegada.

Para testar a prioridade, insira clientes nas duas filas e selecione a opção de atendimento várias vezes. A fila prioritária será atendida primeiro, mas a fila normal será atendida depois de cinco prioritários quando ainda houver clientes aguardando.
