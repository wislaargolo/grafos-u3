CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -I.


TEST_SRCS := $(wildcard tests/*.cpp)
BIN_DIR := build
OBJ_DIR := $(BIN_DIR)/obj
BINS := $(patsubst tests/%.cpp,$(BIN_DIR)/%,$(TEST_SRCS))

# Libraries (implementation .cpp files that need to be linked)
LIB_SRCS := algorithm/LocalSearch.cpp utils/TSPUtils.cpp
OBJ_LOCALSEARCH := $(OBJ_DIR)/LocalSearch.o
OBJ_TSPUTILS := $(OBJ_DIR)/TSPUtils.o
LIB_OBJS := $(OBJ_LOCALSEARCH) $(OBJ_TSPUTILS)

.PHONY: all clean

all: $(BINS)

# Build object files for library sources
$(OBJ_LOCALSEARCH): algorithm/LocalSearch.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJ_TSPUTILS): utils/TSPUtils.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile each test and link with the library objects
$(BIN_DIR)/%: tests/%.cpp $(LIB_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIB_OBJS)

clean:
	rm -rf $(BIN_DIR)
