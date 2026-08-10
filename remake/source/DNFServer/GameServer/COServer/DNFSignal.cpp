// df_coserver_r — CSignal 基类 + Terminate/Segv/Sysfail/Fpe（ORIG DNFSignal.cpp）
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#include "DNFSignal.h"
#include "DNFApplication.h"
#include "DNFPacketTracer.h"

CSignal::CSignal()
{
    m_app = 0;
}

CSignal::~CSignal()
{
}

void CSignal::attachApp(CApplication* app)
{
    m_app = app;
}

void CSignal::dump_core_file()
{
    CPacketTracerInstance()->AbsoluteWriteLog();
    volatile int r;  // ORIG keeps getrlimit/setrlimit 的返回值存储（死存储）；GCC4.4.7 会消除普通局部，用 volatile 照抄 ORIG 机器码（§16/§36/§74）
    struct rlimit rl;
    r = getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = 0xffffffff;
    r = setrlimit(RLIMIT_CORE, &rl);
    abort();
}

CTerminateSig::CTerminateSig() throw()
{
}

CTerminateSig::~CTerminateSig() throw()
{
}

void CTerminateSig::handle(int sig)
{
    puts("Recv SIGTERM signal");
    if (m_app != 0)
    {
        m_app->App_Stop();
    }
}

CSegmentationFaultSig::CSegmentationFaultSig() throw()
{
}

CSegmentationFaultSig::~CSegmentationFaultSig() throw()
{
}

void CSegmentationFaultSig::handle(int sig)
{
    puts("Recv SIGSEGV signal --> make Dump Core file.");
    if (m_app != 0)
    {
        m_app->App_Stop();
    }
    dump_core_file();
}

CSystemFailSig::CSystemFailSig() throw()
{
}

CSystemFailSig::~CSystemFailSig() throw()
{
}

void CSystemFailSig::handle(int sig)
{
    puts("Recv SIGSYS signal");
    puts("Recv SIGSYS signal --> make Dump Core file.");
    if (m_app != 0)
    {
        m_app->App_Stop();
    }
    dump_core_file();
    exit(-1);
}

CFloatingPointExceptSig::~CFloatingPointExceptSig() throw()
{
}

void CFloatingPointExceptSig::handle(int sig)
{
    puts("Recv SIGFPE signal --> make Dump Core file.");
    if (m_app != 0)
    {
        m_app->App_Stop();
    }
    dump_core_file();
}
