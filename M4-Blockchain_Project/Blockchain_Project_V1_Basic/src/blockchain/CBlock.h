#ifndef __C_BLOCK_INCLUDED__
#define __C_BLOCK_INCLUDED__
#include <string>
#include <openssl/sha.h>

namespace blockchain
{
    class CBlock
    {
    private:
        uint8_t mHash[SHA256_DIGEST_LENGTH];            // Current hash
        uint8_t mPrevHash[SHA256_DIGEST_LENGTH];        // Previous hash
        CBlock* mPrevBlock;                             // Pointer to the previous block, will be null
        uint8_t* mData;                                 // Byte data of the transactions
        uint32_t mDataSize;                             // Size of the data
        time_t mCreatedTS;                              // Timestamp of the block creation
        uint32_t mNonce;                                // Nonce of the block
    
    public:
        CBlock(CBlock* prevBlock);                      // Constructor
        void calculateHash();                           // Calculates sha256 hash
        uint8_t* getHash();                             // Gets current hash -> mHash
        std::string getHashStr();                        // Gets the string representation of mHash
        CBlock* getPrevBlock();                         // Gets a pointer of the previous block
        void appendData(uint8_t* data, uint32_t size);  // Appends the data to mData
        bool isDifficulty(int difficulty);              // Difficulty
        void mine (int difficulty);                     // Mine the block
        uint32_t getNonce();                            // Gets the nonce value
    };
}

#endif