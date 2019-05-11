

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
	unsigned int logNumOfWays_;
	unsigned int numOfWays_;
	unsigned int numOfBlocks_;
	unsigned int numOfSets_;
	unsigned int setMask_;
	Block **cacheTable;


public:
	CacheMemory(unsigned int logBlockSize, unsigned int logCacheSize, unsigned int logNumOfWays);
	CacheMemory();
	~CacheMemory();
	const Block & getBlock(unsigned long wayIdx, unsigned long setIdx) const;
	void setBlock(const Block &memBlock, unsigned long wayIdx, unsigned long setIdx) ;
	void invalidateBlock(unsigned long wayIdx , unsigned long setIdx);
	unsigned long int getSetIdx(unsigned long int tag);
	void updateLru(unsigned long int wayIdx, unsigned long int setIdx);
	void updateBlock(unsigned long int tag, unsigned long int wayIdx, unsigned long int setIdx, bool isDirty);
	void writeBlock(unsigned long int tag , unsigned long int wayIdx, unsigned long int setIdx);
	bool isBlockInCache(unsigned long int tag, unsigned long &wayIdx, unsigned long &setIdx);
	unsigned long int selectVictimBlock(unsigned long int tag, unsigned long &wayIdx, unsigned long &setIdx,
										bool &isDirty,
										bool &isValid);
	bool isBlockDirty(unsigned long int wayIdx , unsigned long int setIdx);
	unsigned long makeEffectiveTag(unsigned long int tag);
	unsigned long int restoreTag(unsigned long int effectiveTag, unsigned long int setIdx);






};


#endif //CACHE_CACHEMEMORY_H
