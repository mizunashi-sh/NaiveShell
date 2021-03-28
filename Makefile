proc: nsh.o
	cc -o nsh nsh.o
nsh.o: nsh.c
	cc -o nsh.o -c nsh.c
clean:
	rm *.o nsh