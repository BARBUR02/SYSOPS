CLEAN_EXTENSIONS = o a so exe
CFLAGS = -Wall
SRC_FILES = interface
LIBNAME = libwordcounter


all : 
	make lib_static
	make lib_shared
	make clean_only_object

lib_static : $(SRC_FILES).c
	gcc $(CFLAGS) -c $(SRC_FILES).c
	ar cr $(LIBNAME).a $(SRC_FILES).o

lib_shared : $(SRC_FILES).c
	gcc $(CFLAGS) -c -fPIC $(SRC_FILES).c
	gcc $(CFLAGS) -shared -o $(LIBNAME).so $(SRC_FILES).o

clean_only_object :
	rm *.o

clean:
	for file in $(CLEAN_EXTENSIONS); do rm -f *.$$file; done