//
// Created by Yonatan on 4/28/2019.
//

#include "CacheMemory.h"

CacheMemory::CacheMemory(unsigned int logBlockSize, unsigned int logCacheSize, unsigned int logNumOfWays) {
	logBlockSize_ = logBlockSize;
	logCacheSize_ = logCacheSize;
	logNumOfWays_ = logNumOfWays;
	numOfBlocks_ = 1 << (logCacheSize_ - logBlockSize);
	numOfWays_ = 1 << logNumOfWays;
	numOfSets_ = 1 <<  (logCacheSize - logBlockSize - logNumOfWays);
	setMask_ = logNumOfWays == 0 ? numOfBlocks_ - 1 :numOfSets_ - 1;
//	cacheTable = new Block *[numOfWays_]; // the number of ways will determine the size of the first degree
//	for(int i = 0 ; i < numOfWays_  ; i++)
//	{
//		cacheTable[i] = new Block[numOfSets_]; //allocate the number of lines
//	}
}


CacheMemory::~CacheMemory() {
//	for(int i = 0 ; i < (1 << logNumOfWays_)  ; i++)
//	{
//		delete[] cacheTable[i];
//	}
//	delete[] cacheTable;
}


unsigned long int CacheMemory::getSetIdx(unsigned long int tag) {
	return (tag >> logBlockSize_) & setMask_;
}

void CacheMemory::updateLru(unsigned long int wayIdx, unsigned long int setIdx) {
	unsigned long int lruState = cacheTable[wayIdx][setIdx].getLruState();
	unsigned long int maxLruState = numOfWays_ - 1;
		cacheTable[wayIdx][setIdx].setLruState(maxLruState);
		for(int i = 0 ; i < numOfWays_ ; i++){
			if(i != wayIdx && cacheTable[i][setIdx].getLruState() > lruState){
						cacheTable[i][setIdx].setLruState(cacheTable[i][setIdx].getLruState() - 1);
					}
			}
}

void CacheMemory::updateBlock(unsigned long int tag, unsigned long int wayIdx, unsigned long int setIdx, bool isDirty) {
	cacheTable[wayIdx][setIdx].setTag(makeEffectiveTag(tag));
	cacheTable[wayIdx][setIdx].setDirty(isDirty);
	cacheTable[wayIdx][setIdx].setValid(true);
	cacheTable[wayIdx][setIdx].setSetIdx(setIdx);
}

void CacheMemory::writeBlock(unsigned long int tag, unsigned long int wayIdx, unsigned long int setIdx) {
	cacheTable[wayIdx][setIdx].setTag(makeEffectiveTag(tag));
	cacheTable[wayIdx][setIdx].setDirty(false);
	cacheTable[wayIdx][setIdx].setValid(true);
	cacheTable[wayIdx][setIdx].setSetIdx(setIdx);
}

bool CacheMemory::isBlockInCache(unsigned long int tag, unsigned long &wayIdx, unsigned long &setIdx) {
	unsigned long int tmpTag = makeEffectiveTag(tag);
	setIdx = getSetIdx(tag);
	for(unsigned long int i = 0 ; i < numOfWays_ ; i++){
		if(cacheTable[i][setIdx].getValid() && cacheTable[i][setIdx].getTag() == tmpTag && cacheTable[i][setIdx].getSetIdx() == setIdx){
			wayIdx = i;
			return true;
		}
	}
	return false;
}

unsigned long int
CacheMemory::selectVictimBlock(unsigned long int tag, unsigned long &wayIdx, unsigned long &setIdx, bool &isDirty,
							   bool &isValid) {
	unsigned long int victimTag;
	setIdx = getSetIdx(tag);
	for(unsigned long int i = 0 ; i < numOfWays_ ; i++){
		if(!cacheTable[i][setIdx].getValid()){
			wayIdx = i;
			victimTag = 0xfffffffff;
			isValid = false;
			return victimTag;
		}
	}
	for(unsigned long int i = 0 ; i < numOfWays_ ; i++){
		if(cacheTable[i][setIdx].getLruState() == 0){
			wayIdx = i;
			victimTag = restoreTag(cacheTable[i][setIdx].getTag() , setIdx);
			isValid = true;
			return victimTag;
		}

	}
	return 0;
}

CacheMemory::CacheMemory() {

}

bool CacheMemory::isBlockDirty(unsigned long int wayIdx, unsigned long int setIdx) {
	return cacheTable[wayIdx][setIdx].getDirty();
}

const Block & CacheMemory::getBlock(unsigned long wayIdx, unsigned long setIdx) const {
	return cacheTable[wayIdx][setIdx];
}

void CacheMemory::setBlock(const Block &memBlock, unsigned long wayIdx, unsigned long setIdx) {
	cacheTable[wayIdx][setIdx] = memBlock;
}

void CacheMemory::invalidateBlock(unsigned long wayIdx, unsigned long setIdx) {
	cacheTable[wayIdx][setIdx].setValid(false);
}

unsigned long CacheMemory::makeEffectiveTag(unsigned long int tag) {
	unsigned long int tmpTag = tag >> (logCacheSize_ - logNumOfWays_) ;
	return tmpTag;
}

unsigned long int CacheMemory::restoreTag(unsigned long int effectiveTag, unsigned long int setIdx) {
	unsigned long int tmpTag = effectiveTag << (logCacheSize_ - logNumOfWays_);
	tmpTag += setIdx;
	tmpTag << logBlockSize_;
	return tmpTag;
}





