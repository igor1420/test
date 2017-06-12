all: main

main:
	g++ -o main main.cpp -lpthread

clean:
	rm -f main
