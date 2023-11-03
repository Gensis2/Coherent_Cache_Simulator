#include <iostream>
#include <fstream>
#include <string>

class Block;
class Set;
class Level;
class Cache;

using namespace std;

class Block{
private:
    Block* next;
    Block* prev;
    int tag;
    bool hit;
    bool miss;

public:
    Block(){
        tag = 0;
        hit = false;
        miss = false;
        next = NULL;
        prev = NULL;
    }
    void setHit(){
        hit = true;
    }
    void resetHit(){
        hit = false;
    }
    bool getHit(){
        return hit;
    }
    void setMiss(){
        miss = true;
    }
    void resetMiss(){
        miss = false;
    }
    bool getMiss(){
        return miss;
    }
    int getTag(){
        return tag;
    }
    void setTag(int newTag){
        tag = newTag;
    }
    Block* getNext(){
        return next;
    }
    void setNext(Block* newNext){
        next = newNext;
    }
    Block* getPrev(){
        return prev;
    }
    void setPrev(Block* newPrev){
        prev = newPrev;
    }
    void printBlock(){
        cout << "Tag = " << tag << hex;
    }
};

class Set{
private:
    Block** blocks;
    Block* head;
    Block* tail;
    int assoc;

public:
    Set(int newAssoc){
        assoc = newAssoc;
        blocks = new Block*[assoc];
        for(int i = 0; i < assoc; i++){
            Block* block = new Block();
            blocks[i] = block;
        }
        head = blocks[0];
        tail = blocks[assoc - 1];
        head->setPrev(tail);
        tail->setNext(head);
        for(int i = 0; i < assoc; i++){
            if(i != assoc - 1){
                blocks[i]->setNext(blocks[i + 1]);
                blocks[i + 1]->setPrev(blocks[i]);
            }
        }

    }

    Block** getBlocks(){
        return blocks;
    }
    Block* getBlock(int i){
        return blocks[i];
    }
    void setHead(Block* newHead){
        head = newHead;
    }
    Block* getHead(){
        return head;
    }
    void setTail(Block* newHead){
        head = newHead;
    }
    Block* getTail(){
        return head;
    }
    void addBlock(int i, Block* newBlock){
        blocks[i] = newBlock;
    }
    void setAssoc(int newAssoc){
        cout << newAssoc << endl;
        assoc = newAssoc;
        return;
    }
    //retuns the block that was replaced or most recently used
    //If tag is found, return NULL;
    Block* memoryAccess(int newTag){
        Block* compareBlock = head;
        do{
            if(compareBlock->getTag() == newTag){
                compareBlock->setHit();
                return compareBlock;
            }
            else if(compareBlock->getTag() == 0){
                compareBlock->setMiss();
                compareBlock->setTag(newTag);
                return compareBlock;
            }
            compareBlock = compareBlock->getNext();
        }while(compareBlock != head);
        compareBlock = head;
        do{
            if(compareBlock->getTag() != newTag){
                compareBlock->setMiss();
                compareBlock->setTag(newTag);
                return compareBlock;
            }
            compareBlock = compareBlock->getNext();
        }while(compareBlock != head);
        return NULL;
    }

    int lruSet(int newTag){
        const int hit = 0, miss = 1;
        Block* access = memoryAccess(newTag);
        access->getNext()->setPrev(access->getPrev());
        access->getPrev()->setNext(access->getNext());
        access->setNext(head);
        access->setNext(tail);
        tail->setNext(access);
        head->setPrev(access);
        tail = access;
        if(access->getHit() == true){
            return hit;
        }
        else if(access->getMiss() == true){
            return miss;
        }
        return -1;
    } 

    int fifoSet(int newTag){
        const int hit = 0, miss = 1;
        Block* access = memoryAccess(newTag);
        tail = head;
        head = head->getNext();
        if(access->getHit() == true){
            return hit;
        }
        else if(access->getMiss() == true){
            return miss;
        }
    }

    void deleteBlock(){
        delete[] blocks;
    }
    void printSet(){
        for(int i = 0; i < assoc; i++){
            cout << "Block " << i << ":"  << " ";
            blocks[i]->printBlock();
            cout << endl << "\t\t";
        }
    }
};

