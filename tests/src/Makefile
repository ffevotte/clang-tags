VPATH=../src

all: main

main: main.o
	$(CXX) -o $@ $<

main.o: main.cxx
	$(CXX) -c -o $@ $<

clean:
	$(RM) main.o