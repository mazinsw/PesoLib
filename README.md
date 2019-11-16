[![GitHub tag](https://img.shields.io/github/release/mazinsw/PesoLib.svg)](https://github.com/mazinsw/PesoLib/releases)

Biblioteca para obtenção de peso sobre balanças
=

PesoLib é uma biblioteca capaz de detectar e conectar-se
à uma balança plugada no computador, os pesos podem ser recebidos
sem a necessidade de solicitação, ou seja quando um item for colocado sobre
a balança a biblioteca enviará o peso automaticamente para sua aplicação.

com ela é possível também informar o preço do item, e mostrar o total no visor da
balança(apenas para balanças que o suportam).

possui exemplos nas linguagens Java, Delphi e C, para 32 e 64 bit
e encontram-se na pasta "wrapper"

Atualmente suporta vários modelos das marcas Toledo, Filizola, Magna, Urano, Elgin e Alfa

Algumas balanças precisam que suas configurações sejam alteradas para o envio automatico do peso
verifique o manual e faça o teste!
Se mesmo assim você não conseguiu conectar à sua balança
envie um email para desenvolvimento@grandchef.com.br 
com o título: Biblioteca para balanças
informando a marca, modelo da balança e a conexão usada(Serial, Paralela ou USB)

sugestões e agradecimentos são sempre bem vindos,
então não deixe de enviar um "Valeu!" para desenvolvimento@grandchef.com.br

### Compilando

Para compilar e criar as dll e executáveis, você precisará do docker instalado em sua máquina

- Windows
  ```cmd
  docker run -ti -v %cd%:/mnt mazinsw/mingw-w64:4.0.4 /bin/sh -c "cd /mnt && make clean shared64 && make clean shared32 && make clean static64 && make clean static32"
  ```

- Linux
  ```cmd
  docker run -ti -v `pwd`:/mnt -u `id -u $USER`:`id -g $USER` mazinsw/mingw-w64:4.0.4 /bin/sh -c 'cd /mnt && make clean shared64 && make clean && make static64 && make clean static32'
  ```

![Alt text](/res/img/teste.png?raw=true "Programa de teste")

![Alt text](/res/img/delphi.png?raw=true "Exemplo em Delphi")

![Alt text](/res/img/java.png?raw=true "Exemplo em Java")