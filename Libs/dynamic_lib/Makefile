all: sum.so

sum.so: sum.o
	g++ -std=c++14 -shared -o sum.so $^

%.o: %.cpp
	g++ -std=c++14 -fPIC -c -o $@ $^

clean:
	rm -f sum.so *.o
