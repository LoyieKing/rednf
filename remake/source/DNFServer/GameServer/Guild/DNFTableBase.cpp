// df_guild_r — DNFTableBase (split from source/guild per ORIG layout)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "PacketHeader.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

#include "DNFTableBase.h"
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

CDNFException::CDNFException(const std::string& msg)
    : m_msg(msg)
{
}

const char* CDNFException::what() const throw()
{
    DNF_LOG_SCOPE_LINE(0x1a, "./log/Except", "%s", m_msg.c_str());
    return m_msg.c_str();
}

CDNFException::~CDNFException() throw()
{
}

CTableBase::~CTableBase()
{
}

int CTableBase::Load_Txt_Table_Data(const char* path, int maxCount)
{
    FILE* f = fopen(path, "rb");
    int count = 0;
    if (f == 0)
    {
        count = -1;
    }
    else
    {
        char line[1024];
        while (true)
        {
            if (!feof(f) && fgets(line, 0x400, f) != 0)
            {
                if (line[0] != '#')
                {
                    if (maxCount <= count)
                    {
                        return -2;
                    }
                    if (Parse_Table(line, count) != 0)
                    {
                        count++;
                    }
                }
            }
            else
            {
                break;
            }
        }
        fclose(f);
    }
    return count;
}

CTableBase::CTableBase()
{
}

