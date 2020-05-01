.PHONY : all clean
all : main.cc
	g++ main.cc -std=c++11 -O2 -o main
clean : 
	@rm -f main
