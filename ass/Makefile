EXE=tareq
CC=g++
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
CXXFLAGS=-g -c
all:dep $(EXE)
dep:
	@$(CC) -MM $(SRC) > .dep
-include .dep
$(EXE):$(OBJ)
	$(CC) -o $(EXE) $(OBJ) 2> ~/exercise/sharepoint/compileError
%.o:%.cpp
	$(CC) $(CXXFLAGS) $< 2> ~/exercise/sharepoint/compileError
clean:
	rm -f $(EXE) $(OBJ) *.h.gch .dep
