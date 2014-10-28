objects = tiff_util.o image_util.o tiff_util_test.o
main: $(objects)
	g++ -g $(objects) -o exe.o
	rm -rf $(objects)

tiff_util_test.o: tiff_util.o image_util.o tiff_util_test.c
	g++ -c tiff_util_test.c

image_util.o: image_util.h image_util.c
	gcc -c image_util.c

tiff_util.o: tiff_util.h tiff_util.c
	gcc -c tiff_util.c

clean:
	rm -rf exe.o
