# State machine unit tests

CC  = gcc
RM = rm

# CFLAGS  = -g -c -Wall -o $@
CFLAGS  = -O -c -Wall -o $@

service_propulsion_sm: main.o service_propulsion_sm.o qepn.o 
	$(CC) -o service_propulsion_sm main.o service_propulsion_sm.o qepn.o

main.o: main.c
	$(CC) $(CFLAGS) $<

service_propulsion_sm.o: service_propulsion_sm.c
	$(CC) $(CFLAGS) $<

qepn.o: qepn.c
	$(CC) $(CFLAGS) $<

clean:
	-$(RM) *.o service_propulsion_sm



