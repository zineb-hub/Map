CXX = g++
CXXFLAGS = -std=c++17 -g -I../include
SRCS = main.cpp noeud.cpp graphe.cpp arc.cpp
OBJS = main.o noeud.o graphe.o arc.o
TARGET = AlgGra_Projet

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@