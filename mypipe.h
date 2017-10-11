// ./askisi1 10 input.txt output.txt

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>

void pipe_close(void);
int pipe_read(void);
void pipe_write(char c);
void pipe_init(int size);
