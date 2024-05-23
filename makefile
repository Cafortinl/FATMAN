CXX       := g++
CXX_FLAGS := 

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib
LIBRARIES	:=
EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	@echo "Executing...\n"
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	@echo "Building...\n"
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	@echo "Clearing...\n"
	-rm $(BIN)/*
