.SILENT:
output: main.o functionality.o 
	gcc main.o functionality.o -o syndi

main.o: main.c
	gcc -c main.c

functionality.o: functionality.c funnies.h
	gcc -c functionality.c

clean: 
	rm -f *.o syndi
	
reload:
	rm -f *.o syndi
	make
	