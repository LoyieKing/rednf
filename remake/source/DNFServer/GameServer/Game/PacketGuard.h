#ifndef GAME_PACKETGUARD_H
#define GAME_PACKETGUARD_H

#include "InterfacePacketBuf.h"

class PacketPool
{
public:
    PacketPool();
    ~PacketPool();
    PacketBuf* Acquire();
    void Free(PacketBuf* packet);
    unsigned long getThreadID();
    void setThreadID(unsigned long threadID);
private:
    char m_pad[8];
};

// ORIG 858dd4c/858de4a/858de80：InterfacePacketBuf 子类，sizeof=12。
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    explicit PacketGuard(PacketBuf* packetBuf);
    ~PacketGuard();

private:
    PacketPool* m_pool; // +0x04
    bool m_external;    // +0x08
};

TEST_CLASS_SIZE(PacketGuard, 12)

#endif
