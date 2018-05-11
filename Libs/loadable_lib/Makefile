all: sub.so

sub.so: sub.o
	g++ -std=c++14 -shared -o sub.so $^

%.o: %.cpp
	g++ -std=c++14 -fPIC -c -o $@ $^

clean:
	rm -f sub.so *.o
