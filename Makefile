CXX = g++
CXXFLAGS = -Wall -std=c++11

cache_sim: main.cpp cache_class.cpp cache_class.h
	$(CXX) $(CXXFLAGS) -o cache_sim main.cpp cache_class.cpp

clean:
	rm -f cache_sim cache_sim_output
