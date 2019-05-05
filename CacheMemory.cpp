//
// Created by Yonatan on 4/28/2019.
//

#include "CacheMemory.h"

CacheMemory::CacheMemory(unsigned int blockSize, unsigned int cacheSize, unsigned int latency,
						 unsigned int numOfWays, unsigned int isWriteAllocate, unsigned int mainMemLatency) {
	blockSize_ = blockSize;
	cacheSize_ = cacheSize;
	latency_ = latency;
	lowerMemLatency_ = mainMemLatency;
	numOfWays_ = numOfWays;
	isWriteAllocate_ = isWriteAllocate;
	cacheLevel_ = 2;
	cacheTable = new Block*[1 << numOfWays]; // the number of ways will determine the size of the first degree
	for(int i = 0 ; i < (1 << numOfWays)  ; i++)
	{
		cacheTable[i] = new Block[1 << (cacheSize - blockSize - numOfWays)]; //allocate the number of lines
	}
	lowerCache_ = nullptr;
}

CacheMemory::CacheMemory(unsigned int blockSizeL1, unsigned int cacheSizeL1, unsigned int latencyL1, unsigned int numOfWaysL1,
						 unsigned int isWriteAllocateL1, unsigned int blockSizeL2, unsigned int cacheSizeL2,
						 unsigned int latencyL2, unsigned int numOfWaysL2, unsigned int isWriteAllocateL2,
						 unsigned int mainMemLatency) {

	blockSize_ = blockSizeL1;
	cacheSize_ = cacheSizeL1;
	latency_ = latencyL1;
	lowerMemLatency_ = 0;
	numOfWays_ = numOfWaysL1;
	isWriteAllocate_ = isWriteAllocateL1;
	cacheLevel_ = 1;

	cacheTable = new Block*[1 << numOfWaysL1]; // the number of ways will determine the size of the first degree
	for(int i = 0 ; i < (1 << numOfWaysL1)  ; i++)
	{
		cacheTable[i] = new Block[1 << (cacheSizeL1 - blockSizeL1 - numOfWaysL1)]; //allocate the number of lines
	}
	lowerCache_ = new CacheMemory(blockSizeL2, cacheSizeL2, latencyL2, numOfWaysL2, isWriteAllocateL2, 0);
}

CacheMemory::~CacheMemory() {
	if(cacheLevel_ == 1) delete lowerCache_;
	for(int i = 0 ; i < (1 << numOfWays_)  ; i++)
	{
		delete[] cacheTable[i];
	}
	delete[] cacheTable;
}


bool CacheMemory::cacheHit(unsigned long int tag , int &wayIdx , int &setIdx) {
	unsigned long int effectiveTag = tag >> (blockSize_ + numOfWays_ ); //relevant bits of the tag
	bool hit = false;
	setIdx = getSetIdx(tag);
	for(int i = 0 ; i < numOfWays_  || i == 0 ; i++){ //either we have more than one way or only one way
		if(cacheTable[i][setIdx].getValid() && cacheTable[i][setIdx].getTag() == effectiveTag){
			hit = true;
			wayIdx = i;
			break;
		}
	}
	return hit;
}

unsigned int CacheMemory::readBlock(unsigned long int tag, bool &hit, unsigned long int &snoopTag) {
	int wayIdx = 0 , setIdx = 0 , tmpWayIdx = 0 , tmpSetIdx = 0;
	bool currentLevelCacheHit = cacheHit(tag , setIdx , wayIdx) , lowerLevelCacheHit = false , isDirty = false;
	unsigned int latency = 0;
	unsigned long int victimTag = 0;

	if(!currentLevelCacheHit){
		latency += lowerMemLatency_;
		locateVictimBlock(tag, victimTag, tmpWayIdx, tmpSetIdx, isDirty);
		if(isDirty) {
			if (lowerCache_ != nullptr) {
				lowerCache_->writeBlockToCache(victimTag);
			}
		}
		if(lowerCache_ != nullptr)
		{
			latency += lowerCache_->readBlock(tag, lowerLevelCacheHit, victimTag);

		}


	}
	else{
		updateLru(wayIdx, setIdx);
	}
	return latency_ + latency;
}

unsigned long CacheMemory::locateVictimBlock(unsigned long int tag, unsigned long int &victimTag, int &wayIdx, int &setIdx,
											 bool &isDirty) {
	setIdx = getSetIdx(tag);
	for(int i = 0 ; i < (1 << numOfWays_) ; i++ ) {
		if (!cacheTable[i][setIdx].getValid()) {
			wayIdx = i;
			isDirty = false;
			return cacheTable[i][setIdx].getTag();
		}
	}
	for(int i = 0 ; i < (1 << numOfWays_) ; i++ ) {
		if(cacheTable[i][setIdx].getValid() && cacheTable[i][setIdx].getLruState() == 0){
			wayIdx = i;
			isDirty = cacheTable[i][setIdx].getDirty();
			return cacheTable[i][setIdx].getTag();
		}
	}
}

unsigned long int CacheMemory::getSetIdx(unsigned long int tag) {

	unsigned long int setMask = (1 << numOfWays_) - 1;
	//unsigned long int offsetMask = (1 << blockSize_) - 1;
	if(numOfWays_ == 0) setMask = 0xffffffff;
	unsigned long int set = (tag >> blockSize_) & setMask;
	return set;
}

void CacheMemory::removeBlockFromCache(unsigned long int tag) {
	int setIdx = 0 , wayIdx = 0;
	unsigned long int victimTag = locateVictimBlock(tag, <#initializer#>, wayIdx, setIdx, <#initializer#>);
	if(cacheTable[wayIdx][setIdx].getDirty()){
		writeBlockToCache(victimTag);
	}
	return 0;
}

void CacheMemory::updateLru(int wayIdx, int setIdx) {
	int lruState = cacheTable[wayIdx][setIdx].getLruState();
	int maxLruState = (1 << numOfWays_) - 1;
	cacheTable[wayIdx][setIdx].setLruState(maxLruState);
	for(int i = 0 ; i < maxLruState ; i++){
		if(i != wayIdx && cacheTable[i][setIdx].getLruState() > lruState){
			cacheTable[i][setIdx].setLruState(cacheTable[i][setIdx].getLruState() - 1);
		}
	}
}

void CacheMemory::addBlockToCache(int wayIdx, int setIdx) {


}





