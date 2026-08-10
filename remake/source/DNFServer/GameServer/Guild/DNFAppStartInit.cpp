// df_guild_r — DNFAppStartInit (split from source/guild per ORIG layout)
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "DNFAppStartInit.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStopInit.h"
#include "DNFApplication.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

void CAppInit::Init(CApplication* app, int argc, char** argv)
{
}

void CAppInit::Load(CApplication* app, int argc, char** argv)
{
}

void CAppInit::Free(CApplication* app)
{
}

CAppStartInit::CAppStartInit()
{
}

CAppStartInit::~CAppStartInit()
{
}

void CAppStartInit::Init(CApplication* app, int argc, char** argv)
{
    srand((unsigned int)time(0));
    CAppConfig* config = new CAppConfig;
    app->m_appConfig = config;
    std::string fn(argv[1]);
    config->Check_FileName(fn);
    CServerConfig* serverConfig = new CServerConfig;
    app->m_serverConfig = serverConfig;
    CKillUSRConfig* killConfig = new CKillUSRConfig;
    app->m_killConfig = killConfig;
    if (Init_Daemon(argc, argv) == -1)
    {
        throw CDNFException("CAppStartInit::Init() Demon Init Exception Break!");
    }
}

int CAppStartInit::Init_Daemon(int argc, char** argv)
{
    if (argv[2] != 0 && strcmp(argv[2], "start") == 0)
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
    std::string fn(argv[1]);
    return Save_pid(fn) ? 0 : -1;
}

bool CAppStartInit::Save_pid(const std::string& name)
{
    std::string path = "./pid/" + name;
    int fd = ::open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0)
    {
        return false;
    }
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%ld\n", (long)getpid());
    int len = (int)strlen(buf);
    ssize_t w = ::write(fd, buf, (size_t)len);
    ::close(fd);
    return w > 0;
}

CAppInit::~CAppInit()
{
}

CAppInit::CAppInit()
{
}

