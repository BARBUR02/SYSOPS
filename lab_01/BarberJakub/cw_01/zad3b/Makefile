CLEAN_EXTENSIONS = o a so exe
CFLAGS = -Wall
SRC_FILES = interface
LIBNAME = libwordcounter
MAIN = main.c
REPORT_FILE = report.txt

all : 
	make lib_static
	make lib_shared
	make exec_static
	make exec_shared
	make exec_dynamic
	make clean_only_object


lib_static : $(SRC_FILES).c
	gcc $(CFLAGS) $(OPT_FLAG) -c $(SRC_FILES).c
	ar cr $(LIBNAME).a $(SRC_FILES).o

lib_shared : $(SRC_FILES).c
	gcc $(CFLAGS) $(OPT_FLAG) -c -fPIC $(SRC_FILES).c
	gcc $(CFLAGS) $(OPT_FLAG) -shared -o $(LIBNAME).so $(SRC_FILES).o

exec_static : lib_static
	gcc $(CFLAGS) $(OPT_FLAG) $(MAIN) -o wc_static.exe -L. $(LIBNAME).a

exec_shared : lib_shared
	gcc $(CFLAGS) $(OPT_FLAG) $(MAIN) -L. -Wl,-rpath=. $(LIBNAME).so -o wc_shared.exe

exec_dynamic : lib_shared
	gcc $(CFLAGS) $(OPT_FLAG) $(MAIN)  $(LIBNAME).so  -D DDL_ACTIVE -o wc_dynamic.exe

clean_only_object :
	rm *.o

clean_results :
	rm ./results/*

merge_to_report :
	> $(REPORT_FILE)
	for file in ./results/*; do echo -e "================\nfile : $$file ...\n" >> $(REPORT_FILE) ; cat $$file >> $(REPORT_FILE) ; done

test_optimization:
	make test OPT_FLAG="-O"
	make test OPT_FLAG="-O1"
	make test OPT_FLAG="-O2"
	make test OPT_FLAG="-O0"
	make test OPT_FLAG="-Os"

test : exec_static exec_shared exec_dynamic
	mkdir -p results/
	./wc_static.exe < test.txt > results/results_static_$(OPT_FLAG).txt
	./wc_shared.exe < test.txt > results/results_shared_$(OPT_FLAG).txt
	./wc_dynamic.exe < test.txt > results/results_dynamic_$(OPT_FLAG).txt

clean:
	for file in $(CLEAN_EXTENSIONS); do rm -f *.$$file; done