// df_dbmw_r - DNFSignal (ORIG DNFSignal.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();
CPacketTracer* CPacketTracerInstance();
CSignalTranslator* CSignalTranslatorInstance();

// ============================================================
// CSignal 家族
// ============================================================
CSignal::CSignal()
{
    m_app = 0;
}
CSignal::~CSignal() {}
void CSignal::dump_core_file()
{
    CPacketTracerInstance()->AbsoluteWriteLog();
    int ret;
    struct rlimit rl;
    ret = getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = -1;
    ret = setrlimit(RLIMIT_CORE, &rl);
    abort();
}
void CSignal::attachApp(CApplication* app) { m_app = app; }


void signal_handler(int sig)
{
    CSignalTranslatorInstance()->getSignal(sig)->handle(sig);
}
