EXEC = projeto
CC = gcc
SRCS = projetosomain.c projetoso.c
CFLAGS = -Wall -pthread

all: $(EXEC)

$(EXEC) : $(SRCS)
	$(CC) -o $(CFLAGS) $(EXEC) $(SRCS) 

clean:
	rm -f $(EXEC)

