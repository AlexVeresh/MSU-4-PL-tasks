mac: 
	g++ --std=c++17 *.cpp -lncurses -o main 
	./main
linux:
	g++ --std=c++17 *.cpp -lncursesw -o main
	./main 