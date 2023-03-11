CLEAN_EXTENSIONS = o a so exe
CFLAGS = -Wall
SRC_FILES = interface
LIBNAME = libwordcounter
MAIN = main.c

all : 
	make lib_static
	make lib_shared
	make exec_static
	make exec_shared
	make exec_dynamic
	make clean_only_object


lib_static : $(SRC_FILES).c
	gcc $(CFLAGS) -c $(SRC_FILES).c
	ar cr $(LIBNAME).a $(SRC_FILES).o

lib_shared : $(SRC_FILES).c
	gcc $(CFLAGS) -c -fPIC $(SRC_FILES).c
	gcc $(CFLAGS) -shared -o $(LIBNAME).so $(SRC_FILES).o

exec_static : $(SRC_FILES).c $(SRC_FILES).h
	gcc $(CFLAGS) $(MAIN) -o wc_static.exe -L. $(LIBNAME).a

exec_shared : $(SRC_FILES).c $(SRC_FILES).h
	gcc $(CFLAGS) $(MAIN) -L. -Wl,-rpath=. $(LIBNAME).so -o wc_shared.exe

exec_dynamic : $(SRC_FILES).c $(SRC_FILES).h
	gcc $(CFLAGS) $(MAIN)  $(LIBNAME).so  -D DDL_ACTIVE -o wc_dynamic.exe

clean_only_object :
	rm *.o

clean:
	for file in $(CLEAN_EXTENSIONS); do rm -f *.$$file; done