CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra

farm: main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f farm

.PHONY: clean
