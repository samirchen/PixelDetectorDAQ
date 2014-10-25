objects = cpuUsage.o data_generator.o
main: $(objects)
	g++ -g $(objects) -o exe.o 
	rm -rf $(objects)

data_generator.o: cpuUsage.o data_generator.cpp
	g++ -c data_generator.cpp

cpuUsage.o: ../common/cpuUsage.h ../common/cpuUsage.c
	g++ -c ../common/cpuUsage.c

clean:
	rm -rf exe.o
