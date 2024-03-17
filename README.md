# Projeto de Controle de Cruzamento de Trens

## Descrição
Este projeto simula um sistema de controle para o cruzamento de trens. Ele utiliza threads para representar cada trem e sincroniza seu acesso ao cruzamento utilizando semáforos, mutexes e variáveis de condição em C.

## Objetivo
O objetivo principal do projeto é gerenciar o cruzamento de vários trens com diferentes prioridades (alta, média, baixa) de forma segura e eficiente. O sistema garante que trens com prioridade mais alta tenham preferência no cruzamento e que a ordem de chegada seja respeitada dentro de cada categoria de prioridade.

## Funcionamento
Cada trem é representado por uma thread. Quando um trem chega ao cruzamento, ele é inserido em uma fila de espera. O sistema então escolhe o próximo trem a cruzar com base em sua prioridade e ordem de chegada. Através de um semáforo, é garantido que apenas um trem cruze o cruzamento por vez.

## Como Compilar e Rodar
Para compilar o projeto, é necessário ter um compilador de C e o `make` instalado no sistema. 

1. Clone o repositório para o seu computador local, utilizando o comando `git clone https://github.com/sabrinaacardoso/cruzamento_de_trem.git`
2. Abra um terminal e navegue até a pasta do projeto.
3. Digite o comando `make` para compilar o programa.

### Rodando o Programa
Após compilar, você pode rodar o programa digitando `./cruzamento` no terminal.

## Estrutura do Código
- **main.c**: Contém a lógica principal do programa, criação de threads e sincronização.
- **Makefile**: Usado para compilar o projeto facilmente usando o comando `make`.

## Dependências
- Bibliotecas padrão do C (`stdio.h`, `stdlib.h`, `string.h`, `unistd.h`)
- Biblioteca de threads POSIX (`pthread.h`)
- Biblioteca de semáforos POSIX (`semaphore.h`)

## Autor
Sabrina Cardoso e Dhara Pamplona
