a.out: tester.o
	g++ tester.o -pthread

tester.o: tester.cpp
	g++ -c tester.cpp

clean:
	rm -f tester.o a.out
