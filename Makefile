CXXOPTS = -std=c++11
CXXOPTS += -g -O1
#CXXOPTS = -O3

do: clang-tags

%.o: %.cxx
	clang++ $(CXXOPTS) -c -Wall $< -o $@

clang-tags: main.o  getopt/getopt.o
	clang++ $(CXXOPTS) $^ -lclang -o $@

test:
	./clang-tags -c main.c

clean:
	$(RM) main.o


main.o: clang/cursor.hxx
main.o: clang/sourceLocation.hxx
main.o: clang/translationUnit.hxx
main.o: getopt/getopt.hxx

getopt/getopt.o: getopt/getopt.hxx