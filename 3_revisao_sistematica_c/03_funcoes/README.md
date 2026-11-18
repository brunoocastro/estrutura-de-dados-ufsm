# Funções em C

Esta pasta apresenta diferentes aspectos das funções em C. Cada arquivo é um
programa independente, com comentários e comandos de compilação no cabeçalho.

## Arquivos

| Arquivo | Conteúdo |
| --- | --- |
| `00_visao_geral.c` | visão geral: protótipos, escopo e passagem de parâmetros |
| `01_prototipos.c` | declaração, definição, parâmetros e retorno |
| `02_escopo.c` | escopo local, global e `static` local |
| `03_valor_ponteiro.c` | passagem por valor e alteração por ponteiro |
| `04_arrays.c` | funções que recebem e alteram arrays |
| `05_const.c` | `const` em arrays, strings e parâmetros |
| `06_recursao.c` | caso base, passo recursivo e pilha de chamadas |
| `07_funcao_como_dado.c` | ponteiros para função e `typedef` |
| `08_args.c` | `argc`, `argv` e argumentos da linha de comando |

## Como compilar e executar

Na raiz do repositório, copie e cole:

```bash
cd 3_revisao_sistematica_c/03_funcoes
gcc -std=c11 -Wall -Wextra -pedantic 01_prototipos.c -o 01_prototipos
./01_prototipos
```

Para compilar um exemplo diferente, troque o nome do arquivo e do executável:

```bash
cd 3_revisao_sistematica_c/03_funcoes
gcc -std=c11 -Wall -Wextra -pedantic 06_recursao.c -o 06_recursao
./06_recursao
```

O exemplo de argumentos recebe valores após o nome do executável:

```bash
cd 3_revisao_sistematica_c/03_funcoes
gcc -std=c11 -Wall -Wextra -pedantic 08_args.c -o 08_args
./08_args um dois tres
```

## Compilar todos os exemplos

```bash
cd 3_revisao_sistematica_c/03_funcoes
for arquivo in *.c; do
    executavel="${arquivo%.c}"
    gcc -std=c11 -Wall -Wextra -pedantic "$arquivo" -o "$executavel" || exit 1
done
```

Executar todos, incluindo o exemplo com argumentos:

```bash
cd 3_revisao_sistematica_c/03_funcoes
for executavel in 00_visao_geral 01_prototipos 02_escopo 03_valor_ponteiro 04_arrays 05_const 06_recursao 07_funcao_como_dado; do
    echo "=== $executavel ==="
    ./$executavel
done
./08_args um dois tres
```

## Conceitos principais

- **Protótipo:** declaração que informa a assinatura antes da chamada.
- **Retorno:** `return` entrega um valor compatível com o tipo declarado; `void`
  indica que a função não devolve valor.
- **Escopo:** locais vivem no bloco; globais podem ser acessadas amplamente;
  `static` local preserva o valor entre chamadas.
- **Passagem por valor:** o parâmetro é uma cópia. Para modificar o chamador,
  passe um ponteiro e desreferencie-o com `*`.
- **Arrays:** ao passá-los a uma função, o array decai para ponteiro; o tamanho
  deve acompanhar o parâmetro.
- **`const`:** impede modificações através daquele parâmetro e documenta a
  intenção de somente leitura.
- **Recursão:** precisa de caso base e avanço em direção a ele.
- **Ponteiro para função:** permite escolher uma operação em tempo de execução.
- **`argc` e `argv`:** permitem receber argumentos do shell como strings.
