// df_monitor_r — DNFSignalTranslator（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFSignalTranslator.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "DNFApplication.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFSignal.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTerminateSig.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"

CSignalTranslator* CSignalTranslatorInstance()
{
    static CSignalTranslator instance;
    return &instance;
}

CSignalTranslator::CSignalTranslator() {}

CSignalTranslator::~CSignalTranslator() {}

void CSignalTranslator::init(CApplication* app)
{
    try
    {
        init_signal();
        init_handler(app);
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
        throw CDNFException("regist_signal():SIGTERM");
    if (!regist_signal(10, signal_handler))
        throw CDNFException("regist_signal():SIGUSR1");
    if (!regist_signal(0xc, signal_handler))
        throw CDNFException("regist_signal():SIGUSR2");
    if (!regist_signal(2, (void (*)(int))1))
        throw CDNFException("regist_signal():SIGINT");
    if (!regist_signal(0xb, signal_handler))
        throw CDNFException("regist_signal():SIGSEGV");
    if (!regist_signal(8, signal_handler))
        throw CDNFException("regist_signal():SIGFPE");
    if (!regist_signal(0xd, (void (*)(int))1))
        throw CDNFException("regist_signal():SIGPIPE");
    if (!regist_signal(4, signal_handler))
        throw CDNFException("regist_signal():SIGILL");
    if (!regist_signal(7, signal_handler))
        throw CDNFException("regist_signal():SIGBUS");
    if (!regist_signal(0x10, signal_handler))
        throw CDNFException("regist_signal():SIGSTKFLT");
    if (!regist_signal(0x17, signal_handler))
        throw CDNFException("regist_signal():SIGURG");
    if (!regist_signal(0x18, signal_handler))
        throw CDNFException("regist_signal():SIGXCPU");
    if (!regist_signal(0x19, signal_handler))
        throw CDNFException("regist_signal():SIGXFSZ");
    if (!regist_signal(0x1f, signal_handler))
        throw CDNFException("regist_signal():SIGSYS");
}

void CSignalTranslator::init_handler(CApplication* app)
{
    m_data[0xf] = new CTerminateSig;
    m_data[0xf]->attachApp(app);
    for (int i = 0; i <= 0x19; i++)
    {
        m_data[i] = m_data[0xf];
    }
    m_data[0x6] = new CSegmentationFaultSig;
    m_data[0x6]->attachApp(app);
    m_data[0xb] = m_data[0x6];
    m_data[0x8] = m_data[0x6];
    m_data[0x2] = m_data[0x6];
    m_data[0xa] = new CUser1Sig;
    m_data[0xa]->attachApp(app);
    m_data[0xc] = new CUser2Sig;
    m_data[0xc]->attachApp(app);
    m_data[0x4] = new CSystemFailSig;
    m_data[0x4]->attachApp(app);
    m_data[0x7] = m_data[0x4];
    m_data[0x17] = m_data[0x4];
    m_data[0x10] = m_data[0x4];
    m_data[0x18] = m_data[0x4];
    m_data[0x19] = m_data[0x4];
    m_data[0x1f] = m_data[0x4];
}

bool CSignalTranslator::regist_signal(int sig, void (*handler)(int))
{
    struct sigaction sa;
    struct sigaction old;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sig == 0xe)
    {
        sa.sa_flags |= 0x20000000;
    }
    else
    {
        sa.sa_flags |= 0x10000000;
    }
    if (sigaction(sig, &sa, &old) < 0)
    {
        printf("%d\xb9\xf8 signal \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\n", sig);
        return 0;
    }
    return 1;
}

CSignal* CSignalTranslator::getSignal(int sig) const
{
    return m_data[sig];
}

void CSignalTranslator::clear()
{
    if (m_data[0x4] != 0)
    {
        delete m_data[0x4];
        m_data[0x4] = 0;
    }
    if (m_data[0xa] != 0)
    {
        delete m_data[0xa];
        m_data[0xa] = 0;
    }
    if (m_data[0xc] != 0)
    {
        delete m_data[0xc];
        m_data[0xc] = 0;
    }
    if (m_data[0x6] != 0)
    {
        delete m_data[0x6];
        m_data[0x6] = 0;
    }
    if (m_data[0xf] != 0)
    {
        delete m_data[0xf];
        m_data[0xf] = 0;
    }
}

void signal_handler(int sig)
{
    CSignalTranslatorInstance()->getSignal(sig)->handle(sig);
}
