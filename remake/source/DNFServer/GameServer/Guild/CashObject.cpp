// df_guild_r — CashObject (split from source/guild per ORIG layout)
#include <stdio.h>
#include <utility>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "CashObject.h"
#include "BlackUser.h"
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

MemPool<CCashObject> m_CashObjectMemPool_(5000);

void* CCashObject::operator new(unsigned int size) { return m_CashObjectMemPool_.alloc(); }

void CCashObject::operator delete(void* p) { m_CashObjectMemPool_.free(p); }

void CCashObject::operator delete(void* p, unsigned int size) { m_CashObjectMemPool_.free(p, size); }

CCashObject::CCashObject()
    : m_lifeTime(5), m_charNo(0)
{
}

CCashObject::~CCashObject()
{
    m_lifeTime = 0;
    m_charNo = 0;
    m_blackUsers.clear();
}

bool CCashObject::IsLifeTimeOut()
{
    m_lifeTime -= 1;
    return m_lifeTime == 0;
}

void CCashObject::SetBlackUsersObject(std::map<unsigned int, CBlackUser*>& blackUsers)
{
    m_blackUsers = blackUsers;
}

void CCashObject::ClearBlackUsers()
{
    for (std::map<unsigned int, CBlackUser*>::iterator it = m_blackUsers.begin();
         it != m_blackUsers.end(); ++it)
    {
        if (it->second)
        {
            delete it->second;
        }
    }
    m_blackUsers.clear();
}

std::map<unsigned int, CBlackUser*>* CCashObject::GetBlackUsersObject()
{
    return &m_blackUsers;
}

void CCashObject::SetCharacNo(unsigned int charNo)
{
    m_charNo = charNo;
}
