#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdnoreturn.h>

typedef struct multi_set {
	size_t count;
	size_t capacity;
	char *set;
} multi_set;

typedef struct unique_set {
	multi_set super;
	size_t set_map[256];
} unique_set;

typedef struct set_t {
	union {
		unique_set unique;
		multi_set multi;
	} *p_set;
	enum {
		SET_TYPE_UNIQUE,
		SET_TYPE_MULTI,
	} type;
	uint8_t __padding[4];
} set_t;


static const char g_version[] = "0.2";
static const size_t SET_DEFAULT_SIZE = 32;
static const size_t FIND_NPOS = ((size_t) -1);

static set_t g_original;
static set_t g_replacement;

noreturn inline static void _panic(const char *file_name, const char *function_name, int line_number, const char *msg) {
	fprintf(stderr, "PANIC: %s@%s:%d\t%s\n", function_name, file_name,line_number, msg);
	exit(EXIT_FAILURE);
}

#define panic(msg) \
do { \
	_panic(__FILE__, __func__, __LINE__, msg); \
} while (0)

static void set_init(set_t *this);
static void set_init_unique(set_t *this);
static void set_free(set_t *this);
static void set_insert_str(set_t* this, const char *p_list, const size_t list_size);
static inline size_t set_find_char(const set_t *this, const char query);
static char set_get(const set_t *this, size_t index);

noreturn void print_usage(const char *program_name);
noreturn void print_version(const char *program_name);
noreturn void tr(void);

void check_args(int argc, const char *argv[]);
void check_args(int argc, const char *argv[]) {
	if (argc != 3) {
		printf("%s requires exactly 2 arguments\n\n", argv[0]);
		print_usage(argv[0]);
	} else {
		set_init_unique(&g_original);
		set_init(&g_replacement);
		set_insert_str(&g_original, argv[1], strlen(argv[1]));
		set_insert_str(&g_replacement, argv[2], strlen(argv[2]));
	}
}

noreturn void tr() {
	int buffer;

	buffer = fgetc(stdin);
	while (!feof(stdin)) {
		size_t idx = set_find_char(&g_original, (char) buffer);
		if (idx != FIND_NPOS) {
			fputc(set_get(&g_replacement, idx), stdout);
		} else {
			fputc((char) buffer, stdout);
		}
		buffer = fgetc(stdin);
	}
	set_free(&g_original);
	set_free(&g_replacement);
	exit(EXIT_SUCCESS);
}

noreturn void print_usage(const char *program_name) {
	printf("Usage: %s [search set] [replacement set]\n", program_name);
	printf("reads from STDIN and writes to STDOUT replacing any character\n");
	printf("existing in [search set] and replacing it with [replacement set].\n");
	printf("All other characters are output unaltered\n");
	exit(EXIT_SUCCESS);
}

noreturn void print_version(const char *program_name) {
	printf("%s version %s  by Scott Geigel (2017)\n", program_name, g_version);
	exit(EXIT_SUCCESS);
}

int main (int argc, char *argv[]) {
	check_args(argc, (const char **) argv);
	tr();
}


static void set_init(set_t *this) {
	multi_set *p_set = malloc(sizeof(*this->p_set));

	if (p_set == NULL) {
		panic("failed to allocate set_t");
	}
	this->p_set = (void*) p_set;

	p_set->count = 0;
	p_set->capacity = SET_DEFAULT_SIZE;
	p_set->set = malloc(p_set->capacity);

	this->type = SET_TYPE_MULTI;
}

static void set_init_unique(set_t *this) {
	unique_set *p_set;
	set_init(this);
	p_set = &this->p_set->unique;
	this->type = SET_TYPE_UNIQUE;
	memset(p_set->set_map, -1, sizeof(p_set->set_map));
}

static void set_free(set_t *this) {
	free(this->p_set->multi.set);
	free(this->p_set);
}

static void set_insert_str(set_t* this, const char *p_list, const size_t list_size) {
	unique_set *unique = &this->p_set->unique;
	multi_set *multi = &this->p_set->multi;

	for (size_t i = 0; i < list_size; i++) {
		char next_char =  p_list[i];
		size_t lookup_idx = (size_t) p_list[i];

		if (this->type == SET_TYPE_UNIQUE) {
			//check if next_char has been in the list yet
			unique->set_map[lookup_idx] = multi->count;
		}

		//check if a the set needs to grow
		if (multi->count >= multi->capacity) {
			size_t new_capacity = multi->capacity * 2;

			//check for overflow
			if (new_capacity > multi->capacity) {
				multi->capacity = new_capacity;
			} else {
				panic("ran out of memory");
			}

			multi->set = realloc(multi->set, multi->capacity);
			if (multi->set == NULL) {
				panic("realloc failed");
			}
		}

		//do insertion
		multi->set[multi->count++] = next_char;
	}
}

static inline size_t set_find_char(const set_t *this, const char query) {
	if (this->type == SET_TYPE_UNIQUE) {
		return this->p_set->unique.set_map[(size_t) (unsigned char) query];
	} else {
		char *swp = memchr(this->p_set->multi.set, query, this->p_set->multi.count);
		if (swp == NULL) {
			return FIND_NPOS;
		} else {
			return (size_t) (swp - this->p_set->multi.set);
		}
	}
}

static char set_get(const set_t *this, size_t index) {
	multi_set *p_set = &this->p_set->multi;

	if (index >= p_set->count) {
		index = p_set->count - 1;
	}
	return p_set->set[index];
}
