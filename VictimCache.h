//
// Created by Yonatan on 5/6/2019.
//

#ifndef CACHE_VICTIMCACHE_H
#define CACHE_VICTIMCACHE_H
#define FIFO_SIZE 4

#include "Block.h"

class VictimCache {
private:
	Block victimFifo_[FIFO_SIZE];
	bool fifoFull_;
public:
	VictimCache();
	void updateBlockFifoIndex(unsigned long lastRemovedFifoIndex);
	bool isBlockInFifo(unsigned long int tag, unsigned long int set, unsigned long &tagIndexInFifo);
	bool writeBlockToFifo(Block blockToFifo);
	void restoreBlock(unsigned long index, bool &isDirty);
	void updateDirty(unsigned long int fifoIdx);
	void refreshQueue();


};


#endif //CACHE_VICTIMCACHE_H
