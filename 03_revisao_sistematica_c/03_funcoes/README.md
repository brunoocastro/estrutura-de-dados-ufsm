# 03 — Funções

**Ementa:** 0.6 (funções) e 0.7 (ponteiros como parâmetros).
**Classroom:** "Revisão sobre funções, recursividade e ponteiros".

Uma função recebe **cópias** dos argumentos (C só tem passagem por valor). Para alterar uma
variável do chamador, passa-se o **endereço** dela. Cada chamada cria um **quadro** (frame) na
pilha com os parâmetros e as variáveis locais; o quadro some quando a função retorna.

```text
 main: x = 10                         troca_por_ponteiro(&x, &y)
 +-----------------+                  +----------------------+
 | x: 10  y: 20    | <---- a, b ----- | a = &x   b = &y      |   *a = *b altera x de verdade
 +-----------------+                  +----------------------+
                                      troca_por_valor(x, y): a e b são CÓPIAS; x e y não mudam
```

## Ordem de estudo

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `00_funcoes_visao_geral.c` | protótipos, global × local, troca por valor × por ponteiro | O(1) por chamada |
| `01_prototipos.c` | declaração antes do uso; definição depois | O(1) |
| `02_escopo.c` | local (pilha) × `static` local × global, com endereços | O(1) |
| `03_valor_ponteiro.c` | a cópia mora em outro endereço; `*p = 99` altera o original | O(1) |
| `04_arrays.c` | vetor passado como ponteiro: sem cópia, função altera o original | passar O(1); percorrer O(n) |
| `05_const.c` | `const` em parâmetros somente leitura | O(n) |
| `06_recursao.c` | primeira recursão (fatorial com níveis) — aprofundada em `../04_recursao/` | O(n) tempo e pilha |
| `07_funcao_como_dado.c` | ponteiro para função e tabela de despacho | O(1) |
| `08_args.c` | `argc`/`argv` (vetor de strings) | O(k) |

## Custos que importam

| Situação | Custo | Por quê |
| --- | --- | --- |
| chamar uma função | O(1) | cria um quadro na pilha |
| passar `int`, `double`, ponteiro | O(1) | copia poucos bytes |
| passar vetor | O(1) | só o endereço do 1º elemento é copiado |
| passar struct **por valor** | O(sizeof(struct)) | a struct inteira é copiada |
| passar struct por ponteiro (`const T *`) | O(1) | só o endereço |
| escolher ação por tabela de ponteiros | O(1) | em vez de um `switch` com n casos |

## Exemplos reais

- `calibrar(leituras, n, &ganho, &offset)`: vários resultados saem por ponteiros.
- Filtro de média móvel com estado `static` interno.
- Tabela de tratadores de comandos Modbus indexada pelo código da função (ponteiros para função).
- Ferramenta de configuração via linha de comando (`argv`).

## Como executar

```sh
./run 00_funcoes_visao_geral               # pede dois inteiros
./run 00_funcoes_visao_geral < /dev/null   # usa 10 e 20
./run 08_args um dois tres
./run list 03_funcoes
```

## Exercícios propostos

1. Escreva `void min_max(const int v[], size_t n, int *min, int *max)` (O(n), uma passada).
2. Escreva `void ordena3(int *a, int *b, int *c)` que deixa `*a <= *b <= *c` usando apenas trocas.
3. Escreva `double aplica_em_todos(double v[], size_t n, double (*f)(double))` que substitui cada
   elemento por `f(v[i])` e teste com `sqrt` e com uma função `celsius_para_fahrenheit`.
4. Use uma variável `static` para escrever `int proximo_id(void)` que devolve 1, 2, 3...
5. Por que `void zera(int *p) { p = NULL; }` não altera o ponteiro do chamador? Corrija com `int **`.

Próximo: [`../04_recursao/`](../04_recursao/).
