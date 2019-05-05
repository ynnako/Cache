

#ifndef CACHE_CACHEMEMORY_H
#define CACHE_CACHEMEMORY_H


#include "Block.h"
#include <vector>
#include <math.h>

using namespace std;
class CacheMemory {
private:
	unsigned int blockSize_;
	unsigned int cacheSize_;
	unsigned int latency_;
	unsigned int numOfWays_;
	unsigned int cacheLevel_;
	unsigned int lowerMemLatency_;
	bool isWriteAllocate_;
	CacheMemory* lowerCache_;
	Block** cacheTable;


public:
	CacheMemory(unsigned int blockSize, unsigned int cacheSize, unsigned int latency,
					unsigned int numOfWays, unsigned int isWriteAllocate, unsigned int mainMemLatency);
	CacheMemory(unsigned int blockSizeL1, unsigned int cacheSizeL1, unsigned int latencyL1, unsigned int numOfWaysL1,
					unsigned int isWriteAllocateL1, unsigned int blockSizeL2, unsigned int cacheSizeL2,
					unsigned int latencyL2, unsigned int numOfWaysL2, unsigned int isWriteAllocateL2,
					unsigned int mainMemLatency);
	~CacheMemory();
	unsigned int writeBlockToCache(unsigned int tag);
	unsigned int readBlock(unsigned long int tag, bool &hit, unsigned long int &snoopTag);
	unsigned long int getSetIdx(unsigned long int tag);
	bool cacheHit(unsigned long int tag , int &wayIdx , int &setIdx);
	unsigned long locateVictimBlock(unsigned long int tag, unsigned long int &victimTag, int &wayIdx, int &setIdx,
										bool &isDirty);
	void removeBlockFromCache(unsigned long int tag);
	void updateLru(int wayIdx, int setIdx);
	void addBlockToCache(int wayIdx, int setIdx);
	bool snoop(unsigned int tag);




};


#endif //CACHE_CACHEMEMORY_H
