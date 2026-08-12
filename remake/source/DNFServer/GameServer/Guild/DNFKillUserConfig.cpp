// df_guild_r — DNFKillUserConfig (split from source/guild per ORIG layout)
#include <stdio.h>
#include <utility>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFKillUserConfig.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
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

CKillUSRConfig::~CKillUSRConfig()
{
    Clear_Table();
}

void CKillUSRConfig::Clear_Table()
{
    if (!m_infos.empty())
    {
        for (std::vector<ST_KillUSRConfig*>::iterator it = m_infos.begin(); it != m_infos.end();
             ++it)
        {
            ST_KillUSRConfig* p = *it;
            delete p;
            p = 0;
        }
        m_infos.clear();
    }
}

int CKillUSRConfig::Parse_Table(char* line, int idx)
{
    if (line[0] == '#')
    {
        return 0;
    }
    char* tok[4];
    ST_KillUSRConfig* p;
    int x;  // ORIG 帧布局对齐（第 22 轮回归）
    if (DNFFLib::ExplodeString(line, " \t\r\n\"", tok, 4) == 4)
    {
        p = new (std::nothrow) ST_KillUSRConfig;
        if (p != 0)
        {
            p->m_field0 = atoi(tok[0]);
            p->m_field1 = atoi(tok[1]);
            p->m_field2 = atoi(tok[2]);
            p->m_field3 = atoi(tok[3]);
            m_infos.push_back(p);
            return 1;
        }
    }
    return 0;
}

void CKillUSRConfig::Load_Table(const std::string& path)
{
    int rc = Load_Txt_Table_Data(path.c_str(), 100);
    if (0 < rc && rc < 0x65)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0x5b, "./log/TableError", "Kill USR Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CKillUSRConfig::Load_Setup_Table() Exception break!");
}

ST_KillUSRConfig::ST_KillUSRConfig()
{
    m_field0 = 0;
    m_field1 = 0;
    m_field2 = 0;
    m_field3 = 0;
}

const std::vector<ST_KillUSRConfig*>* CKillUSRConfig::GetInfo() const
{
    return &m_infos;
}

CKillUSRConfig::CKillUSRConfig()
{
}
