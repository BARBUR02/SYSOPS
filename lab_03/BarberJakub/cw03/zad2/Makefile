SRC_FILE = main.c
OUT_FILE = main.exe
CLEAN_EXTENSIONS = o exe 
FLAGS = -Wall

all:
	gcc $(FLAGS) $(SRC_FILE) -o $(OUT_FILE)


clean : 
	for file in $(CLEAN_EXTENSIONS); do rm -f *.$$file; done