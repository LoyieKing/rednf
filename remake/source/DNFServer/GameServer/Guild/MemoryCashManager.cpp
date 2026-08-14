// df_guild_r — MemoryCashManager (split from source/guild per ORIG layout)
#include <string.h>
#include <time.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include "tinyxml.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "MemoryCashManager.h"
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

CMemoryCashManager::CMemoryCashManager()
{
    m_app = 0;
}

CMemoryCashManager::~CMemoryCashManager()
{
}

void CMemoryCashManager::Init(CApplication* app)
{
    m_app = app;
}

bool CMemoryCashManager::IsRightObject(CUser* user)
{
    if ((user->GetBlackListDBFlag() & 4) != 0)
    {
        return true;
    }
    return false;
}

int CMemoryCashManager::InsertCashMemorySetCharacterObject(CUser* user)
{
    CCashObject* obj;
    if (IsRightObject(user))
    {
        obj = new CCashObject;
        obj->SetCharacNo(user->GetUniqCharNo());
        if (!m_cashObjects.insert(std::make_pair(user->GetDBID(), obj)).second)
        {
            delete obj;
            return 0;
        }
        return 1;
    }
    return 0;
}

bool CMemoryCashManager::SetUserObject(CUser* user)
{
    std::map<unsigned int, CCashObject*>::iterator it =
        m_cashObjects.find(user->GetDBID());
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        obj->SetBlackUsersObject(*(std::map<unsigned int, CBlackUser*>*)user->GetMapBlackList());
        return true;
    }
    return false;
}

void CMemoryCashManager::ProcessLifeTimeOut()
{
    if (m_app == 0)
    {
        return;
    }
    for (std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.begin();
         it != m_cashObjects.end(); )
    {
        CCashObject* obj = it->second;
        if (obj->IsLifeTimeOut())
        {
            obj->ClearBlackUsers();
            m_cashObjects.erase(it++);
            delete obj;
        }
        else
        {
            ++it;
        }
    }
}

int CMemoryCashManager::QueryCashMemoryBlackList(CUser* user)
{
    std::map<unsigned int, CCashObject*>::iterator it =
        m_cashObjects.find(user->GetDBID());
    if (it != m_cashObjects.end())
    {
        CCashObject* obj = it->second;
        user->RegisterToCashBlackList(*obj->GetBlackUsersObject());
        user->SetBlackListDBFlag(4);
        return true;
    }
    return false;
}

void CMemoryCashManager::DeleteCashObjecct(unsigned int dbid)
{
    std::map<unsigned int, CCashObject*>::iterator it = m_cashObjects.find(dbid);
    if (it != m_cashObjects.end())
    {
        delete it->second;
        m_cashObjects.erase(it);
    }
}
