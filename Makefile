program_name="tr"
c_flags="-Weverything"

debug:
	clang $(c_flags) -ggdb -o tr main.c
all:
	clang $(c_flags) -o tr main.c

clean:
	rm -f $(program_name) tr.o
