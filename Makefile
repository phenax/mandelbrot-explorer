# mandel - Mandelbrot explorer
# See LICENSE file for copyright and license details.

include config.mk

SRC = mandel.c complex.c
OBJ = ${SRC:.c=.o}

all: clean options mandel

options:
	@echo mandel build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

mandel: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f mandel ${OBJ}

.PHONY: all options clean
