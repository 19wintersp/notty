#define _POSIX_SOURCE

#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "run.h"
#include "process.h"

int run(char* argv[]) {
	Child child = launch(argv);

	char buf[256];

	for (;;) {
		struct pollfd fds[2] = {
			{ child.pty, POLLIN, 0 },
			{ fileno(stdin), POLLIN, 0 }
		};

		if (poll(fds, 2, 5) < 0) {
			kill(child.pid, SIGTERM);

			perror("poll() failed");
			return EXIT_FAILURE;
		}

		if (fds[0].revents & POLLIN) {
			size_t r = read(child.pty, buf, sizeof(buf));
			size_t w = fwrite(buf, 1, r, stdout);

			if (w != r) perror("write() failed");
		}

		if (fds[1].revents & POLLIN) {
			size_t r = fread(buf, 1, sizeof(buf), stdin);
			size_t w = write(child.pty, buf, r);

			if (w != r) perror("write() failed");
		}

		int status;
		int result = waitpid(child.pid, &status, WNOHANG);

		if (result < 0) {
			perror("waitpid() failed");
		} else if (result > 0) {
			if (WIFEXITED(status)) return WEXITSTATUS(status);
			if (WIFSIGNALED(status)) return WTERMSIG(status) | 0x80;
			if (WIFSTOPPED(status)) return WSTOPSIG(status) | 0x80;

			return EXIT_FAILURE;
		}
	}
}
