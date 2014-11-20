objects = cpuUsage.o tiff_util.o image_util.o data_generator.o
main: $(objects)
	g++ -g $(objects) -o exe.o 
	rm -rf $(objects)

data_generator.o: cpuUsage.o tiff_util.o image_util.o data_generator.cpp
	g++ -c data_generator.cpp

image_util.o: ../tiff_util/image_util.h ../tiff_util/image_util.cpp
	g++ -c ../tiff_util/image_util.cpp

tiff_util.o: ../tiff_util/tiff_util.h ../tiff_util/tiff_util.c
	gcc -c ../tiff_util/tiff_util.c

cpuUsage.o: ../common/cpuUsage.h ../common/cpuUsage.c
	gcc -c ../common/cpuUsage.c

clean:
	rm -rf exe.o
