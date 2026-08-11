// df_dbmw_r - DNFAppStartInit (ORIG DNFAppStartInit.cpp)
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
CAppStartInit::CAppStartInit() {}
CAppStartInit::~CAppStartInit() {}
bool CAppStartInit::Save_pid(const std::string& path)
{
    std::string full = std::string("./pid/") + path + std::string(".pid");
    int fd = open(full.c_str(), 0x42, 0x1a4);
    if (fd < 0)
        return 0;
    char buf[0x400];
    memset(buf, 0, 0x400);
    sprintf(buf, "%ld\n", (long)getpid());
    int n = write(fd, buf, strlen(buf));
    if (n < 0)
    {
        close(fd);
        return 0;
    }
    return 1;
}
int CAppStartInit::Init_Daemon(int argc, char** argv)
{
    const char* arg = argv[2];
    if (strcmp(arg, "start") == 0)
    {
        int pid = fork();
        if (pid < 0)
            return -1;
        if (pid != 0)
            exit(0);
        setsid();
        chdir("./");
        umask(0);
    }
    if (!Save_pid(std::string(argv[1])))
        return -1;
    return 0;
}
void CAppStartInit::Init(CApplication* app, int argc, char** argv)
{
    srand(time(0));
    app->m_appConfig = new CAppConfig;
    ((CAppConfig*)app->m_appConfig)->Check_FileName(argv[1]);
    app->m_serverConfig = new CServerConfig;
    app->m_killUsrConfig = new CKillUSRConfig;
    if (Init_Daemon(argc, argv) == -1)
    {
        throw CDNFException("CAppStartInit::Init() Demon Init Exception Break!");
    }
}
