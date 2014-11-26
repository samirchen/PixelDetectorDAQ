objects = cpuUsage.o tiff_util.o data_acquisitor.o
main: $(objects)
	gcc -g -pthread $(objects) -o exe.o 
	rm -rf $(objects)

data_acquisitor.o: cpuUsage.o tiff_util.o data_acquisitor.c
	gcc -c data_acquisitor.c

tiff_util.o: ../tiff_util/tiff_util.h ../tiff_util/tiff_util.c
	gcc -c ../tiff_util/tiff_util.c

cpuUsage.o: ../common/cpuUsage.h ../common/cpuUsage.c
	gcc -c ../common/cpuUsage.c

clean:
	rm -rf exe.o
