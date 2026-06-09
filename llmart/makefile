CC=clang
CXX=clang++

CFLAGS=-O2 -std=c17 -I./src
CXXFLAGS=-O2 -std=c++17 -I./src

LDFLAGS=-lcurl

# C layer (runtime)
SRC_C=src/main.c src/http.c

# C++ layer (logic)
SRC_CPP=src/bridge.cpp src/backend.cpp

OBJ_C=$(SRC_C:.c=.o)
OBJ_CPP=$(SRC_CPP:.cpp=.o)

TARGET=llmrt

all: $(TARGET)

$(TARGET): $(OBJ_C) $(OBJ_CPP)
	$(CXX) $(OBJ_C) $(OBJ_CPP) $(LDFLAGS) -o $(TARGET)

# compile C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# compile C++
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)