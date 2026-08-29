// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG 0x0822bb2e (C1 W) + 提取自 LC_Dispatchers.cpp:26 定义段（原 cpp 内定义，未改 cpp）。
#ifndef DNF_GAME_PACKET_REGISTER_TO_BLACKLIST_H_
#define DNF_GAME_PACKET_REGISTER_TO_BLACKLIST_H_

#include <cstring>

#include "PacketHeader.h"

// 0x2c；ctor W 0x0822bb2e：PacketHeader(0x5dc, 0x2c) 后清零 name。
// +0x00 PacketHeader(0xa packed) / +0x0a accId / +0x0e char name[0x1e]。
class Packet_Register_To_BlackList
{
public:
    Packet_Register_To_BlackList()
        : m_header(0x5dc, 0x2c)
    {
        std::memset(m_name, 0, sizeof(m_name));
    }

    PacketHeader m_header;    // +0x00
    unsigned int m_accId;     // +0x0a
    char m_name[0x1e];        // +0x0e
};  // sizeof == 0x2c

#endif  // DNF_GAME_PACKET_REGISTER_TO_BLACKLIST_H_
