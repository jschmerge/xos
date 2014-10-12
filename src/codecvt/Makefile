%.o: %.cc
	g++ -std=c++11 -Wall -c $<

test: test.o codecvt_specializations.o
	g++ -std=c++11 -Wall -o $@ $^

check: test
	./test

clean:
	rm -f *.o test

codecvt_specializations.o: codecvt_specializations.cc \
 codecvt_specializations.h utf_conversion_helpers.h
test.o: test.cc codecvt_specializations.h codecvt_utf8.h codecvt_mode.h \
 utf_conversion_helpers.h
