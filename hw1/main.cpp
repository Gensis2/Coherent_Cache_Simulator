#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

class Block;
class Set;
class Level;
class Cache;

using namespace std;


// Block class. Used to store tag and memory for ease of data transfer, as well as mark bits as dirty
class Block{
private:
    Block* next;
    Block* prev;
    bool dirty;
    int tag;
    int memory;

public: // constructors, setters, and getters
    Block(){
        tag = 0;
        dirty = false;
        next = NULL;
    }
    void setMemory(int newMemory){
        memory = newMemory;
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
        cout << hex;
        cout << tag;
        if(dirty){
            cout << " D ";
        }
        else{
            cout << "   ";
        }
    }
    void setDirty(){
       dirty = true; 
    }
    void unsetDirty(){
        dirty = false;
    }
    bool getDirty(){
        return dirty;
    }
};

class Set{ // set class. Main logic of the simulator. Handles replacement policy and injected and ejection of the cache sets
private:   // this class has a array of blocks that build the set.
    Block** blocks;
    Block* head;
    Block* tail;
    int assoc;
    int replacementPolicy;
    int inclusionProperty;
    int reads, readMisses, writes, writeMisses, writebacks;

public: // constructor, setters, getters, and destructors
    Set(int newAssoc, int replacement, int inclusionPolicy){
        reads = 0, writes = 0, readMisses = 0, writeMisses = 0, writebacks = 0;
        replacementPolicy = replacement;
        assoc = newAssoc;
        inclusionProperty = inclusionPolicy;
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
        assoc = newAssoc;
        return;
    }
    int getReads(){
        return reads;
    }
    int getWrites(){
        return writes;
    }
    int getReadMisses(){
        return readMisses;
    }
    int getWriteMisses(){
        return writeMisses;
    }
    int getWritebacks(){
        return writebacks;
    }


    bool backInvalidate(int newTag){ // method used to backInvalidate if the inclusion property is Inclusive. Will evict from the set and cache, and return to memory.
        Block* compareBlock = head;
        do{
            if(compareBlock->getTag() == newTag){
                compareBlock->setTag(0);
                compareBlock->setMemory(0);
                if(compareBlock->getDirty()){
                    compareBlock->unsetDirty();
                    return true;
                }
                return false;
            }
            compareBlock = compareBlock->getNext();
        }while(compareBlock != tail->getNext());
        return false;
    }

    int memoryAccess(int newTag, int accessType, int level){ // method used to handle memory access, as well as call LRU or FIFO given the replacement policy
        Block* compareBlock = head; 
        int tag = 0;
        do{
            if(compareBlock->getTag() == newTag){ // checks if the block is already in the cache
                if(replacementPolicy == 0){
                    setLRU(compareBlock);
                }
                if(accessType == 1){
                    compareBlock->setDirty();

                }
                return -1;
            }
            compareBlock = compareBlock->getNext();
        }while(compareBlock != tail->getNext());
        
        compareBlock = head;
        do{
            if(compareBlock->getTag() == 0){ // Then checks if there is an empty or invalid block
                compareBlock->setTag(newTag);
                if(replacementPolicy == 0){
                    setLRU(head);
                }
                else{
                    setFIFO(head);
                }
                if(accessType == 1){
                    compareBlock->setDirty();
                }
                if(accessType == 0){
                    readMisses++;
                }
                else if(accessType == 1){
                    writeMisses++;
                }
                return 0;
            }
            compareBlock = compareBlock->getNext();
        }while(compareBlock->getNext() != tail->getNext());
        tag = head->getTag(); // Finaly evicts the head and replaces it, then updates replacement
        head->setTag(newTag);
        if(head->getDirty()){ // used to handle simulator metrics within each cache block
            if(accessType == 0){
                head->unsetDirty();
                writebacks++;
                readMisses++;
            }
            else if(accessType == 1){
                writebacks++;
                writeMisses++;
            }
        }
        else if(!head->getDirty()){
            if(!(inclusionProperty == 1 && level == 1)){
                tag = 0;
            }
            if(accessType == 1){
                head->setDirty();
                writeMisses++;
            }
            else if(accessType == 0){
                readMisses++;
            }
        }
        if(replacementPolicy == 0){
            setLRU(head);
        }
        else{
            setFIFO(head);
        }
        return tag;
    }

    void setLRU(Block* access){ // LRU logic for replacement
        if(access == head){
            tail = head;
            head = head->getNext();
        }
        else if(access == tail){
            return;
        }
        else{
            access->getNext()->setPrev(access->getPrev());
            access->getPrev()->setNext(access->getNext());
            access->setNext(head);
            access->setPrev(tail);
            head->setPrev(access);
            tail->setNext(access);
            tail = access;
        }
    }

