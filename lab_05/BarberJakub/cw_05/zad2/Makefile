SRC_FILE = main.c 
FLAGS = -Wall

all :
	gcc $(FLAGS) $(SRC_FILE) -o main


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
	rm -f main 