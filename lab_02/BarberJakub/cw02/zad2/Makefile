MAIN= main.c
FLAGS = -Wall
SRC_FILES = rev_lib.c main.c
CLEAN_EXTENTIONS = o a so exe
OBJECT_EXTENTIONS = o
TEST_DATA = test_data.txt

all :  
	make rev_lib_char
	make rev_lib_byte
rev_lib_char:
	gcc ${FLAGS} ${SRC_FILES} -o rev_char.exe 

make rev_lib_byte:
	gcc ${FLAGS} ${SRC_FILES}  -D BYTE  -o rev_byte.exe

make_res: rev_lib_char rev_lib_byte
	for i in 100 200 300 400 1000 20000 10000 100000 1000000; do \
		echo "\nLength: $$i" >> pomiar_char_zad_2.txt; \
		echo "\nLength: $$i" >> pomiar_byte_zad_2.txt; \
		head -c $$i < /dev/urandom > ${TEST_DATA} ; \
		./rev_char.exe  ${TEST_DATA} lib_buff >> pomiar_char_zad_2.txt ; \
		./rev_byte.exe  ${TEST_DATA} sys_buff >> pomiar_byte_zad_2.txt ; \
	done
	rm ${TEST_DATA} lib_buff sys_buff

clean_obj:	
	for file in $(OBJECT_EXTENTIONS); do rm -f *.$$file; done

clean_reports:
	rm pomiar_char_zad_2.txt pomiar_byte_zad_2.txt
 
clean:
	for file in $(CLEAN_EXTENTIONS); do rm -f *.$$file; done
	