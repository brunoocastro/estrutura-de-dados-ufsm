# 02 — Tipos compostos: vetores, matrizes, strings, structs e ponteiros

**Ementa:** 0.4 (vetores e matrizes), 0.5 (strings), 0.7 (ponteiros) e 1.1 (structs).

Tipos compostos agrupam vários valores. A pergunta central desta pasta é **como eles ficam na
memória**, porque disso decorre o custo de cada operação:

```text
 int v[5]:   [ 10 ][ 20 ][ 30 ][ 40 ][ 50 ]        contíguo: &v[i] = base + i*sizeof(int)
             base  +4    +8    +12   +16           -> acesso O(1), inserir no meio O(n)

 char s[8]:  [ 'C' ][ ' ' ][ 'e' ][ 'h' ][ '\0' ][ ? ][ ? ][ ? ]
              strlen(s) = 4 (procura o '\0': O(n))   sizeof s = 8 (capacidade)

 int m[2][3]: [1][2][3][4][5][6]                    linha após linha (row-major)

 struct {char a; double b;}:  [a][pad pad pad pad pad pad pad][b b b b b b b b]

 int *p = &v[2]:   p --> [ 30 ]       p + 1 avança sizeof(int) bytes
```

Textos de apoio: [`arrays.md`](arrays.md), [`structs.md`](structs.md),
[`ponteiros.md`](ponteiros.md).

## Ordem de estudo

| Arquivo | O que mostra | Complexidade-chave |
| --- | --- | --- |
| `00_tipos_compostos_visao_geral.c` | panorama: array, matriz, string, struct, union, enum | — |
| `01_arrays.c` | inicialização, endereços, `base + i*tamanho`, busca sequencial, decaimento | `v[i]` O(1); busca O(n) |
| `02_vetor_insercao_remocao.c` | **passo a passo**: deslocamentos ao inserir/remover no meio | O(n); no fim O(1) |
| `03_matrizes.c` | row-major, deslocamento `l*C + c`, soma de linhas/colunas | acesso O(1); percorrer O(L·C) |
| `04_strings.c` | `'\0'`, `strlen` × `sizeof`, byte a byte | `strlen` O(n) |
| `05_strings_maiores.c` | `fgets`, cópia e concatenação seguras, `strstr`/`strchr` | O(n); `strstr` O(n·m) |
| `06_strings_custo_oculto.c` | **contadores**: `strlen` no `for` e `strcat` repetido viram O(n²) | O(n²) × O(n) |
| `07_structs.c` | 4 formas de declarar, `.` e `->`, padding, ordem dos campos | campo O(1) |
| `08_unions.c` | membros no mesmo endereço, bytes de um `float` | O(1) |
| `09_enums.c` | constantes nomeadas para estados | O(1) |
| `10_structs_aninhadas.c` | struct dentro de struct, cópia por valor | cópia O(sizeof) |
| `11_ponteiros_simples.c` | `&x`, `p`, `*p` com desenho das caixas | O(1) |
| `12_ponteiros.c` | `**`, ponteiro em função, vetor, `->`, `NULL`, primeiro `malloc` | O(1) |
| `13_aritmetica_ponteiros.c` | **passo a passo**: ponteiro andando no vetor; cursor `**` num quadro Modbus | `p + k` O(1) |

## Tabela comparativa

| Operação | Vetor | Matriz | String | Struct |
| --- | --- | --- | --- | --- |
| acesso a um elemento/campo | O(1) | O(1) | O(1) | O(1) |
| tamanho | `sizeof` O(1) (só no escopo do vetor) | `sizeof` O(1) | `strlen` **O(n)** | `sizeof` O(1) |
| busca por valor | O(n) | O(L·C) | `strchr` O(n) | — |
| inserir/remover no meio | O(n) | — | O(n) | — |
| copiar | O(n) (laço/`memcpy`) | O(L·C) | `strcpy` O(n) | `=` O(sizeof) |

## Exemplos reais

- Média móvel das últimas leituras de um sensor (vetor); mapa de ocupação de um AGV (matriz).
- Comandos de texto pela serial e linhas de log (strings com capacidade fixa).
- Registro de alarme de SCADA (struct + enum), bytes de um `float` no Modbus (union).
- Decodificação de quadro Modbus RTU com um cursor (`const uint8_t **`).

## Como executar

```sh
./run 02_vetor_insercao_remocao               # ENTER avança cada passo
./run 06_strings_custo_oculto < /dev/null     # sem pausas
./run list tipos_compostos
```

## Exercícios propostos

1. Escreva `remover_valor(v, &n, x)` que remove **todas** as ocorrências de `x` com uma única
   passada (O(n)), sem deslocar o vetor várias vezes. Conte as movimentações.
2. Escreva `transposta(L, C, m, t)` para matrizes de tamanho fixo e imprima os endereços de
   `m[0][1]` e `t[1][0]`. Por que um percorre memória contígua e o outro não?
3. Escreva `inverte_string(s)` *in place* usando dois ponteiros (início e fim) e mostre que é O(n).
4. Monte a linha `"t=1200;TT101=75.3;PT201=5.01"` com `snprintf` mantendo um ponteiro para o
   fim do texto (sem `strcat`).
5. Reordene os campos de `struct { char a; int b; char c; double d; }` para minimizar o padding
   e confirme com `sizeof` e `offsetof`.
6. Escreva `int *procura(int *ini, int *fim, int x)` que devolve um ponteiro para `x` ou `fim` se
   não achar (o mesmo contrato das bibliotecas de C++).

Próximo: [`../03_funcoes/`](../03_funcoes/). Memória que cresce em execução:
[`../05_alocacao_dinamica/`](../05_alocacao_dinamica/).
