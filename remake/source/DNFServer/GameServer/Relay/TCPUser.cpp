// df_relay_r — TCPUser（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <string.h>

#include "TCPUser.h"
#include "RelayService.h"
#include "Helper.h"

namespace RelayServiceApp
{
// ---- TCPUser ----

TCPUser::TCPUser()
    : m_accId(0), m_kind(4), m_isDisconnected(false),
      m_isAboutToDisconnect(false), m_lastAccessTime(0), m_sock(0)
{
}

TCPUser::~TCPUser()
{
}

void TCPUser::setACCID(unsigned int acc_id)
{
    m_accId = acc_id;
}

int TCPUser::getHandle()
{
    if (m_sock != 0)
    {
        return m_sock->getHandle();
    }
    return 0;
}

TCPSocket* TCPUser::getSocket()
{
    return m_sock;
}

void TCPUser::setSocket(TCPSocket* sock)
{
    m_sock = sock;
}

bool TCPUser::isAboutToDisconnect() const
{
    return m_isAboutToDisconnect;
}

bool TCPUser::isDisconnected() const
{
    return m_isDisconnected;
}

bool TCPUser::isIdle() const
{
    long long now = get_ms_tick();
    long long diff = now - m_lastAccessTime;
    if (m_lastAccessTime != 0)
    {
        if (30000 < diff)
        {
            return true;
        }
        if ((m_accId == 0) && (5000 < diff))
        {
            return true;
        }
    }
    return false;
}

bool TCPUser::isToWrite() const
{
    int n = m_sendQueue.getPushedLength();
    return 0 < n;
}

void TCPUser::setLastAccessTime()
{
    m_lastAccessTime = get_ms_tick();
}

void TCPUser::postDisconnected(int flag)
{
    m_isAboutToDisconnect = true;
    m_kind = flag;
}

void TCPUser::notifyCannotLoginByMaxUserCount()
{
}

void TCPUser::onRead()
{
    if (m_sock == 0 || m_sock->getHandle() < 0)
    {
        postDisconnected(1);
        return;
    }
    if (isAboutToDisconnect() || isDisconnected())
    {
        return;
    }
    onRead_();
}

void TCPUser::onRead_()
{
    register int avail = m_recvQueue.getAvailableSpace();
    int r = m_sock->recv(m_recvQueue.peekPush(), avail);
    if (r > 0)
    {
        int pidx = m_recvQueue.pushIndex(r);
        if (pidx < 0)
        {
            postDisconnected(1);
            return;
        }
    }
    else
    {
        if (r < 0)
        {
            postDisconnected(1);
            return;
        }
    }
    onPacketParse();
}

void TCPUser::onWrite()
{
    if (m_sock == 0)
    {
        return;
    }
    if (m_sock->getHandle() < 0)
    {
        return;
    }
    if (isAboutToDisconnect() || isDisconnected())
    {
        return;
    }
    while (!m_sendQueue.isEmpty())
    {
        if (m_sendQueue.isPushGreaterThanPop())
        {
            int len = m_sendQueue.getPushedLength();
            if (0 < len)
            {
                int r = m_sock->send(m_sendQueue.peekPop(), len);
                if (r > 0)
                {
                    m_sendQueue.pop(r);
                }
                else
                {
                    if (-1 < r)
                    {
                        return;
                    }
                    postDisconnected(0);
                    return;
                }
            }
        }
        else
        {
            int len = m_sendQueue.getPopLengthToEnd();
            if (0 < len)
            {
                int r = m_sock->send(m_sendQueue.peekPop(), len);
                if (r > 0)
                {
                    m_sendQueue.pop(r);
                }
                else
                {
                    if (-1 < r)
                    {
                        return;
                    }
                    postDisconnected(0);
                    return;
                }
            }
        }
    }
}

void TCPUser::onError()
{
    onClose();
}

void TCPUser::onClose()
{
    if (m_isDisconnected)
    {
        return;
    }
    getManager()->disconnectEvent2TCPUser(this);
    if (m_sock != 0)
    {
        m_sock->close();
    }
    m_isDisconnected = true;
}

void TCPUser::onAccept()
{
    m_recvQueue.clear();
    m_sendQueue.clear();
}

void TCPUser::shutdown()
{
    m_accId = 0;
    m_isDisconnected = false;
    m_isAboutToDisconnect = false;
    m_sock = 0;
}

void TCPUser::startupAfterSetSocket()
{
    m_accId = 0;
    m_isDisconnected = false;
    m_isAboutToDisconnect = false;
}

int TCPUser::postSendPacket(char* buf)
{
    return 0;
}

int TCPUser::send(PacketHeader* buf)
{
    if (m_sock == 0)
    {
        return -1;
    }
    if (m_sock->getHandle() < 0)
    {
        return -1;
    }
    if (isAboutToDisconnect() || isDisconnected())
    {
        return -2;
    }
    if (buf->m_size == 0)
    {
        return -3;
    }
    int r;
    if ((int)m_sendQueue.getPushedLength() > 0)
    {
        if ((int)(m_sendQueue.getPushedLength() + buf->m_size) > 0xc7ff)
        {
            return -4;
        }
        r = m_sendQueue.push((char*)buf, buf->m_size);
        if (r < 0)
        {
            return -5;
        }
        return 0;
    }
    int r2 = m_sock->send((char*)buf, buf->m_size);
    if (r2 != (int)buf->m_size)
    {
        if ((int)(m_sendQueue.getPushedLength() + (buf->m_size - r2)) > 0xc7ff)
        {
            return -4;
        }
        int r3 = m_sendQueue.push((char*)buf + r2, buf->m_size - r2);
        if (r3 < 0)
        {
            return -5;
        }
    }
    return 0;
}

void TCPUser::onPacketParse()
{
    do
    {
        if (m_recvQueue.isPopStraight(0xc))
        {
            char* p = m_recvQueue.peekPop();
            PacketHeader* pkt = (PacketHeader*)p;
            int size = pkt->m_size;
            if ((pkt->m_type != 0) && (pkt->m_type != 1))
            {
                postDisconnected(1);
                return;
            }
            if (size > 0)
            {
                if (m_recvQueue.isPopStraight(size))
                {
                    long long t1 = get_ms_tick();
                    getManager()->m_handlers.getTCPHandlerRelay()->dispatch(this, p, 0, size);
                    if (pkt->m_type == 0 && getACCID() == 0)
                    {
                        postDisconnected(1);
                        return;
                    }
                    m_recvQueue.pop(size);
                    long long t2 = get_ms_tick();
                    getManager()->m_users.setDispatchTime((int)(t2 - t1));
                }
                else
                {
                    if ((int)m_recvQueue.getPushedLength() < size)
                    {
                        return;
                    }
                    char* buf = (char*)calloc(size, 1);
                    if (buf == 0)
                    {
                        return;
                    }
                    bool ok = m_recvQueue.popCopy(size, buf);
                    if (!ok)
                    {
                        free(buf);
                        return;
                    }
                    long long t1 = get_ms_tick();
                    getManager()->m_handlers.getTCPHandlerRelay()->dispatch(this, buf, 0, size);
                    if (pkt->m_type == 0 && getACCID() == 0)
                    {
                        free(buf);
                        postDisconnected(1);
                        return;
                    }
                    long long t2 = get_ms_tick();
                    getManager()->m_users.setDispatchTime((int)(t2 - t1));
                    free(buf);
                }
            }
            else
            {
                postDisconnected(1);
                return;
            }
        }
        else
        {
            if (m_recvQueue.getPushedLength() <= 0xb)
            {
                return;
            }
            // round-11：1 字节栈槽使 header 落在 -0x69（ORIG 布局），
            // 帧 0x94 对齐；仅布局占位，不参与逻辑。
            char header_pad[1];
            char header[0xc];
            if (m_recvQueue.peekCopy(0xc, header) != 0)
            {
                char* p = header;
                int size = ((PacketHeader*)p)->m_size;
                if ((((PacketHeader*)p)->m_type != 0) && (((PacketHeader*)p)->m_type != 1))
                {
                    postDisconnected(1);
                    return;
                }
                if (size > 0)
                {
                    if ((int)m_recvQueue.getPushedLength() >= size)
                    {
                        char* buf = (char*)calloc(size, 1);
                        if (buf == 0)
                        {
                            return;
                        }
                        bool ok = m_recvQueue.popCopy(size, buf);
                        if (!ok)
                        {
                            free(buf);
                            postDisconnected(1);
                            return;
                        }
                        long long t1 = get_ms_tick();
                        getManager()->m_handlers.getTCPHandlerRelay()->dispatch(this, buf, 0, size);
                        if (((PacketHeader*)p)->m_type == 0 && getACCID() == 0)
                        {
                            free(buf);
                            postDisconnected(1);
                            return;
                        }
                        long long t2 = get_ms_tick();
                        getManager()->m_users.setDispatchTime((int)(t2 - t1));
                        free(buf);
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    postDisconnected(1);
                    return;
                }
            }
            else
            {
                postDisconnected(1);
                return;
            }
        }
        if (isAboutToDisconnect())
        {
            return;
        }
    } while (m_recvQueue.isEmpty() == 0);
    return;
}


} // namespace RelayServiceApp
