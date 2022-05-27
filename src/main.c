#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
	char* argv0 = argc > 0 ? argv[0] : "notty";

	if (argc < 2) goto usage;
	if (argc > 2 && argv[1][0] == '-' && strcmp(argv[1], "--") != 0) goto usage;

	if (
		strcmp(argv[1], "--help") == 0 ||
		strcmp(argv[1], "-h") == 0
	) {
		printf("Usage: %s [OPTION]... [--] [COMMAND [ARG]...]\n", argv0);
		puts("Run COMMAND with ARGS with a pseudoterminal.");

		puts("");

		puts("-h, --help     display this help and exit");
		puts("-v, --version  display version information");

		puts("");

		puts("GNU GPL 3.0 license");
		puts("This is free software: you may change and share it.");
		puts("There is no warranty, to the extent permitted by law.");
		
		return argc < 2 ? EXIT_FAILURE : EXIT_SUCCESS;
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

	if (argv[1][0] == '-' && strcmp(argv[1], "--") != 0) goto usage;

	abort();

usage:
	fprintf(stderr, "Usage: %s [OPTION]... [--] [COMMAND [ARG]...]\n", argv0);
	fprintf(stderr, "Try '%s --help' for more information.", argv0);

	return EXIT_FAILURE;
}
