mandelGUI: mandelGUI.o mandelCore.o mandelThreaded.o 
	gcc  $(CSWITCHES)  -Wall mandelGUI.o mandelCore.o mandelThreaded.o -o mandelGUI -lpthread -lX11
	
mandelGUI.o: mandelGUI.c mandelCore.h mandelThreaded.h 
	gcc  $(CSWITCHES) -Wall -c mandelGUI.c 

mandelCore.o: mandelCore.c mandelCore.h
	gcc -Wall -c mandelCore.c 

mandelThreaded.o: mandelThreaded.c mandelThreaded.h
	gcc -Wall -c mandelThreaded.c -lpthread	

