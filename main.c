#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

const char version[] = "0.1";
static char g_original_char;
static char g_replacement_char;

noreturn void print_usage(const char *program_name) {
	printf("Usage: %s [original] [replacement]\n", program_name);
	printf("reads from STDIN and writes to STDOUT replacing any character\n");
	printf("equal to [original] and replacing it with [replacement].\n");
	printf("All other characters are output unaltered\n");
	exit(EXIT_SUCCESS);
}

void check_args(int argc, const char *argv[]) {
	if (argc != 3) {
		printf("%s requires exactly 2 arguments\n\n", argv[0]);
		print_usage(argv[0]);
	} else {
		g_original_char = argv[1][0];
		g_replacement_char = argv[2][0];
	}
}

noreturn void tr() {
	char buffer;

	buffer = fgetc(stdin);
	while (!feof(stdin)) {
		if (buffer == g_original_char) {
			fputc(g_replacement_char, stdout);
		} else {
			fputc(buffer, stdout);
		}
		buffer = fgetc(stdin);
	}
	exit(EXIT_SUCCESS);
}

int main (int argc, char *argv[]) {
	check_args(argc, (const char **) argv);
	tr();
	exit(EXIT_FAILURE);
}
