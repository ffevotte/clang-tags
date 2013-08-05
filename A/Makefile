
libA.a: libA.o
	ar r $@ $^

%.o: %.cxx
	$(CXX) -c $<

clean:
	$(RM) libA.a libA.o