CC = gcc
CCFLAGS = -g -Wall -std=c99 -lm  -I.
RM = rm -f
Y86EMUL = y86emul
all: y86emul

#Build an executable program y86emul from y86emul.c
y86emul: y86emul.c
		$(CC) $(CCFLAGS) -o y86emul y86emul.c

clean:
		$(RM) $(Y86EMUL)
