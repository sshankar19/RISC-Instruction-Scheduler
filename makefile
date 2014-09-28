all: scheduler	
scheduler: scheduler.c schedutils.h setup.c dependencies.c	
	gcc -Wall -g -o scheduler scheduler.c dependencies.c setup.c schedutils.h queue.c
clean:
	rm -f scheduler
