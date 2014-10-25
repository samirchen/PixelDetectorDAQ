objects = tiff_util.o tiff_util_test.o
main: $(objects)
	gcc -g $(objects) -o exe.o
	rm -rf $(objects)

tiff_util_test.o: tiff_util.o tiff_util_test.c
	gcc -c tiff_util_test.c

tiff_util.o: tiff_util.h tiff_util.c
	gcc -c tiff_util.c

clean:
	rm -rf exe.o
