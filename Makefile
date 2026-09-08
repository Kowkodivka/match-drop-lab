CXX := clang++
CXXFLAGS := -std=c++26 -Wall -Wextra -O2

TARGETS := calc sim

.PHONY: all run-calc run-sim clean

all: $(TARGETS)

calc: calc.cpp
	$(CXX) $(CXXFLAGS) calc.cpp -o calc

sim: sim.cpp
	$(CXX) $(CXXFLAGS) sim.cpp -o sim

run-calc: calc
	./calc

run-sim: sim
	./sim

clean:
	rm -f $(TARGETS)
