stringscout: stringscout.o
	gcc -O3 -o stringscout stringscout.o

stringscout.o: stringscout.c
	gcc -c stringscout.c

clean:
	rm -f stringscout stringscout.o