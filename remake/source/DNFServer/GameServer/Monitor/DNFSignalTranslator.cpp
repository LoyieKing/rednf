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
        printf("CSignalTranslator::init() Exception : %s\n", e.what());
        throw;
    }
    catch (...)
    {
        puts("CSignalTranslator::init() Exception");
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
    void** table = (void**)this;
    CTerminateSig* term = new CTerminateSig;
    table[0xf] = term;
    term->attachApp(app);
    for (int i = 0; i < 0x1a; i++)
    {
        table[i] = term;
    }
    CSegmentationFaultSig* segv = new CSegmentationFaultSig;
    table[0x6] = segv;
    segv->attachApp(app);
    table[0xb] = segv;
    table[0x8] = segv;
    table[0x2] = segv;
    CUser1Sig* u1 = new CUser1Sig;
    table[0xa] = u1;
    u1->attachApp(app);
    CUser2Sig* u2 = new CUser2Sig;
    table[0xc] = u2;
    u2->attachApp(app);
    CSystemFailSig* fail = new CSystemFailSig;
    table[0x4] = fail;
    fail->attachApp(app);
    table[0x7] = fail;
    table[0x17] = fail;
    table[0x10] = fail;
    table[0x18] = fail;
    table[0x19] = fail;
    table[0x1f] = fail;
}

bool CSignalTranslator::regist_signal(int sig, void (*handler)(int))
{
    struct sigaction sa;
    struct sigaction old;
    sigset_t mask;
    sa.sa_handler = handler;
    sigemptyset(&mask);
    int flags = (sig == 0xe) ? 0x20000000 : 0x10000000;
    int r = sigaction(sig, &sa, &old);
    if (r < 0)
    {
        printf("regist signal error %d\n", sig);
    }
    return r >= 0;
}

void* CSignalTranslator::getSignal(int sig) const
{
    return *(void**)((char*)this + sig * 4);
}

void CSignalTranslator::clear()
{
    void** table = (void**)this;
    if (table[0x4] != 0)
    {
        (*(void(**)(void*))(*(void**)table[0x4] + 8))(table[0x4]);
        table[0x4] = 0;
    }
    if (table[0xa] != 0)
    {
        (*(void(**)(void*))(*(void**)table[0xa] + 8))(table[0xa]);
        table[0xa] = 0;
    }
    if (table[0xc] != 0)
    {
        (*(void(**)(void*))(*(void**)table[0xc] + 8))(table[0xc]);
        table[0xc] = 0;
    }
    if (table[0x6] != 0)
    {
        (*(void(**)(void*))(*(void**)table[0x6] + 8))(table[0x6]);
        table[0x6] = 0;
    }
    if (table[0xf] != 0)
    {
        (*(void(**)(void*))(*(void**)table[0xf] + 8))(table[0xf]);
        table[0xf] = 0;
    }
}

void signal_handler(int sig)
{
    CSignalTranslator* st = CSignalTranslatorInstance();
    void* sigObj = st->getSignal(sig);
    void (**fn)(void*, int) = *(void(***)(void*, int))sigObj;
    fn[0](sigObj, sig);
}
