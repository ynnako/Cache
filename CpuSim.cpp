//
// Created by Yonatan on 5/6/2019.
//

#include <iostream>
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
	for(unsigned long int i = 0 ; i < MAX_MEMORY_LEVELS ; i++ ){
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
	unsigned long int effectiveTagVc = l2->makeEffectiveTag(tag) , effectiveSet = l2->getSetIdx(tag);
	unsigned long int fifoIdx = 0;
	bool isDirtyL1 = false , isDirtyL2 = false , isDirtyVc = false , isValidL1 = false , isValidL2 = false , lruFlag = false;
	accessArray_[0] = 1;




	if(l1->isBlockInCache(tag , l1WayIdx , l1SetIdx)){ // block is in l1
		l1->updateLru(l1WayIdx , l1SetIdx);
	}
	else if(l2->isBlockInCache(tag , l2WayIdx , l2SetIdx)){ //block is in l2
		missArray_[0] = 1;
		accessArray_[1] = 1;
		l2->updateLru(l2WayIdx , l2SetIdx);
		yVictim = l1->selectVictimBlock(tag, l1WayIdx, l1SetIdx, isDirtyL1, isValidL1);
		if(isValidL1 && isDirtyL1){
			if(l2->isBlockInCache(yVictim , l2WayIdx2 ,l2SetIdx2)){
				l2->updateBlock(yVictim, l2WayIdx2, l2SetIdx2, true);
				l2->updateLru(l2WayIdx , l2SetIdx);
			}
			else{
				cerr << "DEBUG - ERROR, Memory not inclusive." << endl << "L2 - hit , L1 - Miss" << endl;
			}

		}
		if(l2->isBlockDirty(l2WayIdx , l2SetIdx)){
			l2->updateBlock(tag , l2WayIdx , l2SetIdx , false);
			l1->updateBlock(tag , l1WayIdx , l1SetIdx , true);
			l1->updateLru(l1WayIdx , l1SetIdx);
		}
		else{
			l1->updateBlock(tag , l1WayIdx , l1SetIdx , false);
			l1->updateLru(l1WayIdx , l1SetIdx);
		}


	}
	else if(!l2->isBlockInCache(tag , l2WayIdx , l2SetIdx)){
		missArray_[0] = 1;
		missArray_[1] = 1;
		accessArray_[1] = 1;
		yVictim = l2->selectVictimBlock(tag, l2WayIdx, l2SetIdx, isDirtyL2, isValidL2);
		if( isValidL2 && l1->isBlockInCache(yVictim ,l1WayIdx ,l1SetIdx)){
			xVictim = yVictim;
			if(l1->isBlockDirty(l1WayIdx , l1SetIdx)){
				l2->updateBlock(yVictim, l2WayIdx, l2SetIdx, true);
				l1->updateBlock(yVictim ,l1WayIdx , l2SetIdx , false);
				//lruFlag = true;
			}
		}
		else{
			xVictim = l1->selectVictimBlock(tag, l1WayIdx, l1SetIdx, isDirtyL1, isValidL1);
			if (isValidL1 && isDirtyL1) {
				if(l2->isBlockInCache(xVictim, l2WayIdx2, l2SetIdx2)){
					l2->updateBlock(xVictim, l2WayIdx2, l2SetIdx2, true);
					l1->updateBlock(xVictim, l1WayIdx, l1SetIdx, false);
					lruFlag = true;
				}
				else{
					cerr << "ERROR, Memory not inclusive." << endl << "L2 - miss , L1 - Miss" << endl;
				}
			}
		}

		if(isVictimCache_) {
			accessArray_[2] = 1;
			if (Vc->isBlockInFifo(effectiveTagVc, effectiveSet, fifoIdx)) {
				Vc->restoreBlock(fifoIdx, isDirtyVc);
			}
			else accessArray_[3] = 1;

			if(isValidL2){ //move l2's victim into fifo
				if(Vc->writeBlockToFifo(l2->getBlock(l2WayIdx, l2SetIdx))){
					accessArray_[3] = 1;
				}
			}

		}
		else {
			accessArray_[3] = 1;
		}

		l2->updateBlock(tag , l2WayIdx , l2SetIdx , false);
		l2->updateLru(l2WayIdx , l2SetIdx);
		l1->updateBlock(tag, l1WayIdx, l1SetIdx, isDirtyVc);
		l1->updateLru(l1WayIdx , l1SetIdx);
		if(lruFlag){
			l2->updateLru(l2WayIdx2 , l2SetIdx2);
		}

	}
}

void CpuSim::write(unsigned long int tag) {
	unsigned long int wayIdx = 0 , setIdx = 0 ;
	unsigned long int effectiveTagVc = l2->makeEffectiveTag(tag) , effectiveSetVc = l2->getSetIdx(tag);
	unsigned long int fifoIdx = 0;
	if(isWriteAllocate_){
		read(tag);
		setDirty(tag, 1);
	}
	else {
		if (l1->isBlockInCache(tag, wayIdx, setIdx)) {
			accessArray_[0] = 1;
			setDirty(tag, 1);
			l1->updateLru(wayIdx, setIdx);
			return;
		}
		else if (l2->isBlockInCache(tag, wayIdx, setIdx)) {
			accessArray_[0] = 1;
			accessArray_[1] = 1;
			missArray_[0] = 1;
			setDirty(tag, 2);
			l2->updateLru(wayIdx, setIdx);
			return;
		}
		else if (isVictimCache_) {
			accessArray_[0] = 1;
			accessArray_[1] = 1;
			accessArray_[2] = 1;
			missArray_[0] = 1;
			missArray_[1] = 1;
			if (Vc->isBlockInFifo(effectiveTagVc, effectiveSetVc , fifoIdx)) {
				setDirty(tag, 3);
			}
			else {
				accessArray_[3] = 1;
			}
			return;
		}
		accessArray_[0] = 1;
		accessArray_[1] = 1;
		accessArray_[2] = 1;
		accessArray_[3] = 1;
		missArray_[0] = 1;
		missArray_[1] = 1;

	}


}

void CpuSim::setDirty(unsigned long int tag, int level) {
	unsigned long int wayIdx = 0 , setIdx = 0;
	unsigned long int effectiveTagVc = l2->makeEffectiveTag(tag) , effectiveSetVc = l2->getSetIdx(tag);

	switch(level){
		case 1:
			if(l1->isBlockInCache(tag , wayIdx , setIdx)) {
				l1->updateBlock(tag, wayIdx, setIdx, true);
				l1->updateLru(wayIdx, setIdx);
			}
			else{
				cerr << "set dirty case 1 problematic" << endl;
			}
			break;
		case 2:
			l2->isBlockInCache(tag , wayIdx , setIdx);
			l2->updateBlock(tag, wayIdx, setIdx, true);
			l2->updateLru(wayIdx , setIdx);
			break;
		case 3:
			if(isVictimCache_){
				Vc->updateDirty(effectiveTagVc, effectiveSetVc);
			}
			break;
		default:
			break;
	}
	return;
}

