SRC_FILE = main.c
CFLAGS = -Wall

all:
	make fork 
	make exec 

fork:
	gcc $(CFLAGS) $(SRC_FILE) -o fork.exe

exec:
	gcc $(CFLAGS) $(SRC_FILE) -o exec.exe -D EXEC

test:
	make all 
	echo "IGNORE" >> fork.txt
	./fork.exe ignore >> fork.txt
	
	echo "HANDLER" >> fork.txt
	./fork.exe handler >> fork.txt

	echo "PENDING" >> fork.txt
	./fork.exe pending >> fork.txt

	echo "MASK" >> fork.txt
	./fork.exe mask >> fork.txt


	echo "IGNORE" >> exec.txt
	./exec.exe ignore >> exec.txt
	
	echo "PENDING" >> exec.txt
	./exec.exe pending >> exec.txt

	echo "MASK" >> exec.txt
	./exec.exe mask >> exec.txt

clear_reports:
	rm -f fork.txt exec.txt

clean:
	rm -f fork.exe exec.exe