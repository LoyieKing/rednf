// df_manager_r — CSignalTranslator / signal_handler（ORIG DNFSignalTranslator.cpp）
#include "DNFSignalTranslator.h"

#include <signal.h>
#include <stdio.h>

#include "DNFTableBase.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFSystemFailSig.h"
#include "DNFTerminateSig.h"
#include "DNFUser1Sig.h"

CSignalTranslator::CSignalTranslator() {}
CSignalTranslator::~CSignalTranslator() {}

CSignalTranslator* CSignalTranslatorInstance()
{
    static CSignalTranslator instance;
    return &instance;
}

bool CSignalTranslator::regist_signal(int sig, void (*handler)(int))
{
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    // R10: ORIG 直接对 act.sa_flags 赋值/位或（无独立 flags 局部），
    // 消除多余栈槽并复现 ORIG 的 eax 往返形态。
    act.sa_flags = 0;
    if (sig == 0xe)
        act.sa_flags |= 0x20000000;
    else
        act.sa_flags |= 0x10000000;
    struct sigaction old;
    if (sigaction(sig, &act, &old) < 0)
    {
        // ORIG 实测（0x8151c86）：EUC-KR 字节 b9f8/b5eeb7cf/bdc7c6d0
        // = "%d번 signal 등록 실패\n"（此前误用 UTF-8 字节导致 mojibake）。
        printf("%d\xB9\xF8 signal \xB5\xEE\xB7\xCF \xBD\xC7\xC6\xD0\n", sig);
        return false;
    }
    return true;
}

void CSignalTranslator::init_signal()
{
    if (!regist_signal(0xf, signal_handler))
        throw CDNFException("regist_signal():SIGTERM");
    if (!regist_signal(0xa, signal_handler))
        throw CDNFException("regist_signal():SIGUSR1");
    if (!regist_signal(0xc, signal_handler))
        throw CDNFException("regist_signal():SIGUSR2");
    if (!regist_signal(0x2, (void (*)(int))1))
        throw CDNFException("regist_signal():SIGINT");
    if (!regist_signal(0xb, signal_handler))
        throw CDNFException("regist_signal():SIGSEGV");
    if (!regist_signal(0x8, signal_handler))
        throw CDNFException("regist_signal():SIGFPE");
    if (!regist_signal(0xd, (void (*)(int))1))
        throw CDNFException("regist_signal():SIGPIPE");
    if (!regist_signal(0x4, signal_handler))
        throw CDNFException("regist_signal():SIGILL");
    if (!regist_signal(0x7, signal_handler))
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
    m_signals[15] = new CTerminateSig;
    m_signals[15]->attachApp(app);
    for (int i = 0; i <= 0x19; i++)
        m_signals[i] = m_signals[15];
    m_signals[6] = new CSegmentationFaultSig;
    m_signals[6]->attachApp(app);
    m_signals[11] = m_signals[6];
    m_signals[8] = m_signals[6];
    m_signals[2] = m_signals[6];
    m_signals[10] = new CUser1Sig;
    m_signals[10]->attachApp(app);
    m_signals[12] = new CUser2Sig;
    m_signals[12]->attachApp(app);
    m_signals[4] = new CSystemFailSig;
    m_signals[4]->attachApp(app);
    m_signals[7] = m_signals[4];
    m_signals[23] = m_signals[4];
    m_signals[16] = m_signals[4];
    m_signals[24] = m_signals[4];
    m_signals[25] = m_signals[4];
    m_signals[31] = m_signals[4];
}

void CSignalTranslator::clear()
{
    if (m_signals[4])
    {
        delete m_signals[4];
        m_signals[4] = 0;
    }
    if (m_signals[10])
    {
        delete m_signals[10];
        m_signals[10] = 0;
    }
    if (m_signals[12])
    {
        delete m_signals[12];
        m_signals[12] = 0;
    }
    if (m_signals[6])
    {
        delete m_signals[6];
        m_signals[6] = 0;
    }
    if (m_signals[15])
    {
        delete m_signals[15];
        m_signals[15] = 0;
    }
}

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

void signal_handler(int sig)
{
    CSignalTranslatorInstance()->getSignal(sig)->handle(sig);
}

CSignal* CSignalTranslator::getSignal(int idx) const { return m_signals[idx]; }
