objects = cpuUsage.o cpuUsage_test.o
main: $(objects)
	gcc -g $(objects) -o exe.o
	rm -rf $(objects)

cpuUsage_test.o: cpuUsage.o cpuUsage_test.c
	gcc -lpthread -c cpuUsage_test.c

cpuUsage.o: cpuUsage.h cpuUsage.c
	gcc -lpthread -c cpuUsage.c

clean:
	rm -rf exe.o
