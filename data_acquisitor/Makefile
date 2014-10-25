objects = cpuUsage.o data_acquisitor.o
main: $(objects)
	gcc -g -pthread $(objects) -o exe.o 
	rm -rf $(objects)

data_acquisitor.o: cpuUsage.o data_acquisitor.c
	gcc -c data_acquisitor.c

cpuUsage.o: ../common/cpuUsage.h ../common/cpuUsage.c
	gcc -c ../common/cpuUsage.c

clean:
	rm -rf $(objects) 
