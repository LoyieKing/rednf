// df_dbmw_r - DNFAppStopInit (ORIG DNFAppStopInit.cpp)
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

// CTableBase / CAppInit / CAppConfig / CServerConfig
CAppStopInit::CAppStopInit() {}
CAppStopInit::~CAppStopInit() {}
void CAppStopInit::Init(CApplication* app, int argc, char** argv)
{
    puts("RECV STOP, Manager had stoped this program.");
    if (!app->Send_Term_Signal(std::string(argv[1])))
        throw CDNFException("By CAppStopInit::Init(), this app had stoped!");
}
