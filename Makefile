ALL=cs amdahl

all: $(ALL)

cs: cs_test.o task.o misc.o queue.o cal.o
	$(CC) $^ -g -lpthread -o $@

amdahl: amdahl.o task.o misc.o cal.o
	$(CC) $^ -g -lpthread -o $@

cs_test.o: cs_test.c misc.h task.h queue.h
task.o: task.c task.h misc.h
misc.o: misc.c misc.h
cal.o: cal.c
amdahl.o: amdahl.c misc.h task.h

%.o: %.c
	$(CC) -c -g -Wall -D_GNU_SOURCE $< -o $@

.PHONY: clean cleanall tags

clean:
	rm -f *.o $(ALL) tags cscope.out

cleanall:
	rm -f *.o $(ALL) tags cscope.out
	sudo rm -f perf perf.data

tags:
	ctags -R --extra=f
	cscope -Rb
