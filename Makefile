CXX = g++

CXXFLAGS_DEBUG = -std=c++20 -Wall -g -O0 -D DEBUG

CXXFLAGS_RELEASE = -std=c++20 -Wall -O2

SRC_DIR = ./src

OBJS = main.o ssd1306.o

TARGET = out

.PHONY: all debug release clean

all: debug

debug: CXXFLAGS = $(CXXFLAGS_DEBUG)
debug: $(TARGET)

release: CXXFLAGS = $(CXXFLAGS_RELEASE)
release: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

rebuild: clean debug

.PHONY: debug release clean all rebuild
