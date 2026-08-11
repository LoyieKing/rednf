// df_coserver_r — CAppInit/CAppStartInit（ORIG DNFAppStartInit.cpp）
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFApplication.h"
#include "DNFKillUserConfig.h"
#include "DNFServerConfig.h"

// ORIG 实测（0x8051e8c/0x8051ea8）：CAppStartInit 构造函数栈帧为 0x18，
// 而本 TU 中其它函数（含 CAppInit 构造）在其之前定义时，4.4.6-3 只给
// 0x4。A/B 验证：把本构造函数放在 TU 首位（.text 第一函数）可复现 0x18
// （GCC 4.4 O0 对 TU 首函数的帧分配形态）。保持与 ORIG 一致的排放。
CAppStartInit::CAppStartInit()
{
}

CAppInit::CAppInit()
{
}

CAppInit::~CAppInit()
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
    app->m_killUsrConfig = new CKillUSRConfig;
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
    if (Save_pid(argv[1]) != 1)
    {
        return -1;
    }
    return 0;
}

bool CAppStartInit::Save_pid(const std::string& name)
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
