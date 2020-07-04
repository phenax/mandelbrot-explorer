# mandle - Suckless hot key daemon
# See LICENSE file for copyright and license details.

include config.mk

SRC = mandle.c
OBJ = ${SRC:.c=.o}

all: clean options mandle

options:
	@echo mandle build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

mandle: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f mandle ${OBJ}

.PHONY: all options clean
