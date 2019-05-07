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
	void updateBlockFifoIndex(int lastRemovedFifoIndex);
	bool isBlockInFifo(unsigned long int tag, int &tagIndexInFifo);
	bool writeBlockToFifo(unsigned long int tag, bool isDirty);
	void restoreBlock(int index, bool &isDirty);

};


#endif //CACHE_VICTIMCACHE_H
