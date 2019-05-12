//
// Created by Yonatan on 5/6/2019.
//

#include "VictimCache.h"

VictimCache::VictimCache() {
	fifoFull_ = false;
}

void VictimCache::updateBlockFifoIndex(unsigned long int lastRemovedFifoIndex) {
	for(int i = 0 ; i < FIFO_SIZE ; i++ ){
		if(victimFifo_[i].getValid() && victimFifo_[i].getLruState() < lastRemovedFifoIndex)
		{
			victimFifo_[i].setLruState(victimFifo_[i].getLruState() + 1);
		}

	}
}

bool VictimCache::isBlockInFifo(unsigned long int tag, unsigned long int set, unsigned long int &tagIndexInFifo) {
	for(unsigned long int i = 0 ; i < FIFO_SIZE ; i++){
		if(victimFifo_[i].getValid() && victimFifo_[i].getTag() == tag && victimFifo_[i].getSetIdx() == set){
			tagIndexInFifo = i;
			return true;
		}
	}
	return false;
}

bool VictimCache::writeBlockToFifo(Block blockToFifo) {
	bool isRemovedDirty = false;
	if(fifoFull_){
		updateBlockFifoIndex(FIFO_SIZE);
		for(int i = 0 ; i < FIFO_SIZE ; i++){

			if(victimFifo_[i].getLruState() == FIFO_SIZE ){
				isRemovedDirty = victimFifo_[i].getDirty();
				victimFifo_[i] = blockToFifo;
				victimFifo_[i].setLruState(0);
				break;
			}
		}
	}
	else{
		for(int i = 0 ; i < FIFO_SIZE ; i++){
			if(!victimFifo_[i].getValid()){
				updateBlockFifoIndex(FIFO_SIZE);
				victimFifo_[i] = blockToFifo;
				victimFifo_[i].setLruState(0);
				break;
			}
		}

	}
	for(int i = 0 ; i < FIFO_SIZE ; i++){
		if(!victimFifo_[i].getValid()){
			fifoFull_ = false;
			break;
		}
		fifoFull_ = true;
	}
	return isRemovedDirty;
}

void VictimCache::restoreBlock(unsigned long index, bool &isDirty) {
	victimFifo_[index].setValid(false);
	fifoFull_ = false;
	isDirty = victimFifo_[index].getDirty();
	updateBlockFifoIndex(victimFifo_[index].getLruState());
	refreshQueue();
}

void VictimCache::updateDirty(unsigned long int tag, unsigned long int set) {
	unsigned long int idx = 0;
	if(isBlockInFifo(tag, set, idx)){
		victimFifo_[idx].setDirty(true);
	}
}

void VictimCache::refreshQueue() {
	for(int i = 0 ; i < FIFO_SIZE ; i++)
	{
		if(victimFifo_[i].getValid()){
			victimFifo_[i].setLruState(victimFifo_[i].getLruState() - 1);
		}
	}

}



