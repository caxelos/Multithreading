gcc -Wall -g mandelGUI.c mandelCore.c mandelThreaded.c  -o mandelGUI -L /usr/X11R6/lib -lX11 -pthread