CLFAGS=-std=c99 -Wall -Werror -Wextra -lm
CC=gcc
PROJ=proj3

.PHONY: clean ndebug

$(PROJ): $(PROJ).c
	$(CC) $(CLFAGS) $(PROJ).c -o $(PROJ)

ndebug: $(PROJ).c
	$(CC) $(CLFAGS) -DNDEBUG $(PROJ).c -o $(PROJ)

clean:
	rm -f $(PROJ)
