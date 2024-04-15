/*  This code is made by Anneshu Nag, Student ID: 2210994760  */
#ifndef __I_NET_INCLUDED__
#define __I_NET_INCLUDED__
#include "CPacket.h"

namespace blockchain
{
    namespace net
    {
        class INet
        {
        public:
            int mSocket;                        // Socket handle
            CPacket recvPacket();               // Receive packet of data
            void sendPacket(CPacket* packet);   // Send packet of data
        protected:
            INet();
        private:
            const uint32_t mChunkSize = 2048;   // Chunk data size

            uint32_t recvUInt();
            void sendUInt(uint32_t num);
            int32_t recvInt();
            void sendInt(int32_t num);
            uint64_t recvUInt64();
            void sendUInt64(uint64_t num);
            uint8_t* recvDataAlloc(uint64_t size);
            void sendData(uint8_t* data, uint64_t size);
        };
    }
}

#endif