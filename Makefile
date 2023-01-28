LIBS= -ledit

default: krypty

krypty:
	$(CC) main.c mpc.c -o krypty $(LIBS)

clean:
	rm krypty *.o
