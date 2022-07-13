CC     = cc
CFLAGS = -Wall -Wextra -Wshadow -pedantic -O2 -ansi
EXEC   = cntax
SRC    = cntax.c

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^
