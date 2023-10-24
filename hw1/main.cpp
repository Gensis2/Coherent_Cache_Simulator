#include <iostream>
#include <fstream>
#include <string>

using namespace std;
int main(){

    int cacheSize = 1024;
    int blockSize = 16;
    int setSize = 32;
    char operation;
    int memory;

    string input;
    ifstream ifp ("gcc_trace.txt");

    if(ifp.is_open()){ 
        ifp >> operation >> hex >> memory; 
        cout << hex;
        cout << memory << " tag: " << memory/(blockSize * setSize);
        cout << " set: " << (memory/blockSize) % (blockSize * setSize);
        cout << " block: " << memory % (setSize * blockSize) << endl;
    }
    else{
        cout << "pooppy" << endl;
    }
    
    ifp.close();
    return 0;
}

class Block : public Set {
private:
    int tag;

public:
    Block(){

    }


    int getTag(){
        return tag;
    }
    void setTag(int memoryAccess){
        tag = memoryAccess/(getBlockSize() * getSetSize());
    }
};

class Set : public Level {
private:
    Block* blocks;

public:
    Set(){

    }
};

class Level : public Cache {
private:
    Set* sets;
    int levelSize;
    int assoc;
    int blockSize;

public:
    Level(){

    }
};

class Cache {
private:
    Level* levels;
    Level* levelHead;
    Level* levelTail;

public:
    Cache(){

    }
};