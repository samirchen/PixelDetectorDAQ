objects = dieWithError.o cpuUsage.o tiff_util.o
main: $(objects)
	g++ -g $(objects) -o exe.o
	rm -rf $(objects)

tiff_util.o: cpuUsage.o tiff_util.cpp
	g++ -c tiff_util.cpp

cpuUsage.o: dieWithError.o ../common/cpuUsage.h ../common/cpuUsage.c
	g++ -c ../common/cpuUsage.c

dieWithError.o: ../common/dieWithError.h ../common/dieWithError.c
	g++ -c ../common/dieWithError.c

clean:
	rm -rf exe.o
