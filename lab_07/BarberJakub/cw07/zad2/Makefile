FLAGS = -Wall
OTHER=sem_utils.c memory_utils.c queue_util.c

all: 
	make main 

main: barber customer
	gcc $(FLAGS) main.c $(OTHER) -lrt -lpthread -o main 

barber:
	gcc $(FLAGS) barber.c $(OTHER) -lrt -lpthread -o barber 

customer:
	gcc $(FLAGS) customer.c  $(OTHER) -lrt  -lpthread -o customer

clean:
	rm -rf barber customer main