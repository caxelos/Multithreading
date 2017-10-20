askisi1: mypipe.o askisi1.o 
	gcc mypipe.o askisi1.o -o askisi1 -lpthread 
	
mypipe.o: mypipe.c mypipe.h
	gcc -Wall -c mypipe.c 

askisi1.o: askisi1.c mypipe.h
	gcc -Wall -c askisi1.c -lpthread
	

