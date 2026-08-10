#ifndef DNFPACKETBUFFER_H_
#define DNFPACKETBUFFER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"
#include <sys/socket.h>

#include "PacketHeader.h"

class CPacketBuffer;
class CTcpRecvBuffer;
class CTcpSendBuffer;
class CUdpRecvBuffer;

// from GuildDomain.h
class CTcpRecvBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    char m_data[0x1804];
};

// from GuildDomain.h
class CTcpSendBuffer
{
public:
    CTcpSendBuffer();
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    char m_data[0x1804];
};

// from GuildMisc.h
class CPacketBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    char m_data[0x1804];
};

// from GuildUdp.h
class CUdpRecvBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    char m_data[0x1804];
};

#endif
