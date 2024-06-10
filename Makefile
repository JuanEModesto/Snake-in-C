#CC = gcc: Define a variável CC como o compilador a ser utilizado, que neste
#caso é o gcc.
CC = gcc
#CFLAGS = -Wall -Wextra: Define a variável CFLAGS para conter as opções de
#compilação, neste caso incluindo as flags -Wall e -Wextra para habilitar
#avisos extras do compilador.
CFLAGS = -Wall -Wextra
#INCLUDES = -I/usr/include/SDL2/: Define a variável INCLUDES para incluir o
#diretório de cabeçalhos do SDL2.
INCLUDES = -I/usr/include/SDL2/
#LIBS = -lSDL2 -lSDL2_ttf: Define a variável LIBS para especificar as
#bibliotecas necessárias durante a ligação do programa final, neste caso o SDL2
#e o SDL2_ttf.
LIBS = -lSDL2 -lSDL2_ttf
#SRCS = snake.c: Define a variável SRCS para conter o nome do arquivo fonte do
#jogo, que é snake.c.
SRCS = snake.c
#OBJS = $(SRCS:.c=.o): Define a variável OBJS para conter o nome dos arquivos de
# objeto a serem gerados a partir dos arquivos fonte. Ele substitui a extensão .
#c por .o nos arquivos listados em SRCS.
OBJS = $(SRCS:.c=.o)
#MAIN = snake: Define a variável MAIN para conter o nome do programa principal,
#que é snake.
MAIN = snake

#$(MAIN): $(OBJS): Esta linha especifica a regra para construir o programa 
#principal. Ele depende dos arquivos de objeto listados em OBJS.
$(MAIN): $(OBJS)
#$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS): Esta linha é a receita 
#para construir o programa principal. Ele usa o compilador ($(CC)), as flags de 
#compilação ($(CFLAGS)), os diretórios de inclusão ($(INCLUDES)), os arquivos de
#objeto ($(OBJS)) e as bibliotecas necessárias ($(LIBS)).
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)
#.c.o:: Esta linha especifica uma regra para construir arquivos de objeto .o a 
#partir de arquivos de origem .c.
.c.o:
#$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@: Esta linha é a receita para construir 
#um arquivo de objeto .o a partir de um arquivo de origem .c. O $< é uma 
#variável automática que representa o arquivo de origem e o $@ representa o 
#arquivo de objeto a ser gerado.
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
#clean:: Esta linha especifica uma regra chamada clean, que é usada para limpar 
#arquivos temporários e o executável final.
clean:
#$(RM) *.o *~ $(MAIN): Esta linha é a receita para limpar arquivos temporários e
#o executável final. O $(RM) é uma variável automática que representa o comando
#de remoção, e *.o *~ $(MAIN) são os arquivos a serem removidos. *.o são
#arquivos de objeto, *~ são arquivos temporários e $(MAIN) é o executável final.
	$(RM) *.o *~ $(MAIN)