// df_manager_r — CSignal（ORIG DNFSignal.cpp）
#include "DNFSignal.h"

#include <stdlib.h>
#include <sys/resource.h>

#include "DNFPacketTracer.h"

CSignal::CSignal()
{
    m_app = 0;
}

CSignal::~CSignal() {}

void CSignal::dump_core_file()
{
    CPacketTracerInstance()->AbsoluteWriteLog();
    struct rlimit rl;
    getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = -1;
    setrlimit(RLIMIT_CORE, &rl);
    abort();
}

void CSignal::attachApp(CApplication* app) { m_app = app; }
