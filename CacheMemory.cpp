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
	cacheTable = new Block *[numOfWays_]; // the number of ways will determine the size of the first degree
	for(int i = 0 ; i < numOfWays_  ; i++)
	{
		cacheTable[i] = new Block[numOfSets_]; //allocate the number of lines
	}
}


CacheMemory::~CacheMemory() {
	for(int i = 0 ; i < (1 << logNumOfWays_)  ; i++)
	{
		delete[] cacheTable[i];
	}
	delete[] cacheTable;
}

unsigned int CacheMemory::getLatency() const {
	return latency_;
}

unsigned long int CacheMemory::getSetIdx(unsigned long int tag) {
	unsigned long int setMask = numOfSets_ - 1;

	return (tag >> logBlockSize_) & setMask;
}

void CacheMemory::updateLru(int wayIdx, int setIdx) {
	int lruState = cacheTable[wayIdx][setIdx].getLruState();
	int maxLruState = numOfWays_ - 1;
		cacheTable[wayIdx][setIdx].setLruState(maxLruState);
		for(int i = 0 ; i < numOfWays_ ; i++){
			if(i != wayIdx && cacheTable[i][setIdx].getLruState() > lruState){
						cacheTable[i][setIdx].setLruState(cacheTable[i][setIdx].getLruState() - 1);
					}
			}
}

void CacheMemory::updateBlock(unsigned long int tag, int wayIdx, int setIdx) {
	cacheTable[wayIdx][setIdx].setTag(tag);
	cacheTable[wayIdx][setIdx].setDirty(true);
	cacheTable[wayIdx][setIdx].setValid(true);
}

void CacheMemory::writeBlock(unsigned long int tag, int wayIdx, int setIdx) {
	cacheTable[wayIdx][setIdx].setTag(tag);
	cacheTable[wayIdx][setIdx].setDirty(false);
	cacheTable[wayIdx][setIdx].setValid(true);
}

bool CacheMemory::isBlockInCache(unsigned long int tag, unsigned long &wayIdx, unsigned long &setIdx) {
	setIdx = getSetIdx(tag);
	for(int i = 0 ; i < numOfWays_ ; i++){
		if(cacheTable[i][setIdx].getValid() && cacheTable[i][setIdx].getTag() == tag){
			wayIdx = i;
			return true;
		}
	}
	return false;
}

unsigned long int
CacheMemory::locateVictimBlock(unsigned long int tag, unsigned long &wayIdx, unsigned long &setIdx, bool &isDirty,
							   bool &isValid) {
	unsigned long int victimTag;
	setIdx = getSetIdx(tag);
	for(int i = 0 ; i < numOfWays_ ; i++){
		if(!cacheTable[i][setIdx].getValid()){
			wayIdx = i;
			victimTag = cacheTable[i][setIdx].getTag();
			isValid = false;
			return victimTag;
		}
	}
	for(int i = 0 ; i < numOfWays_ ; i++){
		if(cacheTable[i][setIdx].getLruState() == 0){
			wayIdx = i;
			victimTag = cacheTable[i][setIdx].getTag();
			isValid = true;
			return victimTag;
		}

	}
	return 0;
}

CacheMemory::CacheMemory() {

}

bool CacheMemory::isBlockDirty(int wayIdx, int setIdx) {
	return cacheTable[wayIdx][setIdx].getDirty();
}