    void setFIFO(Block* access){ // Fifo logic for replacement
        head = tail;
        tail = tail->getPrev();
    }


    void deleteBlock(){
        delete[] blocks;
    }
    void printSet(){ // prints the set contents in expected format
        for(int i = 0; i < assoc; i++){
            blocks[i]->printBlock();
            cout << " ";
        }
    }
};

class Level{ // Level class handles each of the two levels, and gives ability to access each set in the level
private:
    Set** sets;
    Level* next;
    Level* prev;
    int levelSize;
    int assoc;
    int blockSize;
    int nSets;
    int setSize;
    int replacementPolicy;
    int inclusionProperty;
    int reads, readMisses, writes, writeMisses, writebacks, writeThroughs, traffic, blocksInvalidated;
    double missRate;


public:
    Level(int newAssoc, int newLevelSize, int newBlockSize, int replacement, int inclusionPolicy){  // constructors, setters, getters, and destructors
        reads = 0; readMisses = 0; writes = 0; writeMisses = 0, blocksInvalidated = 0;
        writebacks = 0; writeThroughs = 0; missRate = 0, traffic = 0;
        
        inclusionProperty = inclusionPolicy;
        replacementPolicy = replacement;
        assoc = newAssoc;
        levelSize = newLevelSize;
        blockSize = newBlockSize;
        setSize = assoc * blockSize;
        nSets = levelSize/(setSize);
        sets = new Set*[nSets];
        next = NULL;
        prev = NULL;

        for(int i = 0; i < nSets; i++){
            Set* set = new Set(newAssoc, replacementPolicy, inclusionPolicy);
            sets[i] = set;
        }
    }
    Set** getSets(){
        return sets;
    }
    Set* getSet(int i){
        return sets[i];
    }
    void setNext(Level* nextL){
        next = nextL;
    }
    void setPrev(Level* prevL){
        prev = prevL;
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
    void printLevel(){ // prints the content of the level (sets, and the sets print blocks) in the expected format
        for(int i = 0; i < nSets; i++){
            cout << dec;
            cout << "Set     " << i << ":";
            if(i < 10){
                cout << "      ";
            }
            else if(i < 100){
                cout << "     ";
            }
            else{
                cout << "    ";
            }
            cout << hex;
            sets[i]->printSet();
            cout << endl;
        }
    }

    void printMetrics(int level){ // This method prints the level metrics, reads, writes, missrate, etc.
        cacheMetrics();
        cout << dec;
        char letter = 'a';
        if(level == 2){
            letter += 6;
        }
        cout << letter << ". number of L" << level << " reads:        " << reads << endl;
        letter++;
        cout << letter << ". number of L" << level << " read misses:  " << readMisses << endl;
        letter++;
        cout << letter << ". number of L" << level << " writes:       " << writes << endl;
        letter++;
        cout << letter << ". number of L" << level << " write misses: " << writeMisses << endl;
        letter++;
        cout << letter << ". L" << level << " miss rate:              " << missRate << endl;
        letter++;
        cout << letter << ". number of L" << level << " writebacks:   " << writebacks << endl;
    }
    
    void cacheMetrics(){ // method to calculate and set all metrics. Accumulates reads, writes, misses, and writebacks from each set and accumulates. Also computes missrate of the given level.
        for(int i = 0; i < nSets; i++){
            reads += sets[i]->getReads();
            writes += sets[i]->getWrites();
            readMisses += sets[i]->getReadMisses();
            writeMisses += sets[i]->getWriteMisses();
            writebacks += sets[i]->getWritebacks();
        }
        
        if(prev != NULL){
            missRate = double(readMisses) / double(reads);
        }
        else{
            missRate = (double(readMisses) + double(writeMisses))/(double(reads) + double(writes));
        }
    }

    int memoryAccess(int memory, int accessType, int level){ // Higher level control of memory access, also increments total reads and writes. This method computes the tag and set given the memory input.
        int ejectedTag = 0;                                  // Outputs an ejected memory block if there is one, else will eject 0 for a clean eject, or -1 for a hit
        int ejectedMemory = 0;
        if(accessType == 0){
            reads++;
        }
        else if(accessType == 1){
            writes++;
        }
        int tag = memory/(blockSize * nSets);
        int setN = (memory % tag) / blockSize;
        ejectedTag = sets[setN]->memoryAccess(tag, accessType, level);
        if(ejectedTag > 0){
            ejectedMemory = (ejectedTag * blockSize * nSets) + (setN * blockSize);
        }
        else{
            ejectedMemory = ejectedTag;
        }
        return ejectedMemory;
    }

    bool backInvalidate(int memory){ // Higher level control for back invalidation. Will return true if the block was invalidated and was dirty, else returns false. Return value is used to 
        bool value = false;          // Calculate total traffic. Method also computes tag and set from memory access.
        int tag = memory/(blockSize * nSets);
        int setN = (memory % tag) / blockSize;
        value = sets[setN]->backInvalidate(tag);
        return value;
    }

    int trafficCalc(){
        traffic = readMisses + writeMisses + writebacks + blocksInvalidated;
        return traffic;
    }

    void incrememntBlocksInvalidated(bool value){
        if(value){
            blocksInvalidated++;
        }
    }
};

class Cache { // High level class, instantiates both levels and controls the inclusion policies
private:
    Level* levelOne;
    Level* levelTwo;
    int levelOneSize;
    int levelTwoSize;
    int levelOneAssoc;
    int levelTwoAssoc;
    int cacheSize;
    int blockSize;
    int replacementPolicy;
    int inclusionProperty;
    int traffic;
    string replacementPolicyStr;
    string inclusionPropertyStr;
    string fileName;


public: // contructors, setters, getters, and destructor
    Cache(int cacheOneSize, int cacheTwoSize, int newBlockSize, int cacheOneAssoc, int cacheTwoAssoc, int replacement, int inclusion, string file){

        traffic = 0;
        replacementPolicy = replacement;
        levelOneSize = cacheOneSize;
        levelTwoSize = cacheTwoSize;
        blockSize = newBlockSize;
        levelOneAssoc = cacheOneAssoc;
        levelTwoAssoc = cacheTwoAssoc;
        inclusionProperty = inclusion;
        fileName = file;

        if(replacement == 0){
            replacementPolicyStr = "LRU";
        }
        else if(replacement == 1){
            replacementPolicyStr = "FIFO";
        }
        if(inclusion == 0){
            inclusionPropertyStr = "non-inclusive";
        }
        else if(inclusion == 1){
            inclusionPropertyStr = "inclusive";
        }
        else if(inclusion == 2){
            inclusionPropertyStr = "exclusive";
        }

        if(cacheTwoSize <= 0){
            cacheSize = 1;
        }
        else{
            cacheSize = 2;
        }
        if(cacheSize == 1){
            levelOne = new Level(cacheOneAssoc, cacheOneSize, newBlockSize, replacementPolicy, inclusionProperty);
        }
        else if(cacheSize == 2){
            levelOne = new Level(cacheOneAssoc, cacheOneSize, newBlockSize, replacementPolicy, inclusionProperty);
            levelTwo = new Level(cacheTwoAssoc, cacheTwoSize, newBlockSize, replacementPolicy, inclusionProperty);
            levelOne->setNext(levelTwo);
            levelTwo->setPrev(levelOne);
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

    void memoryAccess(int memory, int accessType){ // High level memory access, has 2 cases for inclusion policies, and used return data from the sets (ejected memory blocks)
        int ejectedMemory = 0;                     // To elect how to handle the data. If non-inclusive, only dirty ejections are writen, and data in the second cache is only read from accesses.
        int invalidMemory = 0;                     // If inclusive. The same logic is followed, but any memory ejected from the inclusive cache must be checked with the lower level to make sure it is 
        bool value = false;                        // ejected as well.
        if(inclusionProperty == 0){ // non-inclusive
            if(cacheSize >= 1){
                ejectedMemory = levelOne->memoryAccess(memory, accessType, 0);
            }
            if(cacheSize == 2){
                if(ejectedMemory == 0){
                    levelTwo->memoryAccess(memory, 0, 1);
                }
                if(ejectedMemory > 0){
                    levelTwo->memoryAccess(ejectedMemory, 1, 1);
                    levelTwo->memoryAccess(memory, 0, 1);
                }
            }
        }
        else if(inclusionProperty == 1){ // inclusive
            if(cacheSize >= 1){
                ejectedMemory = levelOne->memoryAccess(memory, accessType, 0);
            }
            if(cacheSize == 2){
                if(ejectedMemory == 0){
                    invalidMemory = levelTwo->memoryAccess(memory, 0, 1);
                    if(invalidMemory > 0){
                        value = levelOne->backInvalidate(invalidMemory);
                        levelTwo->incrememntBlocksInvalidated(value);
                    }
                }
                if(ejectedMemory > 0){
                    levelTwo->memoryAccess(ejectedMemory, 1, 1);
                    if(invalidMemory > 0){
                        value = levelOne->backInvalidate(invalidMemory);
                        levelTwo->incrememntBlocksInvalidated(value);
                    }
                    invalidMemory = levelTwo->memoryAccess(memory, 0, 1);
                    if(invalidMemory > 0){
                        value = levelOne->backInvalidate(invalidMemory);
                        levelTwo->incrememntBlocksInvalidated(value);
                    }
                }
            }
        }
        else if(inclusionProperty == 1){ // inclusive

        }
        else if(inclusionProperty == 2){ // exclusive

        }
        else{ // error
            cout << "Inclusion Policy Error" << endl;   
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

    void printLevels(){ // High level method for handleing printing to the console in proper format.
        cout << fixed << setprecision(6);
        cout << "===== Simulator configuration =====" << endl;
        cout << "BLOCKSIZE:             " << blockSize << endl;
        cout << "L1_SIZE:               " << levelOneSize << endl;
        cout << "L1_ASSOC:              " << levelOneAssoc << endl;
        cout << "L2_SIZE:               " << levelTwoSize << endl;
        cout << "L2_ASSOC:              " << levelTwoAssoc << endl;
        cout << "REPLACEMENT POLICY:    " << replacementPolicyStr << endl;
        cout << "INCLUSION PROPERTY:    " << inclusionPropertyStr << endl;
        cout << "trace_file:            " << fileName << endl;
        cout << "===== L1 contents =====" << endl;
        levelOne->printLevel();
        if(cacheSize > 1){
            cout << "===== L2 contents =====" << endl;
            levelTwo->printLevel();
        }
        cout << "===== Simulation results (raw) =====" << endl;
        levelOne->printMetrics(1);
        if(cacheSize > 1){
            levelTwo->printMetrics(2);
        }
        else{
            cout << "g. number of L2 reads:        0" << endl;
            cout << "h. number of L2 read misses:  0" << endl;
            cout << "i. number of L2 writes:       0" << endl;
            cout << "j. number of L2 write misses: 0" << endl;
            cout << "k. L2 miss rate:              0" << endl;
            cout << "l. number of L2 writebacks:   0" << endl;
        }
        if(cacheSize == 1){
            traffic = levelOne->trafficCalc();
        }
        else{
            traffic = levelTwo->trafficCalc();
        }
        cout << "m. total memory traffic:      " << traffic << endl; 
        
    }
};

int main(int argc, char* argv[]){ // Main function, takes in input from console in expected form.


    if(argc != 9){
        cerr << "Usage: " << argv[0] << " <BLOCKSIZE> <L1_SIZE> <L1_ASSOC> <L2_SIZE> <L2_ASSOC> <REPLACEMENT_POLICY> <INCLUSION_PROPERTY> <trace_file>" << endl;
        return 1;
    }

    int blockSize = stoi(argv[1]);
    int cacheSize = stoi(argv[2]);   
    int assoc = stoi(argv[3]);
    int cacheSizeTwo = stoi(argv[4]);
    int assocTwo = stoi(argv[5]);
    int replacement = stoi(argv[6]);
    int inclusion = stoi(argv[7]);
    string inputFile = argv[8];

    /*int blockSize = 16;  // Hard code for testing and validation
    int cacheSize = 1024;   
    int assoc = 2;
    int cacheSizeTwo = 8192;
    int assocTwo = 4;
    int replacement = 0;
    int inclusion = 1;
    string inputFile = "gcc_trace.txt";*/
    char operation;
    int memory;
    
    string input;
    ifstream ifp (inputFile);   

    Cache* cache = new Cache(cacheSize, cacheSizeTwo, blockSize, assoc, assocTwo, replacement, inclusion, inputFile); // instantiate cache

    if(ifp.is_open()){ // loop to read from file
        while(getline(ifp, input)){
            istringstream inputSplit(input);
            if(inputSplit >> operation >> hex >> memory){
                if(operation == 'r'){
                    cache->memoryAccess(memory, 0);
                }
                else if(operation == 'w'){
                    cache->memoryAccess(memory, 1);
                }
            }
            else{
                break;
            }
        }
        
    }
    else{
        cout << "File Failed To Open" << endl;
    }
    ifp.close();  // properly close file and delete cache, as well as print cache contents to the console.
    cache->printLevels();
    delete cache;
    return 0;
}


