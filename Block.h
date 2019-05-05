//
// Created by Yonatan on 4/28/2019.
//

#ifndef CACHE_ENTRY_H
#define CACHE_ENTRY_H


class Block {
private:
	unsigned long int tag_;
	int lruState_;
	bool valid_;
	bool dirty_;

public:
	Block();
	unsigned long int getTag()const;
	void setTag(unsigned long int tag);
	int getLruState()const;
	void setLruState(int lruState);
	bool getValid() const;
	void setValid(bool valid);
	bool getDirty() const;
	void setDirty(bool dirty);
	void initBlock(unsigned long int tag, bool isDirty, int numOfWays);



};


#endif //CACHE_ENTRY_H
