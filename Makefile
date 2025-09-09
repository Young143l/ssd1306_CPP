gXX = g++
CXXFLAGS = -std=c++20 -Wall -O2
OBJS = main.o ssd1306.o
TARGET = main

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: ./src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
