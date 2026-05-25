# Makefile for Collision Engine - SFML3 Compatible

# Compiler flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3
SFML_FLAGS = `pkg-config --cflags sfml-graphics sfml-window sfml-system`
SFML_LIBS = `pkg-config --libs sfml-graphics sfml-window sfml-system`

# Targets
TARGETS = 10k 100k sfml-app test

# Source files
CORE_SRC = core.cpp
DISPLAY_SRC = displayScript.cpp
NAIVE_SRC = naive_collision.cpp

# Default target
.PHONY: all clean help

all: $(TARGETS)

help:
	@echo "Collision Engine - SFML3 Build System"
	@echo "Available targets:"
	@echo "  make all       - Build all targets (10k, 100k, sfml-app, test)"
	@echo "  make 10k       - Build 10k particle simulation"
	@echo "  make 100k      - Build 100k particle simulation"
	@echo "  make sfml-app  - Build display script (sfml-app)"
	@echo "  make test      - Build naive collision test"
	@echo "  make clean     - Remove all built executables"
	@echo "  make help      - Show this help message"

# 10k target - 10,000 particles
10k: $(CORE_SRC)
	$(CXX) $(CXXFLAGS) $(SFML_FLAGS) -DDEFAULT_PARTICLE_COUNT=10000 $< -o $@ $(SFML_LIBS)

# 100k target - 100,000 particles
100k: $(CORE_SRC)
	$(CXX) $(CXXFLAGS) $(SFML_FLAGS) -DDEFAULT_PARTICLE_COUNT=100000 $< -o $@ $(SFML_LIBS)

# sfml-app target - display script
sfml-app: $(DISPLAY_SRC)
	$(CXX) $(CXXFLAGS) $(SFML_FLAGS) $< -o $@ $(SFML_LIBS)

# test target - naive collision
test: $(NAIVE_SRC)
	$(CXX) $(CXXFLAGS) $(SFML_FLAGS) $< -o $@ $(SFML_LIBS)

# Clean build artifacts
clean:
	rm -f $(TARGETS)
	@echo "Cleaned up build artifacts"

# Phony targets for all executables
.PHONY: 10k 100k sfml-app test
