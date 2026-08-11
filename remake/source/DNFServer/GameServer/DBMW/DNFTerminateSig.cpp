// df_dbmw_r - DNFTerminateSig (ORIG DNFTerminateSig.cpp)
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
CTerminateSig::CTerminateSig() {}
CTerminateSig::~CTerminateSig() {}
void CTerminateSig::handle(int sig)
{
    puts("Recv SIGTERM signal");
    if (m_app)
        m_app->App_Stop();
}
