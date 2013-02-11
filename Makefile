do: clang-tags

%.o: %.cxx
	clang++ -c -Wall $^ -o $@

clang-tags: main.o
	clang++ $^ -lclang -o $@

test:
	./clang-tags -c main.c

clean:
	$(RM) main.o