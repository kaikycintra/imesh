imesh: src/imesh.c
	gcc -Wall -o imesh src/imesh.c -lreadline

ep1: src/ep1.c
	gcc -Wall -o ep1 src/ep1.c

all: imesh ep1

clean:
	rm imesh
	rm ep1