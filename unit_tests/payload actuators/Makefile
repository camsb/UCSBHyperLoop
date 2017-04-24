# State machine unit tests

CC  = gcc
RM = rm

# CFLAGS  = -g -c -Wall -o $@
CFLAGS  = -O -c -Wall -o $@

payload_actuator_sm: main.o payload_actuator_sm.o qepn.o 
	$(CC) -o payload_actuator_sm main.o payload_actuator_sm.o qepn.o

main.o: main.c
	$(CC) $(CFLAGS) $<

payload_actuator_sm.o: payload_actuator_sm.c
	$(CC) $(CFLAGS) $<

qepn.o: qepn.c
	$(CC) $(CFLAGS) $<

clean:
	-$(RM) *.o payload_actuator_sm



