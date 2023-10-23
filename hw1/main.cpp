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
        cout << " block: " << memory % 100 << endl;
    }
    else{
        cout << "pooppy" << endl;
    }
    
    ifp.close();
    return 0;
}

class Block {
    int data;
};

class Set {
    Block* blocks;
    int assoc;
};

class Level {
    Set* sets;
    int levelSize;
};

class Cache {
    Level* levels;
    Level levelHead;
    Level levelTail;
};