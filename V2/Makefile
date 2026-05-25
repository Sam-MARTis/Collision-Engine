# Makefile for Collision Engine - SFML3 Compatible

# Compiler flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O1 -march=native
SFML_FLAGS = `pkg-config --cflags sfml-graphics sfml-window sfml-system`
SFML_LIBS = `pkg-config --libs sfml-graphics sfml-window sfml-system`

# Targets
TARGETS = sim


MAIN_SRC = main.cpp
AUX_SRC = aux.cpp
CORE_SRC = core-functions.cpp
DISPLAY_SRC = display-functions.cpp

HEADERS = display-functions.hpp constants.hpp aux.hpp core-functions.hpp

.PHONY: all clean help

all: $(TARGETS)

SOURCES = $(MAIN_SRC) $(CORE_SRC) $(AUX_SRC) $(DISPLAY_SRC) 



sim: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SFML_FLAGS) $(SOURCES) -o $@ $(SFML_LIBS)

# Clean build artifacts
clean:
	rm -f $(TARGETS)
	@echo "Cleaned up build artifacts"

# .PHONY: 10k 100k sfml-app test
