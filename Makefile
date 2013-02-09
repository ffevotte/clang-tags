do: clang-tags

%.o: %.c
	clang -c -Wall $^ -o $@

clang-tags: main.o
	clang $^ -lclang -o $@

test:
	./clang-tags -c main.c