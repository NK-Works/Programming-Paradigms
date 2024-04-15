#ifndef __C_CHAIN_INCLUDED__
#define __C_CHAIN_INCLUDED__
#include "CBlock.h"
#include "storage/EStorageType.h"
#include "storage/IStorage.h"
#include <vector>

namespace blockchain
{
    class CChain
    {
    private:
        std::vector<CBlock*> mChain;             // List of the blocks
        CBlock* mCurrentBlock;                  // Pointer to the current block &mChain.last()    
        int mDifficulty;                        // Difficulty
        storage::IStorage* mStorage;
    
    public:
        CChain(int difficulty, storage::E_STORAGE_TYPE storageType = storage::EST_NONE);                 // Constructor
        ~CChain();
        void appendToCurrentBlock(uint8_t* data, uint32_t size);    // Appending data to the current block
        void nextBlock(bool save  = true);           // Continue to the next block
        CBlock* getCurrentBlock();  // Gets a pointer to the current block
        void load();                // Load the blockchain
        size_t getBlockCount();     // Return the number of block
        bool isValid();             // If the chain is valid
    };
}

#endif