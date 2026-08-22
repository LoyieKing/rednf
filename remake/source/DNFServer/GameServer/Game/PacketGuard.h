#ifndef GAME_PACKETGUARD_H
#define GAME_PACKETGUARD_H

#include "InterfacePacketBuf.h"
#include "GameTypes.h"

// ORIG：PacketPool +0x00 DynamicPool<PacketBuf>*，+0x04 线程 ID（sizeof 8）。
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
    DynamicPool<PacketBuf>* m_pool;  // +0x00
    unsigned long m_threadID;        // +0x04
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
