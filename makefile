all: virusDetector

virusDetector: main.o
	gcc -g -Wall -ansi -o virusDetector main.o

main.o: main.c
	gcc -g -Wall -c -ansi -o main.o main.c 
	
clean:
	rm -f *.o virusDetector 
