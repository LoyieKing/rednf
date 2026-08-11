#ifndef RELAYSERVICE_H_
#define RELAYSERVICE_H_

#include <deque>
#include <map>
#include <queue>
#include <vector>
#include <ext/hash_map>

#include "ThreadLock.h"
#include "Reactor.h"
#include "TCPUser.h"
#include "UDPUser.h"
#include "User.h"
#include "UserPool.h"
#include "TCPThread.h"
#include "UDPThread.h"
#include "TCPAcceptThread.h"
#include "TCPHandler.h"
#include "UDPHandler.h"

namespace RelayServiceApp
{

struct PacketHeader
{
    unsigned short m_type;     // +0
    unsigned short m_size;     // +2
    unsigned int m_reserved;   // +4
    unsigned int m_accId;      // +8
};

#pragma pack(push, 1)
struct PacketHeaderS2S
{
    PacketHeaderS2S(unsigned short a, unsigned short b)
    {
        m_a = a;
        m_b = b;
        m_c = 0;
        m_reserved = 0;
    }
    unsigned short m_a;       // +0
    unsigned short m_b;       // +2
    unsigned short m_c;       // +4
    unsigned int m_reserved;  // +6
    unsigned int m_f;         // +0xa
    unsigned char m_g;        // +0xe
};
#pragma pack(pop)

struct Packet_Relay_User_Check : public PacketHeaderS2S
{
    Packet_Relay_User_Check()
        : PacketHeaderS2S(0x9c4, 0xf)
    {
        m_f = 0;
        m_g = 0;
    }
};


// ---- PortInfo：mTCPPort@0 / mUDPPort@4 / mUDPS2SPort@8 / vector<int>@0xc（0x18）----
class PortInfo
{
public:
    PortInfo();
    ~PortInfo();
    int getTCPPort() const
    {
        return mTCPPort;
    }
    void setTCPPort(int port)
    {
        mTCPPort = port;
    }
    void setUDPPort(int port)
    {
        mUDPPort = port;
    }
    void setUDPS2SPort(int port)
    {
        mUDPS2SPort = port;
    }
    int getUDPS2SPort() const
    {
        return mUDPS2SPort;
    }

private:
    int mTCPPort;
    int mUDPPort;
    int mUDPS2SPort;
    std::vector<int> m_ports;
};

// ---- Handlers：3 个 handler 指针（0x18）----
class Handlers
{
public:
    Handlers();
    TCPHandlerRelay* getTCPHandlerRelay()
    {
        return m_tcpHandlerRelay;
    }
    TCPHandler* m_tcpHandler;       // +0
    UDPHandlerS2S* m_udpHandler;    // +4（实际放 UDPHandlerS2S）
    TCPHandlerRelay* m_tcpHandlerRelay;  // +8
};

// ---- Threads：TCPThread*@0 / UDPThread*@4 / UDPThread*@8 / vector<UDPThread*>@0xc ----
class Threads
{
public:
    Threads();
    ~Threads();
    TCPThread* m_tcpThread;
    UDPThread* m_udpThread;
    UDPThread* m_udpS2SThread;
    std::vector<UDPThread*> m_udpThreads;
};

// ---- Users：锁 + map + 2 个 hash_map（0xb8）----
class Users
{
public:
    Users();
    ~Users();
    void setMaxUserCount(int n);
    int getMaxUserCount() const
    {
        return m_maxUserCount;
    }
    TCPUser* getTCPUser(unsigned int acc_id);
    void setTCPUser(unsigned int acc_id, TCPUser* user);
    UDPUser* getUDPUser(unsigned int acc_id);
    void setUDPUser(unsigned int acc_id, UDPUser* user);
    void delUser(unsigned int acc_id);
    void increaseUserCount();
    void decreaseUserCount();
    int getUserCount() const
    {
        return m_currentUserCount;
    }
    int getCurrentMaxUserCount() const
    {
        return m_currentMaxUserCount;
    }
    float getAverageDispatchTime() const
    {
        return m_dispatchCount != 0
            ? (float)(m_totalDispatchTime / m_dispatchCount) : 0.0f;
    }
    int getMaxDispatchTime() const
    {
        return m_maxDispatchTime;
    }
    int getDispatchCout() const
    {
        return m_dispatchCount;
    }
    void clearCurrentMaxUserCount();
    void setDispatchTime(int t);
    void clearDispatchTime();

    int m_currentUserCount;                             // +0
    int m_currentMaxUserCount;                          // +4
    int m_maxUserCount;                                 // +8
    TThreadLock<ThreadLock_linux> m_lock1;              // +0xc
    int m_maxDispatchTime;                              // +0x24
    int m_totalDispatchTime;                            // +0x28
    int m_dispatchCount;                                // +0x2c
    std::map<TCPUser*, int> m_userMap;                  // +0x30
    TThreadLock<ThreadLock_linux> m_lock2;              // +0x48
    __gnu_cxx::hash_map<unsigned int, TCPUser*> m_tcpUsers;  // +0x60
    TThreadLock<ThreadLock_linux> m_lock3;              // +0x74
    __gnu_cxx::hash_map<unsigned int, UDPUser*> m_udpUsers;  // +0x8c
    TThreadLock<ThreadLock_linux> m_lock4;              // +0xa0
};


// ---- RelayService（独立类，0x1d8）----
class RelayService
{
public:
    RelayService();
    ~RelayService();
    void startup();
    void shutdown();
    void setAuthenticated(unsigned int acc_id);
    long long getTick() const
    {
        return m_tick;
    }
    long long getTickLog();
    void setTick();
    void setTickLog();
    void makeLog();
    void postDisconnectEvent2TCPUser(unsigned int acc_id, int flag);
    void disconnectEvent2TCPUser(TCPUser* user);
    void relayToTCP(PacketHeader* pkt);
    enum Mode
    {
        MODE_NONE = 0
    };
    void setMode(Mode mode)
    {
        m_mode = mode;
    }

    PortInfo m_portInfo;      // +0
    Handlers m_handlers;      // +0x18
    Users m_users;            // +0x24
    Threads m_threads;        // +0xdc
    UserPools m_userPools;    // +0xf4
    Reactor m_reactor;        // +0x184
    Mode m_mode;              // +0x1c4
    long long m_tick;         // +0x1c8
    long long m_tickLog;      // +0x1d0
};

} // namespace RelayServiceApp

#endif // RELAYSERVICE_H_
