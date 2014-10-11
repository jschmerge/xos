%.o: %.cc
	g++ -std=c++11 -Wall -c $<

test: test.o codecvt_specializations.o
	g++ -std=c++11 -Wall -o $@ $^

clean:
	rm -f *.o test
