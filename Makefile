CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
LDFLAGS = `sdl2-config --libs`
SDLFLAGS = `sdl2-config --cflags`

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/game

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(SDLFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re