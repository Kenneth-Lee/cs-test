cs-test: cs_test.o task.o misc.o

cs_test.o: cs_test.c
task.o: task.c
misc.o: task.c

%.o: %.c
	$(CC) -c -Wall $< -o $@

.PHONY: clean

clean:
	rm -f *.o cs-test
