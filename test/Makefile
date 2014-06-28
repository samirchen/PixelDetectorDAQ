objects = test.o
main: $(objects)
	g++ -g $(objects) -o exe.o
	rm -rf $(objects)

test.o: test.c
	gcc -c test.c

clean:
	rm -rf exe.o
