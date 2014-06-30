objects = dieWithError.o cpuUsage.o tiff_util.o
main: $(objects)
	gcc -g $(objects) -o exe.o
	rm -rf $(objects)

tiff_util.o: cpuUsage.o tiff_util.c
	gcc -c tiff_util.c

cpuUsage.o: dieWithError.o ../common/cpuUsage.h ../common/cpuUsage.c
	gcc -c ../common/cpuUsage.c

dieWithError.o: ../common/dieWithError.h ../common/dieWithError.c
	gcc -c ../common/dieWithError.c

clean:
	rm -rf exe.o