class Level{
private:
    Set** sets;
    int levelSize;
    int assoc;
    int blockSize;
    int nSets;
    int setSize;
    int reads, readMisses, writes, writeMisses, writeBacks, writeThroughs;
    double missRate;


public:
    Level(int newAssoc, int newLevelSize, int newBlockSize){
        reads = 0; readMisses = 0; writes = 0; writeMisses = 0;
        writeBacks = 0; writeThroughs = 0; missRate = 0;

        assoc = newAssoc;
        levelSize = newLevelSize;
        blockSize = newBlockSize;
        setSize = assoc * blockSize;
        nSets = levelSize/(setSize);
        sets = new Set*[nSets];

        for(int i = 0; i < nSets; i++){
            Set* set = new Set(newAssoc);
            sets[i] = set;
        }
    }
    Set** getSets(){
        return sets;
    }
    Set* getSet(int i){
        return sets[i];
    }
    int getLevelSize(){
        return levelSize;
    }
    int getAssoc(){
        return assoc;
    }
    int getSetSize(){
        return setSize;
    }
    void deleteSets(){
        for(int i = 0; i < assoc; i++){
            sets[i]->deleteBlock();
        }
        delete[] sets;
    }
    void printLevel(){
        for(int i = 0; i < nSets; i++){
            cout << "Set " << i << dec << ":" << endl << "\t\t";
            sets[i]->printSet();
            cout << endl << "\t";
        }
    }
    
    void memoryAccess(int memory){
        int tag = memory/(levelSize/(blockSize * assoc));
        int set = (memory/blockSize) % (blockSize * assoc);
        sets[set]->lruSet(tag);
    }
};

class Cache {
private:
    Level* levelOne;
    Level* levelTwo;
    int cacheSize;
    int blockSize;
    int writePolicy;
    int inclusionPolity;


public:
    Cache(int cacheOneSize, int cacheTwoSize, int newBlockSize, int cacheOneAssoc, int cacheTwoAssoc){
        if(cacheTwoSize <= 0){
            cacheSize = 1;
        }
        else{
            cacheSize = 2;
        }
        blockSize = newBlockSize;
        if(cacheSize == 1){
            levelOne = new Level(cacheOneAssoc, cacheOneSize, newBlockSize);
        }
        else if(cacheSize == 2){
            levelOne = new Level(cacheOneAssoc, cacheOneSize, newBlockSize);
            levelTwo = new Level(cacheTwoAssoc, cacheTwoSize, newBlockSize);
        }
    }

    Level* getLevelOne(){
        return levelOne;
    }
    Level* getLevelTwo(){
        return levelTwo;
    }
    int getBlockSize(){
        return blockSize;
    }

    int searchCache(int tag){
        if(levelOne->getLevelSize() <= 0 && levelTwo->getLevelSize() <= 0){
            return -1;
        }
        else if(levelTwo->getLevelSize() <= 0){

        }
    }

    void deleteLevels(){
        if(cacheSize == 1){
            levelOne->deleteSets();
            delete levelOne;
        }
        else if(cacheSize == 2){
            levelOne->deleteSets();
             levelTwo->deleteSets();
            delete levelOne;
            delete levelTwo;
        } 
    }

    void printLevels(){
        cout << "L1 Cache:" << endl << "\t";
        levelOne->printLevel();
        if(cacheSize == 2){
            cout << endl;
            cout << "L2 Cache" << endl << "\t";
            levelTwo->printLevel();
        }
    }
};

int main(){

    int cacheSizeTwo = 0;
    int cacheSize = 1024;
    int blockSize = 16;
    int assoc = 2;
    char operation;
    int memory;
    int set;
    int tag;

    string input;
    ifstream ifp ("gcc_trace.txt");

    Cache* cache = new Cache(1024, 0, 16, 2, 0);




    for(int i = 0; i <= 284; i++){
        if(ifp.is_open()){ 
            ifp >> operation >> hex >> memory;
            if(i == 283){
                cout << "print here" << endl;
            }
            //set = (memory/blockSize) % (blockSize * assoc);
            //tag = memory/(cacheSize/(blockSize * assoc));
            //cout << memory << " tag: " << hex << tag << "\tset: " << dec << set << endl;
            cache->getLevelOne()->memoryAccess(memory);
        }
        else{
            cout << "poopy" << endl;
        }
    }
    ifp.close();
    cache->printLevels();
    delete cache;
    return 0;
}