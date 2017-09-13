program_name="tr"
c_flags=--std=c11 -Weverything

debug:
	clang $(c_flags) -ggdb -o tr main.c
all:
	clang $(c_flags) -o tr main.c

clean:
	rm -f $(program_name) tr.o
