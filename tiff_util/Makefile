objects = tiff_util.o image_util.o tiff_util_test.o
main: $(objects)
	g++ -g $(objects) -o exe.o
	rm -rf $(objects)

tiff_util_test.o: tiff_util.o image_util.o tiff_util_test.cpp
	g++ -c tiff_util_test.cpp

image_util.o: image_util.h image_util.cpp
	g++ -c image_util.cpp

tiff_util.o: tiff_util.h tiff_util.c
	gcc -c tiff_util.c

clean:
	rm -rf exe.o
