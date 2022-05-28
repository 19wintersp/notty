#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define EXIT_ERROR(p) { perror(p "() failed"); exit(EXIT_FAILURE); }

int posix_openpt(int);
char* ptsname(int);
int grantpt(int);
int unlockpt(int);

typedef struct {
	int pty;
	pid_t pid;
} Child;

int run(char* argv[]);
Child launch(char* argv[]);

int main(int argc, char* argv[]) {
	char* argv0 = argc > 0 ? argv[0] : "notty";

	if (argc < 2) goto usage;
	if (argc > 2 && argv[1][0] == '-' && strcmp(argv[1], "--") != 0) goto usage;

	if (
		strcmp(argv[1], "--help") == 0 ||
		strcmp(argv[1], "-h") == 0
	) {
		printf("Usage: %s [OPTION]... [--] COMMAND [ARG]...\n", argv0);
		puts("Run COMMAND with ARGS with a pseudoterminal.");

		puts("");

		puts("-h, --help     display this help and exit");
		puts("-v, --version  display version information");

		puts("");

		puts("GNU GPL 3.0 license");
		puts("This is free software: you may change and share it.");
		puts("There is no warranty, to the extent permitted by law.");
		
		return EXIT_SUCCESS;
	}

	if (
		strcmp(argv[1], "--version") == 0 ||
		strcmp(argv[1], "-v") == 0
	) {
		puts("NoTTY 0.1.0");
		puts("Copyright (c) 2022 Patrick Winters");

#ifdef __DATE__
		puts("Compiled on " __DATE__);
#endif

		return EXIT_SUCCESS;
	}

	int first = 1;
	if (strcmp(argv[1], "--") == 0) first = 2;

	if (argc - first == 0) goto usage;
	if (argv[1][0] == '-' && first == 1) goto usage;

	return run(argv + first);

usage:
	fprintf(stderr, "Usage: %s [OPTION]... [--] COMMAND [ARG]...\n", argv0);
	fprintf(stderr, "Try '%s --help' for more information.\n", argv0);

	return EXIT_FAILURE;
}

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
