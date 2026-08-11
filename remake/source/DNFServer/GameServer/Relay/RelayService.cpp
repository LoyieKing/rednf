// df_relay_r — RelayService（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include "RelayService.h"
#include "TCPUser.h"
#include "UDPUser.h"
#include "UserPool.h"
#include "Socket.h"
#include "Helper.h"
#include "ScriptData.h"
#include "Globals.h"

namespace RelayServiceApp
{

PortInfo::PortInfo()
{
    mTCPPort = 0;
    mUDPPort = 0;
    mUDPS2SPort = 0;
}

PortInfo::~PortInfo()
{
}

// ---- Handlers ----


Handlers::Handlers()
{
    m_tcpHandler = 0;
    m_udpHandler = 0;
    m_tcpHandlerRelay = 0;
}


// ---- Threads ----

Threads::Threads()
{
    m_tcpThread = 0;
    m_udpThread = 0;
    m_udpS2SThread = 0;
}

Threads::~Threads()
{
}


// ---- Users ----

Users::Users()
    : m_currentUserCount(0), m_currentMaxUserCount(0), m_maxUserCount(0),
      m_maxDispatchTime(0), m_totalDispatchTime(0), m_dispatchCount(0)
{
}

Users::~Users()
{
}

void Users::setMaxUserCount(int n)
{
    m_maxUserCount = n;
}

TCPUser* Users::getTCPUser(unsigned int acc_id)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock3);
    __gnu_cxx::hash_map<unsigned int, TCPUser*>::iterator it = m_tcpUsers.find(acc_id);
    if (it == m_tcpUsers.end())
    {
        return 0;
    }
    TCPUser* user = (*it).second;
    return user;
}

void Users::setTCPUser(unsigned int acc_id, TCPUser* user)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock3);
    m_tcpUsers[acc_id] = user;
}

UDPUser* Users::getUDPUser(unsigned int acc_id)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock4);
    __gnu_cxx::hash_map<unsigned int, UDPUser*>::iterator it = m_udpUsers.find(acc_id);
    if (it == m_udpUsers.end())
    {
        return 0;
    }
    UDPUser* user = (*it).second;
    return user;
}

void Users::setUDPUser(unsigned int acc_id, UDPUser* user)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock4);
    m_udpUsers[acc_id] = user;
}

void Users::delUser(unsigned int acc_id)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock3);
    m_tcpUsers.erase(acc_id);
}

void Users::increaseUserCount()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock1);
    if (m_currentMaxUserCount < ++m_currentUserCount)
    {
        m_currentMaxUserCount = m_currentUserCount;
    }
}

void Users::decreaseUserCount()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_lock1);
    if (m_currentMaxUserCount < --m_currentUserCount)
    {
        m_currentMaxUserCount = m_currentUserCount;
    }
}

void Users::clearCurrentMaxUserCount()
{
    m_currentMaxUserCount = 0;
}

void Users::setDispatchTime(int t)
{
    if (m_maxDispatchTime < t)
    {
        m_maxDispatchTime = t;
    }
    m_totalDispatchTime += t;
    m_dispatchCount++;
}

void Users::clearDispatchTime()
{
    m_dispatchCount = 0;
    m_totalDispatchTime = m_dispatchCount;
    m_maxDispatchTime = m_totalDispatchTime;
}


// ---- RelayService ----

RelayService::RelayService()
{
    m_mode = MODE_NONE;
    m_tick = 0;
    m_tickLog = 0;
}

RelayService::~RelayService()
{
}

void RelayService::startup()
{
    createFileLogInfo();
    createFileLogWarn();
    createFileLogError();
    createFileLogCri();
    createLogInfo();
    createLogCri();
    createLogWarn();
    createLogError();
    m_userPools.m_tcpSocketPool.startup(G_ScriptData()->mRelayNum + 0x69);
    m_userPools.m_tcpUserPool.startup(G_ScriptData()->mRelayNum + 0x69);
    m_users.setMaxUserCount(G_ScriptData()->mRelayNum);
    m_threads.m_tcpThread = new TCPThread;
    m_threads.m_tcpThread->setManager(this);
    m_threads.m_tcpThread->setPort(m_portInfo.getTCPPort());
    if (G_ScriptData()->mFlag)
    {
        m_threads.m_udpS2SThread = new UDPThread;
        m_threads.m_udpS2SThread->setManager(this);
        m_threads.m_udpS2SThread->setPort(m_portInfo.getUDPS2SPort());
    }
    m_handlers.m_tcpHandlerRelay = new TCPHandlerRelay;
    m_handlers.m_tcpHandlerRelay->setManager(this);
    if (G_ScriptData()->mFlag)
    {
        m_handlers.m_udpHandler = new UDPHandlerS2S;
        m_handlers.m_udpHandler->setManager(this);
    }
    m_threads.m_tcpThread->setHandler(m_handlers.m_tcpHandlerRelay);
    if (G_ScriptData()->mFlag)
    {
        m_threads.m_udpS2SThread->setHandler(m_handlers.m_udpHandler);
    }
    m_threads.m_tcpThread->begin();
    if (G_ScriptData()->mFlag)
    {
        m_threads.m_udpS2SThread->begin();
    }
}

