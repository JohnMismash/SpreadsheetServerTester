spreadsheet_tester: tester.o
	g++ -o spreadsheet_tester tester.o -pthread

tester.o: tester.cpp
	g++ -c tester.cpp

clean:
	rm -f tester.o spreadsheet_tester
