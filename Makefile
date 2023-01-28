LIBEDIT= -ledit
LIBS= -lm $(LIBEDIT)

default: krypty

krypty: main.c
	$(CC) main.c mpc.c -o krypty $(LIBS)

clean:
	rm krypty *.o
