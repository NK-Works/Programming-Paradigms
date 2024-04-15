#ifndef __C_CHAIN_INCLUDED__
#define __C_CHAIN_INCLUDED__
#include "CBlock.h"
#include <vector>

namespace blockchain
{
    class CChain
    {
    private:
        std::vector<CBlock> mChain;             // List of the blocks
        CBlock* mCurrentBlock;                  // Pointer to the current block &mChain.last()    
        int mDifficulty;                        // Difficulty
    
    public:
        CChain(int difficulty);                 // Constructor
        void appendToCurrentBlock(uint8_t* data, uint32_t size);    // Appending data to the current block
        void nextBlock();           // Continue to the next block
        CBlock* getCurrentBlock();  // Gets a pointer to the current block
    };
}

#endif