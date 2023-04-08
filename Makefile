OPER_SYSTEM = linux

all: $(OPER_SYSTEM)

mac: 
	g++ --std=c++17 cpp/*.cpp -lncurses -o main 
linux:
	g++ --std=c++17 cpp/*.cpp -lncursesw -o main