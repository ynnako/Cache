

#ifndef CACHE_CACHEMEMORY_H
#define CACHE_CACHEMEMORY_H


#include "Block.h"
#include <vector>
#include <math.h>

using namespace std;
class CacheMemory {
private:
	unsigned int logBlockSize_;
	unsigned int logCacheSize_;
	unsigned int latency_;
	unsigned int logNumOfWays_;
	unsigned int numOfWays_;
	unsigned int numOfBlocks_;
	unsigned int numOfSets_;
	Block **cacheTable;


public:
	CacheMemory(unsigned int logBlockSize, unsigned int logCacheSize, unsigned int logNumOfWays);
	CacheMemory();
	~CacheMemory();
	unsigned int getLatency() const;
	unsigned long int getSetIdx(unsigned long int tag);
	void updateLru(int wayIdx, int setIdx);
	void updateBlock(unsigned long int tag , int wayIdx, int setIdx);
	void writeBlock(unsigned long int tag , int wayIdx, int setIdx);
	bool isBlockInCache(unsigned long int tag, unsigned long &wayIdx, unsigned long &setIdx);
	unsigned long int locateVictimBlock(unsigned long int tag, unsigned long &wayIdx, unsigned long &setIdx, bool &isDirty,
											bool &isValid);
	bool isBlockDirty(int wayIdx , int setIdx);






};


#endif //CACHE_CACHEMEMORY_H
