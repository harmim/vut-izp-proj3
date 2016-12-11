CLFAGS=-std=c99 -Wall -Werror -Wextra -lm
CC=gcc
PROJ=proj3

.PHONY: clean ndebug tests

$(PROJ): $(PROJ).c
	$(CC) $(CLFAGS) $(PROJ).c -o $(PROJ)

ndebug: $(PROJ).c
	$(CC) $(CLFAGS) -DNDEBUG $(PROJ).c -o $(PROJ)

tests: tests/test.sh
	cd tests && ./test.sh -m

clean: tests/test.sh
	rm -f $(PROJ)
	cd tests && ./test.sh -c
