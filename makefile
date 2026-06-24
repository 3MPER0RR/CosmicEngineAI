CC=clang
CXX=clang++

CFLAGS=-O2 -std=c17 -I./src
CXXFLAGS=-O2 -std=c++17 -I./src

LDFLAGS=-lcurl

TARGET=llmrt

# -------------------------
# C CORE + PROVIDERS
# -------------------------
SRC_C=\
src/main.c \
src/http.c \
src/engine.c \
src/img_backend.c \
src/telemetry/telemetry.c \
src/providers/groq/stream.c

# -------------------------
# C++ ENGINE LAYER
# -------------------------
SRC_CPP=\
src/bridge.cpp \
src/backend.cpp \
src/probe.cpp \
src/orchestrator/orchestrator.cpp

OBJ_C=$(SRC_C:.c=.o)
OBJ_CPP=$(SRC_CPP:.cpp=.o)
OBJ=$(OBJ_C) $(OBJ_CPP)

# -------------------------
# BUILD
# -------------------------
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# -------------------------
# C COMPILATION
# -------------------------
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# -------------------------
# C++ COMPILATION
# -------------------------
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# -------------------------
# CLEAN
# -------------------------
clean:
	rm -f $(OBJ) $(TARGET)
