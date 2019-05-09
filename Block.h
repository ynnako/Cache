//
// Created by Yonatan on 4/28/2019.
//

#ifndef CACHE_ENTRY_H
#define CACHE_ENTRY_H


class Block {
private:
	unsigned long int tag_;
	unsigned long int set_;
	unsigned long int lruState_;
	bool valid_;
	bool dirty_;

public:
	Block();
	unsigned long int getTag()const;
	void setTag(unsigned long int tag);
	unsigned long int getLruState()const;
	void setLruState(unsigned long int lruState);
	bool getValid() const;
	void setValid(bool valid);
	bool getDirty() const;
	void setDirty(bool dirty);
	void initBlock(unsigned long int tag, bool isDirty, int numOfWays);
	void setSetIdx(unsigned long int set);
	unsigned long int getSetIdx()const;
//	Block& operator = (Block const& rhs);



};


#endif //CACHE_ENTRY_H
