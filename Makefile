.PHONY : all clean
all : main.cc
	g++ main.cc -std=c++11 -o main
clean : 
	@rm -f main
