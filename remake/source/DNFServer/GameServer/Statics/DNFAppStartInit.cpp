// df_statics_r — DNFAppStartInit（ORIG DNFAppStartInit.o 拆分）
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "DNFAppStartInit.h"
#include "DNFApplication.h"
#include "DNFAppConfig.h"
#include "DNFServerConfig.h"
#include "DNFTableBase.h"

void CommonTime::SetCurTime()
{
    time_t t;
    time(&t);
    tm* pt = localtime(&t);
    m_time[0] = (char)((char)pt->tm_year - 100);
    m_time[1] = (char)((char)pt->tm_mon + 1);
    m_time[2] = (char)pt->tm_mday;
    m_time[3] = (char)pt->tm_hour;
    m_time[4] = (char)pt->tm_min;
    m_time[5] = (char)pt->tm_wday;
}
CAppStartInit::CAppStartInit()
{
}
CAppStartInit::~CAppStartInit()
{
}
void CAppStartInit::Init(CApplication* app, int argc, char** argv)
{
    srand(time(0));
    app->m_appConfig = new CAppConfig;
    app->m_appConfig->Check_FileName(argv[1]);
    app->m_serverConfig = new CServerConfig;
    if (Init_Daemon(argc, argv) == -1)
    {
        throw CDNFException("CAppStartInit::Init() Demon Init Exception Break!");
    }
}
int CAppStartInit::Init_Daemon(int argc, char** argv)
{
    char* cmd = argv[2];
    if (strcmp(cmd, "start") == 0)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            return -1;
        }
        if (pid != 0)
        {
            exit(0);
        }
        setsid();
        chdir("./");
        umask(0);
    }
    if (Save_pid(argv[1]) == 1)
    {
        return 0;
    }
    return -1;
}
int CAppStartInit::Save_pid(const std::string& name)
{
    std::string path = "./pid/" + name + ".pid";
    int fd = open(path.c_str(), 0x42, 0x1a4);
    if (fd < 0)
    {
        return 0;
    }
    char buf[1024];
    memset(buf, 0, 0x400);
    sprintf(buf, "%ld\n", (long)getpid());
    ssize_t n = write(fd, buf, strlen(buf));
    if (n < 0)
    {
        close(fd);
        return 0;
    }
    return 1;
}
CAppInit::~CAppInit()
{
}
CAppInit::CAppInit()
{
}
