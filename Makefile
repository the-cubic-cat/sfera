CXX       := g++
CXX_FLAGS := -std=c++23 -ggdb $(shell pkg-config --cflags SDL2_gfx) -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -fsanitize=undefined -J 16

BIN     := bin
SRC     := src
INCLUDE := include

LIBRARIES   := -lSDL2main -lSDL2 -lSDL2_ttf $(shell pkg-config --libs SDL2_gfx)
EXECUTABLE  := main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*