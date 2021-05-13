SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

MAIN := $(BIN_DIR)/exe #output exe

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC)) # Convert .cpp files to .o

CXXFLAGS := -Iinclude -pthread
# LDLIBS := -lavcodec -lavformat -lGL -lGLU -lglfw
LDLIBS := -lsfml-graphics -lsfml-window -lsfml-system -lavcodec -lavformat -lavutil -lpthread

.PHONY: all clean

all: $(MAIN)

$(MAIN): $(OBJ) | $(BIN_DIR)
	$(CXX) $^ -o $@ $(LDLIBS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

-include $(OBJ:.o=.d)