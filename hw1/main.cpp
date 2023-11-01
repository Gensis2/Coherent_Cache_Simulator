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

public:
    Block(){
        tag = 0;
        next = NULL;
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
    int assoc;

public:
    Set(int newAssoc){
        assoc = newAssoc;
        blocks = new Block*[assoc];
        for(int i = 0; i < assoc; i++){
            Block* block = new Block();
            blocks[i] = block;
        }
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
    void addBlock(int i, Block* newBlock){
        blocks[i] = newBlock;
    }
    void setAssoc(int newAssoc){
        cout << newAssoc << endl;
        assoc = newAssoc;
        return;
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
    Block* head;
    int levelSize;
    int assoc;
    int blockSize;
    int nSets;
    int setSize;

public:
    Level(int newAssoc, int newLevelSize, int newBlockSize){
        assoc = newAssoc;
        levelSize = newLevelSize;
        blockSize = newBlockSize;
        setSize = assoc * blockSize;
        nSets = levelSize/(setSize);
        sets = new Set*[nSets];

        for(int i = 0; i < nSets; i++){
            Set* set = new Set(newAssoc);
            sets[i] = set;
            if(i != nSets - 1 && i >= 1){
                sets[i - 1]->getBlock(assoc - 1)->setNext(sets[i]->getBlock(0));
                sets[i]->getBlock(0)->setPrev(sets[i - 1]->getBlock(assoc - 1));
            }
            if(i == nSets - 1){
                sets[i - 1]->getBlock(assoc - 1)->setNext(sets[i]->getBlock(0));
                sets[i]->getBlock(0)->setPrev(sets[i - 1]->getBlock(assoc - 1));
            }
        }
        sets[0]->getBlock(0)->setPrev(sets[nSets - 1]->getBlock(assoc - 1));
        sets[nSets - 1]->getBlock(assoc - 1)->setNext(sets[0]->getBlock(0));

        head = sets[0]->getBlock(0);
    }
    void setHead(Block* newHead){
        head = newHead;
    }
    Block* getHead(){
        return head;
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
    
    int checkSets(int memory){
        int tag = memory/(levelSize/(blockSize * assoc));
        int setN = (memory/blockSize) % (blockSize * assoc);
        Set* set = sets[setN];
        
        for(int i = 0; i < assoc; i++){
            Block* block = sets[i]->getBlock(i);
            if(block->getTag() != tag);
        }
        return 0;
    }
    void lru(int memory){
        int search = checkSets(memory);
        if(search == -1){
            return;
        }
        else{

        }
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
    Block* block = cache->getLevelOne()->getHead();


    for(int i = 0; i <= 10; i++){
        if(ifp.is_open()){ 
            ifp >> operation >> hex >> memory;
            //set = (memory/blockSize) % (blockSize * assoc);
            //tag = memory/(cacheSize/(blockSize * assoc));
            //cout << memory << " tag: " << hex << tag << "\tset: " << dec << set << endl;
        }
        else{
            cout << "poopy" << endl;
        }
    }
    ifp.close();
    delete cache;
    return 0;
}