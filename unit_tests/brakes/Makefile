# State machine unit tests

CC  = gcc
RM = rm

# CFLAGS  = -g -c -Wall -o $@
CFLAGS  = -O -c -Wall -o $@

braking_state_machine: main.o braking_state_machine.o qepn.o 
	$(CC) -o braking_state_machine main.o braking_state_machine.o qepn.o

main.o: main.c
	$(CC) $(CFLAGS) $<

braking_state_machine.o: braking_state_machine.c
	$(CC) $(CFLAGS) $<

qepn.o: qepn.c
	$(CC) $(CFLAGS) $<

clean:
	-$(RM) *.o braking_state_machine



