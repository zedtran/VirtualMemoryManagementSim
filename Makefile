CC=gcc
CFLAGS=-I.
DEPS = vmtypes.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

vm_sim: vmtypes.o vm_sim.o
	gcc -o vm_sim vm_sim.o vmtypes.o -I.
