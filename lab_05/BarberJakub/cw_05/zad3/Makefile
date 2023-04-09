SRC_FILE = main.c 
INTEGRAL = integral_util.c
FLAGS = -Wall

all :
	make main
	make integral_util

main:
	gcc $(FLAGS) $(SRC_FILE) -o main 

integral_util:
	gcc $(FLAGS) $(INTEGRAL) -o integral_util

test: all
	rm -f report.txt
	for n in  3 10 20; do \
		for eps in  0.000001 0.00000001 0.000000001; do \
			echo "\nResult for $$n iterations and epsilon = $$eps :" >> report.txt ; \
			./main $$eps  $$n >> report.txt; \
		done ; \
	done

clean_report:
	rm -f report.txt

clean:
	rm -f main integral_util