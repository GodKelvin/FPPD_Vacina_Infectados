# Resumo
Trabalho realizado durante a disciplina de Fundamentos de Programação Paralela e Distribuída (2021/2), do curso Bacharelado em Sistemas de informação (BSI) do IFES-Campus Serra.

## Docente da disciplina
Flávio Severiano Lamas

## Assunto
Sincronização de threads garantindo exclusão mútua, evitando deadlock e starvation.

## Para compilar
```
gcc -Wall -pedantic main.c vacina.c vacina.h -pthread -lrt -o exec
```

## Para rodar os testes
Para fins de agilidade, criei um script bash para executar diversas vezes o algoritmo, dada a quantidade de trabalho mínima informada. 

Mas claro, o código também pode ser executada da forma original. O script bash em nada impacta nisso.

Primeiro, deve tornar o arquivo bash executável (p/Linux)
```
chmod a+x run_test.sh
```

Em seguida, executa-lo informando a quantidade de trabalho mínima.<br>
(O script bash está configurando para executar 500 vezes).
```
./run_test.sh <qtd_trabalho_minima>
```

O print das execuções pode ser encontrado em 

```
prints_exec
```

Cujo formato é: 

```
Exec_<quantidade_minima_de_trabalho>_<como_foi_executada>_<numero_do_print>
```

Sendo: 

Quantidade_minima_executada: um inteiro informando a quantidade de trabalho minima daquele print.<br>
Como_foi_executado: Manual ou Bash<br>
Numero_do_print: Apenas um identificador da imagem.

<i><b> Há também um print chamado "Exec_multiples_manual", que nada mais é do que a execução do código da forma convencional (não utilizando bash) com vários valores para o trabalho mínimo.</b></i>


## Descrição do Problema
Existem 6 processos nesse problema: 3 processos infectados e 3 processos laboratórios.

Cada um dos infectados vai fazer uma vacina e usá-la [o vírus é altamente mutável então a vacina tem que ser reaplicada a todo momento que for possível].

Para fazer uma vacina é necessário vírus morto, injeção e insumo secreto.

Cada infectado tem 1 dos 3 produtos.

Exemplo: 1 processo infectado tem um suprimento infinito de vírus morto, outro tem de injeção e o último tem de insumo secreto.

Cada laboratório tem um diferente suprimento infinito de 2 dos 3 produtos.
* Exemplo:laboratorio1 tem injeção, vírus morto, laboratorio2 tem injeção e insumo secreto e laboratorio3 tem insumo secreto e vírus morto.

Cada laboratório coloca seus 2 produtos próprios numa mesa em comum a todos infectados a disposição de quem quiser e todos os infectados correm pra pegar os dois que eles precisam pra fazer a vacina e somente renovam quando os SEUS PRÓPRIOS distribuídos forem consumidos. [o lab2 não renova se consumir a injeção do lab1 e o insumo secreto do lab3, por exemplo]


* Como garantir exclusão mutua, sem deadlock e sem starvation e o seu código ser o mais paralelo possível?

[dica, só vai funcionar com uma mescla de semáforos e mutexes]

a entrada do código será um numero que vai ser o numero de vezes MÍNIMO que cada thread realizou seu objetivo primordial. [um infectado que atinge o mínimo NÃO para de concorrer com os outros, ele continua se vacinando]

A saída deverá ser o número de vezes que cada thread realizou seu objetivo primordial:
no caso, os infectados irão contabilizar o número de vezes que cada um se injetou com a vacina e os laboratórios o número de vezes que cada um renovou seus estoques:
Exemplo de de execução:

./covid-19 90  [irá executar até todas as threads terem executado seu objetivo PELO MENOS 90 vezes]
infectado 1: 120
infectado 2: 100
infectado 3: 90
laboratorio 1: 120
laboratorio 2: 100
laboratorio 3: 90

[isso é um exemplo de MOLDE e em nenhum momento condiz com a real saída de números esperada]