void RelayService::shutdown()
{
    if (G_ScriptData()->mFlag && m_threads.m_udpS2SThread != 0)
    {
        m_threads.m_udpS2SThread->waitForTerminated(100);
    }
    if (m_threads.m_tcpThread != 0)
    {
        m_threads.m_tcpThread->waitForTerminated(100);
    }
    if (m_handlers.m_tcpHandlerRelay != 0)
    {
        delete m_handlers.m_tcpHandlerRelay;
    }
    m_handlers.m_tcpHandlerRelay = 0;
    if (G_ScriptData()->mFlag)
    {
        if (m_handlers.m_udpHandler != 0)
        {
            operator delete(m_handlers.m_udpHandler);
        }
        m_handlers.m_udpHandler = 0;
        if (m_threads.m_udpS2SThread != 0)
        {
            delete m_threads.m_udpS2SThread;
        }
        m_threads.m_udpS2SThread = 0;
    }
    if (m_threads.m_tcpThread != 0)
    {
        delete m_threads.m_tcpThread;
    }
    m_threads.m_tcpThread = 0;
    m_userPools.m_tcpUserPool.shutdown();
    m_userPools.m_tcpSocketPool.shutdown();
    destroyFileLogInfo();
    destroyFileLogWarn();
    destroyFileLogError();
    destroyFileLogCri();
    destroyLogInfo();
    destroyLogCri();
    destroyLogWarn();
    destroyLogError();
}

void RelayService::setAuthenticated(unsigned int acc_id)
{
    if (!G_ScriptData()->mFlag)
    {
        return;
    }
    UDPSocket* udp = m_threads.m_udpS2SThread->getUDPSocket();
    if (udp != 0)
    {
        udp->pushMonitorAuthPacket(acc_id);
    }
    return;
}

void RelayService::setTick()
{
    m_tick = get_ms_tick();
}

void RelayService::setTickLog()
{
    m_tickLog = m_tick;
}

void RelayService::makeLog()
{
    long long tick = getTick();
    long long diff = tick - getTickLog();
    if (getTickLog() != 0 && diff >= 0xea61)
    {
        setTickLog();
        time_t t = (time_t)(tick / 1000);
        tm* tm_now = localtime(&t);
        char filename[256] = {0};
        snprintf(filename, 0x100, "./log/Relay%4d%02d%02d_T%d.log",
                 tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                 m_portInfo.getTCPPort());
        FILE* f = fopen(filename, "a+");
        if (f != 0)
        {
            if (G_ScriptData()->mFlag)
            {
                UDPSocket* udp = m_threads.m_udpS2SThread->getUDPSocket();
                if (udp == 0)
                {
                    fprintf(f,
                        "%02d/%02d/%02d %02d:%02d:%02d Current User: %d\tError UDPS2SSocket!! Not Auth mode\tDispatch Time: MAX = %d, AVG = %.2f, CNT = %d\n",
                        tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                        tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec,
                        m_users.getCurrentMaxUserCount() == 0 ? -m_users.getUserCount() : m_users.getCurrentMaxUserCount(),
                        m_users.getMaxDispatchTime(), m_users.getAverageDispatchTime(), m_users.getDispatchCout());
                }
                else
                {
                    fprintf(f,
                        "%02d/%02d/%02d %02d:%02d:%02d Current User: %d\tAuth Packet Queue: %d\tDispatch Time: MAX = %d, AVG = %.2f, CNT = %d\n",
                        tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                        tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec,
                        m_users.getCurrentMaxUserCount() == 0 ? -m_users.getUserCount() : m_users.getCurrentMaxUserCount(),
                        udp->sizeMonitorAuthPacket(), m_users.getMaxDispatchTime(),
                        m_users.getAverageDispatchTime(), m_users.getDispatchCout());
                }
            }
            else
            {
                fprintf(f,
                    "%02d/%02d/%02d %02d:%02d:%02d Current User: %d Dispatch Time: MAX = %d, AVG = %.2f, CNT = %d\n",
                    tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday,
                    tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec,
                    m_users.getCurrentMaxUserCount() == 0 ? -m_users.getUserCount() : m_users.getCurrentMaxUserCount(),
                    m_users.getMaxDispatchTime(), m_users.getAverageDispatchTime(), m_users.getDispatchCout());
            }
            fclose(f);
            m_users.clearCurrentMaxUserCount();
            m_users.clearDispatchTime();
        }
    }
    else
    {
        if (getTickLog() == 0)
        {
            setTickLog();
        }
    }
}

void RelayService::postDisconnectEvent2TCPUser(unsigned int acc_id, int flag)
{
    if (acc_id != 0)
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_users.m_lock3);
        __gnu_cxx::hash_map<unsigned int, TCPUser*>::iterator it = m_users.m_tcpUsers.find(acc_id);
        if (it != m_users.m_tcpUsers.end())
        {
            (*it).second->postDisconnected(flag);
        }
    }
}

void RelayService::disconnectEvent2TCPUser(TCPUser* user)
{
    unsigned int acc_id = user->getACCID();
    if (acc_id != 0)
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > scoped(m_users.m_lock3);
        m_users.m_tcpUsers.erase(acc_id);
    }
    if (G_ScriptData()->mFlag)
    {
        m_threads.m_udpS2SThread->getUDPSocket()->delDisconnectUser(acc_id);
    }
    RelayTReactor* reactor = (RelayTReactor*)m_reactor.getReactor();
    reactor->unregistHandle(user);
    m_userPools.destroyTCPUser(user);
    m_users.decreaseUserCount();
}

void RelayService::relayToTCP(PacketHeader* pkt)
{
    pkt->m_type = 3;
    TCPUser* user = m_users.getTCPUser(pkt->m_accId);
    if (user != 0)
    {
        int r = user->send(pkt);
    }
}


} // namespace RelayServiceApp
