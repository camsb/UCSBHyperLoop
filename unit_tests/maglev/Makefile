# State machine unit tests

CC  = gcc
RM = rm

# CFLAGS  = -g -c -Wall -o $@
CFLAGS  = -O -c -Wall -o $@

maglev_state_machine: main.o maglev_state_machine.o qepn.o 
	$(CC) -o maglev_state_machine main.o maglev_state_machine.o qepn.o

main.o: main.c
	$(CC) $(CFLAGS) $<

maglev_state_machine.o: maglev_state_machine.c
	$(CC) $(CFLAGS) $<

qepn.o: qepn.c
	$(CC) $(CFLAGS) $<

clean:
	-$(RM) *.o maglev_state_machine



