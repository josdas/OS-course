all: libs

libs: main.o static_lib/mul dynamic_lib/sum.so
	g++ -std=c++14 -L. -l:static_lib/mul -l:dynamic_lib/sum.so -Wl,-rpath,. -o $@ $^ -ldl

%.o: %.cpp
	g++ -std=c++14 -c -o $@ $^

clean:
	rm -rf libs *.o
