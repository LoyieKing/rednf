// df_guild_r — DNFSignalTranslator (split from source/guild per ORIG layout)
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include "DNFFileLog.h"

#include "DNFSignalTranslator.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

CSignalTranslator* CSignalTranslatorInstance()
{
    static CSignalTranslator instance;
    return &instance;
}

CSignalTranslator::CSignalTranslator()
{
}

CSignalTranslator::~CSignalTranslator()
{
}

void CSignalTranslator::clear()
{
    if (m_handlers[0x10 / 4] != 0)
    {
        delete m_handlers[0x10 / 4];
        m_handlers[0x10 / 4] = 0;
    }
    if (m_handlers[0x28 / 4] != 0)
    {
        delete m_handlers[0x28 / 4];
        m_handlers[0x28 / 4] = 0;
    }
    if (m_handlers[0x30 / 4] != 0)
    {
        delete m_handlers[0x30 / 4];
        m_handlers[0x30 / 4] = 0;
    }
    if (m_handlers[0x18 / 4] != 0)
    {
        delete m_handlers[0x18 / 4];
        m_handlers[0x18 / 4] = 0;
    }
    if (m_handlers[0x3c / 4] != 0)
    {
        delete m_handlers[0x3c / 4];
        m_handlers[0x3c / 4] = 0;
    }
}

int CSignalTranslator::init(CApplication* app)
{
    try
    {
        init_signal();
        return init_handler(app);
    }
    catch (CDNFException& e)
    {
        printf("CSignalTranslator Exception Break : %s\n", e.what());
        throw;
    }
    catch (...)
    {
        puts("CSignalTranslator Exception Break");
        throw;
    }
}

void CSignalTranslator::init_signal()
{
    if (!regist_signal(0xf, signal_handler))
    {
        throw CDNFException("regist_signal():SIGTERM");
    }
    if (!regist_signal(10, signal_handler))
    {
        throw CDNFException("regist_signal():SIGUSR1");
    }
    if (!regist_signal(0xc, signal_handler))
    {
        throw CDNFException("regist_signal():SIGUSR2");
    }
    if (!regist_signal(2, (void (*)(int))1))
    {
        throw CDNFException("regist_signal():SIGINT");
    }
    if (!regist_signal(0xb, signal_handler))
    {
        throw CDNFException("regist_signal():SIGSEGV");
    }
    if (!regist_signal(8, signal_handler))
    {
        throw CDNFException("regist_signal():SIGFPE");
    }
    if (!regist_signal(0xd, (void (*)(int))1))
    {
        throw CDNFException("regist_signal():SIGPIPE");
    }
    if (!regist_signal(4, signal_handler))
    {
        throw CDNFException("regist_signal():SIGILL");
    }
    if (!regist_signal(7, signal_handler))
    {
        throw CDNFException("regist_signal():SIGBUS");
    }
    if (!regist_signal(0x10, signal_handler))
    {
        throw CDNFException("regist_signal():SIGSTKFLT");
    }
    if (!regist_signal(0x17, signal_handler))
    {
        throw CDNFException("regist_signal():SIGURG");
    }
    if (!regist_signal(0x18, signal_handler))
    {
        throw CDNFException("regist_signal():SIGXCPU");
    }
    if (!regist_signal(0x19, signal_handler))
    {
        throw CDNFException("regist_signal():SIGXFSZ");
    }
    if (!regist_signal(0x1f, signal_handler))
    {
        throw CDNFException("regist_signal():SIGSYS");
    }
}

int CSignalTranslator::init_handler(CApplication* app)
{
    m_handlers[0x3c / 4] = new CTerminateSig;
    m_handlers[0x3c / 4]->attachApp(app);
    for (int i = 0; i < 0x1a; i++)
    {
        m_handlers[i] = m_handlers[0x3c / 4];
    }
    m_handlers[0x18 / 4] = new CSegmentationFaultSig;
    m_handlers[0x18 / 4]->attachApp(app);
    m_handlers[0x2c / 4] = m_handlers[0x18 / 4];
    m_handlers[0x20 / 4] = m_handlers[0x18 / 4];
    m_handlers[2] = m_handlers[0x18 / 4];
    m_handlers[0x28 / 4] = new CUser1Sig;
    m_handlers[0x28 / 4]->attachApp(app);
    m_handlers[0x30 / 4] = new CUser2Sig;
    m_handlers[0x30 / 4]->attachApp(app);
    m_handlers[0x10 / 4] = new CSystemFailSig;
    m_handlers[0x10 / 4]->attachApp(app);
    m_handlers[0x1c / 4] = m_handlers[0x10 / 4];
    m_handlers[0x5c / 4] = m_handlers[0x10 / 4];
    m_handlers[0x40 / 4] = m_handlers[0x10 / 4];
    m_handlers[0x60 / 4] = m_handlers[0x10 / 4];
    m_handlers[100 / 4] = m_handlers[0x10 / 4];
    m_handlers[0x7c / 4] = m_handlers[0x10 / 4];
    return 0;
}

int CSignalTranslator::regist_signal(int sig, void (*handler)(int))
{
    struct sigaction sa;
    struct sigaction old;
    sa.sa_handler = (__sighandler_t)handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = (sig == 0xe) ? 0x20000000 : 0x10000000;
    int r = sigaction(sig, &sa, &old);
    if (-1 >= r)
    {
        printf("%d\xb9\xf8 signal \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\n", sig);
    }
    return -1 < r;
}

int CSignalTranslator::getSignal(int sig) const
{
    return (int)m_handlers[sig];
}

void signal_handler(int sig)
{
    ((CSignal*)CSignalTranslatorInstance()->getSignal(sig))->handle(sig);
}
