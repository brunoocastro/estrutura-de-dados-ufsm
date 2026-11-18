# Revisão sistemática de C

O conteúdo foi dividido por grandes assuntos. Cada assunto possui uma pasta com
vários exemplos independentes e um README próprio.

## Conteúdo

| Pasta | Conteúdo |
| --- | --- |
| `01_tipos_simples/` | tipos básicos, modificadores, memória, conversões e constantes |
| `02_tipos_compostos/` | arrays, matrizes, strings, `struct`, `union` e `enum` |
| `03_funcoes/` | protótipos, escopo, passagem por valor e ponteiros |
| `04_recursao/` | casos base, rastreamento de chamadas, arrays, busca e algoritmos recursivos |

## Começar pelos tipos simples

Copie e cole na raiz do repositório:

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
cat README.md
```

Para compilar todos os exemplos da primeira pasta:

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
for arquivo in *.c; do
    executavel="${arquivo%.c}"
    gcc -std=c11 -Wall -Wextra -pedantic "$arquivo" -o "$executavel" || exit 1
done
```

Para executar todos:

```bash
cd 3_revisao_sistematica_c/01_tipos_simples
for executavel in 00_visao_geral 01_char 02_inteiros 03_reais 04_conversoes 05_constantes; do
    echo "=== $executavel ==="
    ./$executavel
done
```

Consulte `01_tipos_simples/README.md` para explicações, comandos individuais e
os objetivos pedagógicos de cada exemplo.

Para iniciar o estudo de tipos compostos:

```bash
cd 3_revisao_sistematica_c/02_tipos_compostos
cat README.md
```

Consulte `02_tipos_compostos/README.md` para os comandos e explicações de arrays,
matrizes, strings, structs, unions e enums. As explicações específicas estão em
[`arrays.md`](02_tipos_compostos/arrays.md) e
[`structs.md`](02_tipos_compostos/structs.md) e
[`ponteiros.md`](02_tipos_compostos/ponteiros.md).

Todos os READMEs explicam o uso do `gcc` e das opções `-std=c11`, `-Wall`,
`-Wextra`, `-pedantic` e `-o`.

## Funções

Para estudar funções com exemplos independentes:

```bash
cd 3_revisao_sistematica_c/03_funcoes
cat README.md
```

Consulte `03_funcoes/README.md` para exemplos de protótipos, escopo, ponteiros,
arrays, `const`, recursão, ponteiros para função e argumentos da linha de comando.

Para estudar exclusivamente recursão:

```bash
cd 3_revisao_sistematica_c/04_recursao
cat README.md
```

A pasta `04_recursao/` contém rastreamentos detalhados de fatorial, Fibonacci,
soma de array, busca binária, palíndromos e MDC.

## Demais assuntos

Os arquivos de visão geral dos outros assuntos estão em:

```text
02_tipos_compostos/00_visao_geral.c
03_funcoes/00_visao_geral.c
```

Eles serão ampliados com novos exemplos detalhados nas próximas etapas.
