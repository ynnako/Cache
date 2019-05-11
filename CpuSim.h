//
// Created by Yonatan on 5/6/2019.
//

#ifndef CACHE_CPUSIM_H
#define CACHE_CPUSIM_H
#define MAX_MEMORY_LEVELS 4

#include "CacheMemory.h"
#include "VictimCache.h"

class CpuSim {
private:
	bool isWriteAllocate_;
	bool isVictimCache_;
	CacheMemory *l1 , *l2;
	VictimCache* Vc;
	int accessArray_[MAX_MEMORY_LEVELS]; //amount of access's of each level. indexes: l1 -> 0 , l2 -> 1 , victim cache -> 2 ,
	int missArray_[2];
public:
	CpuSim(unsigned int l1logBlockSize, unsigned int l1logCacheSize, unsigned int l1logNumOfWays,
			   unsigned int l2logBlockSize, unsigned int l2logCacheSize, unsigned int l2logNumOfWays,
			   unsigned int isAllocate, unsigned int isVictimCache);
	~CpuSim();
	void getAccessAmount(double *accessArray, double *missArray) const;
	void resetAccessAmount();
	void read(unsigned long int tag);
	void write(unsigned long int tag);
	void setDirty(unsigned long int tag, int level);

};


#endif //CACHE_CPUSIM_H
