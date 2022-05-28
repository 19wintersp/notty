#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

typedef struct {
	int pty;
	pid_t pid;
} Child;

int wait(pid_t pid);
Child launch(char* argv[]);

#endif // ifndef PROCESS_H
