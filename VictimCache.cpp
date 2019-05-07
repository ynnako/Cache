//
// Created by Yonatan on 5/6/2019.
//

#include "VictimCache.h"

VictimCache::VictimCache() {
	fifoFull_ = false;
}

void VictimCache::updateBlockFifoIndex(int lastRemovedFifoIndex) {
	for(int i = 0 ; i < FIFO_SIZE ; i++ ){
		if(victimFifo_[i].getValid() && victimFifo_[i].getLruState() < lastRemovedFifoIndex)
		{
			victimFifo_[i].setLruState(victimFifo_[i].getLruState() + 1);
		}

	}
}

bool VictimCache::isBlockInFifo(unsigned long int tag, int &tagIndexInFifo) {
	for(int i = 0 ; i < FIFO_SIZE ; i++){
		if(victimFifo_[i].getValid() && victimFifo_[i].getTag() == tag){
			tagIndexInFifo = i;
			return true;
		}
	}
	return false;
}

bool VictimCache::writeBlockToFifo(unsigned long int tag, bool isDirty) {
	int maxLru = 0 , currentLru;
	int firsIn = 0;
	bool isRemovedDirty = false;
	if(fifoFull_){
		updateBlockFifoIndex(FIFO_SIZE);
		for(int i = 0 ; i < FIFO_SIZE ; i++){

			if(victimFifo_[i].getLruState() == FIFO_SIZE ){
				isRemovedDirty = victimFifo_[i].getDirty();
				victimFifo_[i].setTag(tag);
				victimFifo_[i].setValid(true);
				victimFifo_[i].setLruState(0);
				victimFifo_[i].setDirty(isDirty);
				break;
			}
		}
	}
	else{
		for(int i = 0 ; i < FIFO_SIZE ; i++){
			if(!victimFifo_[i].getValid()){
				updateBlockFifoIndex(FIFO_SIZE);
				victimFifo_[i].setTag(tag);
				victimFifo_[i].setValid(true);
				victimFifo_[i].setLruState(0);
				victimFifo_[i].setDirty(isDirty);
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

void VictimCache::restoreBlock(int index, bool &isDirty) {
	victimFifo_[index].setValid(false);
	isDirty = victimFifo_[index].getDirty();
	updateBlockFifoIndex(victimFifo_[index].getLruState());
}


