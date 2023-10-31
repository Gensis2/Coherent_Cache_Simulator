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
    int tag;

public:
    Block(){
        tag = 0;
    }
    int getTag(){
        return tag;
    }
    void setTag(int newTag){
        tag = newTag;
    }
};

class Set{
private:
    Block** blocks;
    int assoc;

public:
    Set(int newAssoc){
        Block** blocks = new Block*[assoc];
        assoc = newAssoc;
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
        cout << "here" << endl;
        return;
    }
    void deleteBlock(){
        delete[] blocks;
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

public:
    Level(int newAssoc, int newLevelSize, int newBlockSize){
        assoc = newAssoc;
        levelSize = newLevelSize;
        blockSize = newBlockSize;
        setSize = assoc * blockSize;
        nSets = levelSize/(setSize);

        Set** sets = new Set*[nSets];
        for(int i = 0; i < nSets; i++){
            sets[i] = new Set(assoc);
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
};

class Cache {
private:
    Level* levelOne;
    Level* levelTwo;
    int cacheSize;
    int blockSize;

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
    cout << cache->getLevelOne()->getLevelSize() << endl << cache->getLevelTwo()->getLevelSize() << endl;

    for(int i = 0; i <= 10; i++){
        if(ifp.is_open()){ 
            ifp >> operation >> hex >> memory;
            set = (memory/blockSize) % (blockSize * assoc);
            tag = memory/(cacheSize/(blockSize * assoc));
            cout << memory << " tag: " << hex << tag << "\tset: " << dec << set << endl;


        }
        else{
            cout << "poopy" << endl;
        }
    }
    ifp.close();
    delete cache;
    return 0;
}