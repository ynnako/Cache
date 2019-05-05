//
// Created by Yonatan on 4/28/2019.
//

#include "Block.h"

Block::Block() {
	tag_ = 0xffffffff;
	lruState_ = 0;
	valid_ = false;
	dirty_ = true;
}

unsigned long int Block::getTag() const {
	return tag_;
}

void Block::setTag(unsigned long int tag) {
	tag_ = tag;
}

int Block::getLruState() const {
	return lruState_;
}

void Block::setLruState(int lruState) {
	lruState_ = lruState;
}

bool Block::getValid() const {
	return valid_;
}

void Block::setValid(bool valid) {
	valid_ = valid;
}

bool Block::getDirty() const {
	return dirty_;
}

void Block::setDirty(bool dirty) {
	dirty_ = dirty;
}

void Block::initBlock(unsigned long int tag, bool isDirty, int numOfWays) {
	tag_ = tag;
	valid_ = true;
	dirty_ = isDirty;
	lruState_ = (1 << numOfWays) - 1;
}






