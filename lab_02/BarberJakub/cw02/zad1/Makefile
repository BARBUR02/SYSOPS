MAIN= main.c
FLAGS = -Wall
SRC_FILES = lib_tr.c sys_tr.c main.c
CLEAN_EXTENTIONS = o a so exe
OBJECT_EXTENTIONS = o
TEST_DATA = test_data.txt

all : 
	make comp_sys 
	make comp_lib 
	make clean_obj
comp_sys:
	gcc ${FLAGS} ${SRC_FILES} -o tr_sys.exe

comp_lib:
	gcc ${FLAGS} ${SRC_FILES} -o tr_lib.exe

make_res: comp_lib comp_sys

	for i in 100 1000 2000 3000 4000 5000 10000 100000 1000000 10000000; do \
		echo "\nLength: $$i" >> pomiar_lib_zad_1.txt; \
		echo "\nLength: $$i" >> pomiar_sys_zad_1.txt; \
		head -c $$i < /dev/urandom > ${TEST_DATA} ; \
		./tr_lib.exe x d ${TEST_DATA} lib_buff >> pomiar_lib_zad_1.txt ; \
		./tr_sys.exe x d ${TEST_DATA} sys_buff >> pomiar_sys_zad_1.txt ; \
	done
	rm ${TEST_DATA} lib_buff sys_buff

clean_obj:	
	for file in $(OBJECT_EXTENTIONS); do rm -f *.$$file; done

clean_reports:
	rm pomiar_lib_zad_1.txt pomiar_sys_zad_1.txt
 
clean:
	for file in $(CLEAN_EXTENTIONS); do rm -f *.$$file; done
	