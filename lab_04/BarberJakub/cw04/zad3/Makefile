SRC_FILES = catcher.c 
CFLAGS = -Wall

all: 
	make catcher
	make sender

catcher: catcher.c 
	gcc $(CFLAGS) catcher.c -o catcher.exe

sender: sender.c 
	gcc $(CFLAGS) sender.c -o sender.exe

clean:
	rm -f catcher.exe sender.exe