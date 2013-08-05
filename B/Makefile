
main: main.o ../A/libA.a
	$(CXX) -o $@ $^

%.o: %.cxx
	$(CXX) -c $< -I ../A

clean:
	$(RM) main main.o