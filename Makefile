cs: cs_test.o task.o misc.o queue.o
	$(CC) $^ -g -lpthread -o $@

cs_test.o: cs_test.c misc.h task.h queue.h
task.o: task.c task.h misc.h
misc.o: misc.c misc.h

%.o: %.c
	$(CC) -c -g -Wall $< -o $@

.PHONY: clean

clean:
	rm -f *.o cs
