all: mul

mul: mul.o
	ar rcs mul $^
	ranlib mul

%.o: %.cpp
	g++ -std=c++14 -c -o $@ $^

clean:
	rm -f mul *.o
