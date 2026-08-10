// df_coserver_r — CSignalTranslator + signal_handler（ORIG DNFSignalTranslator.cpp）
#include <signal.h>
#include <stdio.h>

#include "DNFSignalTranslator.h"
#include "DNFTableBase.h"
#include "DNFUser1Sig.h"

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
    // NOTE: ORIG 无 return 语句直接落底（返回残留 eax），调用方均忽略返回值；照抄 ORIG 以对齐机器码
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

CSignal* CSignalTranslator::getSignal(int sig) const
{
    return m_handlers[sig];
}

void signal_handler(int sig)
{
    CSignalTranslatorInstance()->getSignal(sig)->handle(sig);
}

CSignalTranslator* CSignalTranslatorInstance()
{
    static CSignalTranslator instance;
    return &instance;
}
