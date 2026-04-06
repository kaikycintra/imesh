imesh: src/imesh.c
	gcc -Wall -o imesh src/imesh.c -lreadline

clean:
	rm imesh