#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define BUF_SIZE 10

void pipe_close(void);
int pipe_read(void);
void pipe_write(char c);
void pipe_init(int size);
