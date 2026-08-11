// df_dbmw_r - Peer (ORIG Peer.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

extern MemPool<CPeer> g_peerPool;

CPeer::CPeer()
{
    memset(&m_sendBuf, 0, 0x20);
}
CPeer::~CPeer()
{
    m_sendBuf = (char*)this + 0x1c;
    m_sendLen = 0;
    m_recvLen = 0;
    m_recvBuf = (char*)this + 0x183c;
    m_remainSendLen = 0;
}
void CPeer::InitPeer(TcpRecvQueue* recvQ, CMutex* qLock, CMutex* bLock)
{
    m_recvQ = recvQ;
    m_sendQLock = qLock;
    m_sendBLock = bLock;
    m_sendBuf = (char*)this + 0x1c;
    m_sendLen = 0;
    m_recvLen = 0;
    m_recvBuf = (char*)this + 0x183c;
    m_remainSendLen = 0;
}
void CPeer::ConnSig()
{
    Packet_InnerPakcet_Login pkt;
    int fd = getHandle();
    CTcpRecvBuffer* buf;
    {
        CGuard<CMutex> guard(m_sendBLock);
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard(m_sendQLock);
        m_recvQ->push(buf);
    }
}
void CPeer::DisConnSig()
{
    Packet_InnerPakcet_Logout pkt;
    int fd = getHandle();
    CTcpRecvBuffer* buf;
    {
        CGuard<CMutex> guard(m_sendBLock);
        buf = new CTcpRecvBuffer;
    }
    memcpy(buf, &pkt, pkt.packetSize);
    {
        CGuard<CMutex> guard(m_sendQLock);
        m_recvQ->push(buf);
    }
}
int CPeer::recv_packet()
{
    if (getHandle() < 0)
        return 0;
    errno = 0;
    int remaining = ((char*)this + 0x1c + 0x1800) - m_sendBuf;
    if (remaining == 0)
    {
        m_sendBuf = (char*)this + 0x1c;
        m_recvLen = 0;
        remaining = 0x1800;
    }
    int n = read(getHandle(), m_sendBuf, remaining);
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR)
            return 0;
        if (errno != 0)
        {
            printf("RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
                   getHandle(), errno, strerror(errno));
            return -1;
        }
        return 0;
    }
    if (n == 0)
    {
        CMyFileLog log(__FUNCTION__, 0xa4);
        log("./log/TcpRecv", "Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)",
            errno, strerror(errno), remaining, n);
        return -1;
    }
    return n;
}
int CPeer::send_packet()
{
    if (m_remainSendLen == 0)
        return 1;
    int ret = write(getHandle(), (char*)this + 0x183c, m_remainSendLen);
    if (ret <= 0)
    {
        if (errno == EAGAIN || errno == EINTR)
            return 1;
        if (errno != 0)
        {
            printf("SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)",
                   getHandle(), errno, strerror(errno));
            return 1;
        }
        return ret;
    }
    if (m_remainSendLen <= ret)
    {
        if (m_remainSendLen < ret)
        {
            printf("offset error[Remain_Data: %d Send:%d]", m_remainSendLen, ret);
            return -1;
        }
        m_recvBuf = (char*)this + 0x183c;
        m_remainSendLen = 0;
        return ret;
    }
    m_recvBuf = (char*)this + 0x183c + ret;
    m_remainSendLen -= ret;
    if (m_remainSendLen > 0x96000)
    {
        CMyFileLog log(__FUNCTION__, 0x17e);
        log("./log/TcpErr", "m_remain_sendlen < MAX_PACKET_SIZE_UDP :  m_remain_sendlen:%d]",
            m_remainSendLen);
        m_recvBuf = (char*)this + 0x183c;
        m_remainSendLen = 0;
        return 1;
    }
    memmove((char*)this + 0x183c, m_recvBuf, m_remainSendLen);
    m_recvBuf = (char*)this + 0x183c + m_remainSendLen;
    return ret;
}
int CPeer::send_packet(char* buf, int len)
{
    if (getHandle() < 0)
        return -1;
    if (len <= 0)
    {
        printf("!!!Send Packet[(%d,%d) Size(%d) Error\n", buf[0], buf[1], len);
        return -1;
    }
    errno = 0;
    m_remainSendLen += len;
    if (m_remainSendLen > 0x96000)
    {
        CMyFileLog log(__FUNCTION__, 0x133);
        log("./log/TcpErr", "!!!Send Packet Overflow P_TYPE[%d] Size:Remain[%d] Last[%d]",
            buf[1], m_remainSendLen, len);
        m_recvBuf = (char*)this + 0x183c;
        m_remainSendLen = 0;
        return -1;
    }
    if (m_recvBuf < (char*)this + 0x183c ||
        m_recvBuf >= (char*)this + 0x183c + 0x96000)
    {
        CMyFileLog log(__FUNCTION__, 0x13b);
        log("./log/TcpErr", "!!!Send Packet Buffer critical error P_TYPE[%d] Size:Remain[%d] Last[%d]",
            buf[1], m_remainSendLen, len);
        m_recvBuf = (char*)this + 0x183c;
        m_remainSendLen = 0;
        return -1;
    }
    memcpy(m_recvBuf, buf, len);
    m_recvBuf += len;
    return send_packet();
}
bool CPeer::parsing(int len)
{
    int parsinglength = m_recvLen + len;
    if (parsinglength <= 9)
    {
        m_recvLen += len;
        m_sendBuf += len;
        CMyFileLog log(__FUNCTION__, 0xbb);
        log("./log/TcpRecv", "(offset:%x - buf:%x) = remainlen:%d, Recv Size[%d] ",
            (char*)this + 0x1c, m_sendBuf, m_recvLen, len);
        return 1;
    }
    for (;;)
    {
        if (m_recvLen != 0)
            m_sendBuf -= m_recvLen;
        PacketHeader hdr(0, 0);
        memcpy(&hdr, m_sendBuf, 10);
        int size = hdr.packetSize;
        if (size <= 9 || size > 0x1800)
        {
            CMyFileLog log(__FUNCTION__, 0xd0);
            log("./log/TcpRecv",
                "Recv Size[%d], Parsing Packet Size[%d] is Too Large, offset:%x, buf:%x, alreadyRead:%d",
                len, size, m_sendBuf, (char*)this + 0x1c, m_sendLen);
            m_sendBuf = (char*)this + 0x1c;
            m_recvLen = 0;
            return 0;
        }
        if (parsinglength < size)
        {
            CMyFileLog log(__FUNCTION__, 0x100);
            log("./log/TcpRecv",
                "need more data (packetsize > (unsigned int)parsinglength): body=%d !!",
                parsinglength);
            break;
        }
        CTcpRecvBuffer* buf;
        {
            CGuard<CMutex> guard(m_sendBLock);
            buf = new CTcpRecvBuffer;
        }
        memcpy(buf, m_sendBuf, size);
        *(int*)((char*)buf + 6) = getHandle();
        {
            CGuard<CMutex> guard(m_sendQLock);
            m_recvQ->push(buf);
        }
        parsinglength -= size;
        m_sendBuf += size;
        m_recvLen = 0;
        if (parsinglength == 0)
        {
            m_sendBuf = (char*)this + 0x1c;
            break;
        }
        if (parsinglength <= 9)
        {
            CMyFileLog log(__FUNCTION__, 0xf8);
            log("./log/TcpRecv",
                "need more data (parsinglength < HEADER_SIZE): body=%d !!",
                parsinglength);
            break;
        }
    }
    if (parsinglength > 0)
    {
        if (parsinglength > 0x1800)
        {
            CMyFileLog log(__FUNCTION__, 0x10e);
            log("./log/TcpRecv",
                "[PARSING LENGTH EXCEPTION] parsinglength > MAX_RECV_BUF , memmove : parsinglength = %d",
                parsinglength);
            return 0;
        }
        memmove((char*)this + 0x1c, m_sendBuf, parsinglength);
        m_recvLen = parsinglength;
        m_sendBuf = (char*)this + 0x1c + parsinglength;
    }
    return 1;
}
bool CPeer::RecvPacket()
{
    int ret = recv_packet();
    if (ret > 0)
    {
        if (!parsing(ret))
        {
            CMyFileLog log(__FUNCTION__, 0x4d);
            log("./log/TcpRecv", "CPeer::Recv (false == parsing( size:%d ) )\n", ret);
            printf("CPeer::Recv (false == parsing( size:%d ) )\n", ret);
            return 1;
        }
        return 1;
    }
    if (ret < 0)
    {
        CMyFileLog log(__FUNCTION__, 0x59);
        log("./log/TcpRecv",
            "Maybe Peer is disconnect!(%d), socket no(%d), addr(%s), port(%d)",
            ret, getHandle(), getPeerAdrs(), getPeerPort());
        printf("CPeer::Recv (size(%d) < 0)\n", ret);
        return 0;
    }
    CMyFileLog log(__FUNCTION__, 0x63);
    log("./log/TcpRecv", "Maybe Peer is disconnect!(size == 0)");
    puts("CPeer::Recv (size == 0)");
    return 1;
}
TCPSocket* CPeer::GetTcpSocket() { return this; }
int CPeer::get_remain_sendlen() { return m_remainSendLen; }
void* CPeer::operator new(unsigned int size) { return g_peerPool.alloc(); }
void CPeer::operator delete(void* ptr) { g_peerPool.free(ptr); }
void CPeer::operator delete(void* ptr, unsigned int size) { g_peerPool.free(ptr, size); }
