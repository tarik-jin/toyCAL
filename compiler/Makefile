EXE=tareq
CC=g++
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
CXXFLAGS=-g
all:dep $(EXE)
dep:
	$(CC) -MM $(SRC) > .dep
-include .dep
$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ)
clean:
	rm -f $(EXE) $(OBJ) *.h.gch .dep
