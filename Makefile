CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I. -I../LAB_2
ASAN_FLAGS = -fsanitize=address
LDFLAGS =

SRCS = main.cpp Cardinal.cpp tests/test_cardinal.cpp tests/test_lazy_sequence.cpp tests/test_stream.cpp tests/test_fsm.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = lab4

.PHONY: all asan valgrind clean rebuild run debug

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(ASAN_FLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(ASAN_FLAGS) -c $< -o $@

asan: $(TARGET)
	./$(TARGET)

valgrind: clean
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean all

debug: $(TARGET)
	gdb ./$(TARGET)