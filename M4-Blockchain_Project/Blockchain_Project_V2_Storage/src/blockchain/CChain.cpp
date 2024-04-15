#include "CChain.h"
#include "storage/storage.h"

namespace blockchain
{
    CChain::CChain(int difficulty, storage::E_STORAGE_TYPE storageType)
    {
        mDifficulty = difficulty;
        mStorage = storage::createStorage(storageType); // Initialised the storage
        CBlock* block = new CBlock(0);
        mChain.push_back(block);    // First block (genesis)
        block->mine(mDifficulty);
        mCurrentBlock = block;
        load();
    }

    CChain::~CChain()
    {
        mStorage->dispose();
        for(std::vector<CBlock*>::iterator it = mChain.begin(); it != mChain.end(); ++it)
        {
            delete (*it);
        }
    }

    void CChain::appendToCurrentBlock(uint8_t* data, uint32_t size)
    {
        mCurrentBlock->appendData(data, size);
    }

    void CChain::nextBlock(bool save)
    {
        mCurrentBlock->calculateHash();
        if(save)
            mStorage->save(mCurrentBlock, mChain.size());
        CBlock* block = new CBlock(mCurrentBlock);
        mChain.push_back(block);
        block->mine(mDifficulty);
        
        mCurrentBlock = block;
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

    void CChain::load()
    {
        mStorage->loadChain(&mChain);
        mCurrentBlock = mChain.back();
        if(mChain.size() > 1)
            nextBlock(false);
    }

    size_t CChain::getBlockCount()
    {
        return mChain.size();
    }

    bool CChain::isValid()
    {
        CBlock* cur = mCurrentBlock;
        while (cur = cur->getPrevBlock())
        {
            if(!cur->isValid())
                return false;
        }
        return true;
    }    
}
