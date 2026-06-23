CC=clang
CXX=clang++

CFLAGS=-O2 -std=c17 -I./src -I./src/llm
CXXFLAGS=-O2 -std=c++17 -I./src -I./src/llm

LDFLAGS=-lcurl

TARGET=llmrt

# C core runtime
SRC_C=\
src/main.c \
src/http.c \
src/engine.c \
src/img_backend.c \
src/llm/stream.c

# C++ engine layer
SRC_CPP=\
src/bridge.cpp \
src/backend.cpp \
src/telemetry.cpp \
src/probe.cpp \
src/orchestrator/orchestrator.cpp

OBJ_C=$(SRC_C:.c=.o)
OBJ_CPP=$(SRC_CPP:.cpp=.o)
OBJ=$(OBJ_C) $(OBJ_CPP)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# compile C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# compile C++
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
