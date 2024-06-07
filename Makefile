EXEC = projeto
CC = gcc
SRCS = projetosomain.c projetoso.c
CFLAGS = -Wall -pthread

all: $(EXEC)

$(EXEC) : $(SRCS)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRCS)

clean:
	rm -f $(EXEC)
