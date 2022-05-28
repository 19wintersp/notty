#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "process.h"

#define EXIT_ERROR(p) { perror(p "() failed"); exit(EXIT_FAILURE); }

Child launch(char* argv[]) {
	int pty_master = posix_openpt(O_NOCTTY | O_RDWR);
	if (pty_master < 0) EXIT_ERROR("openpt");

	if (grantpt(pty_master) < 0) EXIT_ERROR("grantpt");
	if (unlockpt(pty_master) < 0) EXIT_ERROR("unlockpt");

	char* pty_slave_path = ptsname(pty_master);
	if (pty_slave_path == NULL) EXIT_ERROR("ptsname");

	int pty_slave = open(pty_slave_path, O_RDWR);
	if (pty_slave < 0) EXIT_ERROR("pty open");

	int fork_result = fork();
	if (fork_result < 0) EXIT_ERROR("fork");

	if (fork_result == 0) {
		int pty_current = open("/dev/tty", O_NOCTTY | O_RDWR);
		if (pty_current != -1) {
			int result = ioctl(pty_current, TIOCNOTTY, 0);
			if (result < 0) EXIT_ERROR("ioctl");

			close(pty_current);
		}

		if (setsid() < 0) EXIT_ERROR("setsid");
		if (ioctl(pty_slave, TIOCSCTTY, 0) < 0) EXIT_ERROR("ioctl");

		if (dup2(pty_slave, 0) < 0) EXIT_ERROR("dup2");
		if (dup2(pty_slave, 1) < 0) EXIT_ERROR("dup2");
		if (dup2(pty_slave, 2) < 0) EXIT_ERROR("dup2");

		close(pty_current);

		if (execvp(argv[0], argv) < 0) EXIT_ERROR("exec");

		abort();
	}

	return (Child) { pty_master, fork_result };
}
