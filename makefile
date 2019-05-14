CC = g++
CXXFLAGS = -g -Wall -std=c++11
CCLINK = $(CC)
OBJS = Block.o CacheMemory.o cacheSim.o CpuSim.o VictimCache.o
TARGET = cacheSim

$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(OBJS)

Block.o: Block.cpp Block.h
CacheMemory.o: CacheMemory.cpp CacheMemory.h Block.h
cacheSim.o: cacheSim.cpp CacheMemory.h Block.h VictimCache.h CpuSim.h
CpuSim.o: CpuSim.cpp CpuSim.h CacheMemory.h Block.h VictimCache.h
VictimCache.o: VictimCache.cpp VictimCache.h Block.h

clean:
	rm -f *.o
	rm -f cacheSim

