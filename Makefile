CXX = g++
CXXFLAGS = -fopenmp
LFLAGS = -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -O3 -mavx2 
WFLAGS = -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -static -std=c++17 -O3 -mavx2 
FILE = main

all: $(FILE).cpp
	$(CXX) $(CXXFLAGS) $(FILE).cpp $(LFLAGS) -o app

genmsvc: $(FILE).cpp
	cl /EHsc /openmp:llvm /O2 /Ot /std:c++17 /arch:AVX2 Application.cpp

run: app
	./app

install:
	sudo apt-get install build-essential libglu1-mesa-dev libpng-dev libmpich12

clean:
	rm -rf *.o
