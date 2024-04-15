#include "CChain.h"

using namespace blockchain;

CChain::CChain(int difficulty)
{
    mDifficulty = difficulty;
    mChain.push_back(CBlock(0));    // First block (genesis)
    mCurrentBlock = &mChain[0];
}

void CChain::appendToCurrentBlock(uint8_t* data, uint32_t size)
{
    mCurrentBlock->appendData(data, size);
}

void CChain::nextBlock()
{
    CBlock block(mCurrentBlock);
    block.mine(mDifficulty);
    mChain.push_back(block);
    mCurrentBlock = &mChain.back();
}

CBlock* CChain::getCurrentBlock()
{
    return mCurrentBlock;

    // if (!mChain.empty()) {
    //     return &mChain.back(); // Return a pointer to the last block in the chain
    // } else {
    //     return nullptr; // Return nullptr if the chain is empty
    // }
}