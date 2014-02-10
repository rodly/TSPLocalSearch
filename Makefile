all:
	clang++ -stdlib=libc++ main.cpp -o tour

clean:
	rm -f tour
