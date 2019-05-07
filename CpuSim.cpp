//
// Created by Yonatan on 5/6/2019.
//

#include "CpuSim.h"

CpuSim::CpuSim(unsigned int l1logBlockSize, unsigned int l1logCacheSize, unsigned int l1logNumOfWays,
			   unsigned int l2logBlockSize, unsigned int l2logCacheSize, unsigned int l2logNumOfWays,
			   unsigned int isAllocate, unsigned int isVictimCache) {
	l1 = new CacheMemory(l1logBlockSize, l1logCacheSize, l1logNumOfWays);
	l2 = new CacheMemory(l2logBlockSize, l2logCacheSize, l2logNumOfWays);
	isVictimCache_ = false;
	isWriteAllocate_ = isAllocate != 0;
	if(isVictimCache != 0){
		isVictimCache_ = true;
		Vc = new VictimCache();
	}
	for(int i = 0 ; i < MAX_MEMORY_LEVELS ; i++ ){
		if(i < 2) missArray_[i] = 0;
		accessArray_[i] = 0;
	}


}

CpuSim::~CpuSim() {
	delete l1;
	delete l2;
	if(isVictimCache_) delete Vc;
}

void CpuSim::getAccessAmount(int *accessArray, int *missArray) const {
	for(int i = 0 ; i < MAX_MEMORY_LEVELS ; i++)
	{
		if(i < 2){
			missArray[i] += missArray_[i];
		}
		accessArray[i] += accessArray_[i];
	}
}

void CpuSim::resetAccessAmount() {
	for (int i = 0; i < MAX_MEMORY_LEVELS ; i++) {
		if(i < 2){
			missArray_[i] = 0;
		}
		accessArray_[i] = 0;
	}

}

void CpuSim::read(unsigned long int tag) {
	unsigned long int l1WayIdx = 0 , l1SetIdx = 0 , l2WayIdx = 0 , l2SetIdx = 0 , l2WayIdx2 = 0 , l2SetIdx2 = 0 , yVictim , xVictim;
	int fifoIdx = 0;
	bool isDirtyL1 = false , isDirtyL2 = false , isDirtyVc = false , isValidL1 = false , isValidL2 = false;
	accessArray_[0] = 1;
	if(l1->isBlockInCache(tag , l1WayIdx , l1SetIdx)){ // block is in l1
		l1->updateLru(l1WayIdx , l1SetIdx);
	}
	else if(l2->isBlockInCache(tag , l2WayIdx , l2SetIdx)){ //block is in l2
		missArray_[0] = 1;
		accessArray_[1] = 1;
		yVictim = l1->locateVictimBlock(tag, l1WayIdx, l1SetIdx, isDirtyL1, isValidL1);
		if(isDirtyL1 && isValidL1){ //is yvictim dirty
			if(l2->isBlockInCache(yVictim , l2WayIdx ,l1SetIdx)) {
				l2->updateBlock(yVictim, l2WayIdx, l2SetIdx);
			}
		}
		l1->writeBlock(tag , l1WayIdx , l1SetIdx);
	}
	else if(!l2->isBlockInCache(tag , l2WayIdx , l2SetIdx)){
		missArray_[0] = 1;
		missArray_[1] = 1;
		accessArray_[1] = 1;
		yVictim = l2->locateVictimBlock(tag, l2WayIdx, l2SetIdx, isDirtyL2, isValidL2);
		if( isValidL2 && l1->isBlockInCache(yVictim ,l1WayIdx ,l1SetIdx)){
			isDirtyL1 = l1->isBlockDirty(l1WayIdx , l1SetIdx);
			if(isDirtyL1) l2->updateBlock(yVictim ,l2WayIdx ,l2SetIdx);
		}
		else{
			xVictim = l1->locateVictimBlock(yVictim, l1WayIdx, l2SetIdx, isDirtyL1, isValidL1);
			if(isValidL1 && isDirtyL1){
				l2->isBlockInCache(xVictim , l2WayIdx2 , l2SetIdx2);
				l2->updateBlock(xVictim , l2WayIdx2 , l2SetIdx2);
			}
		}
		//isDirtyL2 = l2->isBlockInCache(yVictim , l2WayIdx ,l2SetIdx);
		l2->writeBlock(tag , l2WayIdx , l2SetIdx);
		if(isVictimCache_) {
			if (Vc->isBlockInFifo(tag, fifoIdx)) {
				Vc->restoreBlock(fifoIdx, isDirtyVc);
				accessArray_[2] = 1;
			}
			else accessArray_[3] = 1;

			if(isValidL2)Vc->writeBlockToFifo(yVictim , isDirtyL2);
		}
		else accessArray_[3] = 1;
		if(isDirtyVc) l1->updateBlock(tag ,l1WayIdx ,l1SetIdx);
		else l1->writeBlock(tag ,l1WayIdx ,l1SetIdx);

	}
}

void CpuSim::write(unsigned long int tag) {
	if(isWriteAllocate_){
		read(tag);

	}

}

void CpuSim::setDirty(unsigned long int tag, bool isDirty) {
	li
}

