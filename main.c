#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <stdnoreturn.h>

const char version[] = "0.1";
static const size_t SET_DEFAULT_SIZE = 32;

noreturn inline static void _panic(const char *file_name, const char *function_name, int line_number, const char *msg) {
	fprintf(stderr, "PANIC: %s@%s:%d\t%s\n", function_name, file_name,line_number, msg);
}

#define panic(msg) \
do { \
	_panic(__FILE__, __func__, __LINE__, msg); \
	exit(EXIT_FAILURE);
} while (0)

typedef struct set_t {
	//maps a character value to a flag to state whether it has been encountered
	//	0 -	character, represented by index, has not been encountered
	//	1	-	character, represented by index, has been encountered before
	bool set_map[255];
	size_t count;
	size_t capacity;
	char *set;
} set_t;

static set_t g_original;
static set_t g_replacement;

static inline void set_init(set_t* this) {
	memset(this->set_map, 0, sizeof(this->set_map));
	this->count = 0;
	this->capacity = SET_DEFAULT_SIZE;
	this->set = malloc(this->capacity);
}

static void set_insert_str(set_t* this, const char *p_list, const size_t list_size) {
		for (int i = 0; i < list_size; i++) {
			char next_char = p_list[i];
			//check if next_hcar has been in the list yet
			if (this->set_map[next_char] == false) {
				//it hasn't, mark it visited
				this->set_map[next_char] = true;

				//check if a the set needs to grow
				this->count++;
				if (this->count >= this->capacity) {
					char *old_set = this->set;
					size_t new_capacity = this->capacity * 2;

					//check for overflow
					if (new_capacity > this->capacity) {
						this->capacity = new_capacity;
					} else {
						panic("ran out of memory");
					}

					this->set = realloc(this->set, this->capacity);
					if (this->set == NULL) {
						panic("realloc failed");
					}
				}

				//do insertion
				this->set[this->count] = next_char;
			}
		}
}

static inline bool set_includes_char(const set_t *this, const char query) {
	return this->set_map[query];
}

static char set_get(const set_t *this, size_t index) {

}

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
		set_init(&g_original);
		set_init(&g_replacement);
		set_insert_str(&g_original, argv[1], strlen(argv[1]));
		set_insert_str(&g_replacement, argv[2], strlen(argv[2]));
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
