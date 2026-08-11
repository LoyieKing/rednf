// df_statics_r — DNFSignal（ORIG DNFSignal.o 拆分）
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

#include "DNFSignal.h"
#include "DNFPacketTracer.h"
#include "DNFApplication.h"

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
    volatile int r;
    struct rlimit rl;
    r = getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = 0xffffffff;
    r = setrlimit(RLIMIT_CORE, &rl);
    abort();
}
CSignal::CSignal()
{
    m_app = 0;
}
