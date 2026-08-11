// df_dbmw_r - DNFUser1Sig (ORIG DNFUser1Sig.cpp)
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

// ============================================================
// CSignal 家族
// ============================================================
CUser1Sig::CUser1Sig() {}
CUser1Sig::~CUser1Sig() {}
void CUser1Sig::handle(int sig)
{
    CMyFileLog log(__FUNCTION__, 0x13);
    log("./log/Signal", "CUser1Sig::handle( int sig )");
    if (m_app)
        m_app->SendTestPacket_2();
}
CUser2Sig::CUser2Sig() {}
CUser2Sig::~CUser2Sig() {}
void CUser2Sig::handle(int sig)
{
    CMyFileLog log(__FUNCTION__, 0x20);
    log("./log/Signal", "CUser2Sig::handle( int sig )");
    if (m_app)
        m_app->TranslateSignal();
}
