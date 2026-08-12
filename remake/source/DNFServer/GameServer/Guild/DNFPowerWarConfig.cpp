// df_guild_r — DNFPowerWarConfig (split from source/guild per ORIG layout)
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

#include "DNFPowerWarConfig.h"
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

CPowerWarConfig::~CPowerWarConfig()
{
    Clear_Table();
}

void CPowerWarConfig::Clear_Table()
{
    m_info.m_day = 0xff;
    m_info.m_hour = 0xff;
    m_info.m_min = 0xff;
    m_info.m_schedule.clear();
}

int CPowerWarConfig::Parse_Table(char* line, int idx)
{
    if (*line == '#')
    {
        return 0;
    }
    int x;  // ORIG 帧布局对齐（第 22 轮回归）
    char* tokens[4];
    int n = DNFFLib::ExplodeString(line, " \t\r\n\"", tokens, 4);
    if (n == 4 || n == 2)
    {
        bool bFlag = 0;
        STPowerWarScheduleTime st;
        switch (idx)
        {
        case 0:
            m_info.m_day = (unsigned char)atoi(tokens[1]);
            break;
        case 1:
            m_info.m_hour = (unsigned char)atoi(tokens[1]);
            break;
        case 2:
            m_info.m_min = (unsigned char)atoi(tokens[1]);
            break;
        case 3:
            m_info.m_field4 = atoi(tokens[1]);
            break;
        case 4:
            bFlag = atoi(tokens[1]) != 0;
            if (bFlag)
            {
                st.m_data[0] = 0;
                st.m_data[1] = (char)atoi(tokens[2]);
                st.m_data[2] = (char)atoi(tokens[3]);
                m_info.m_schedule.push_back(st);
            }
            break;
        case 5:
            bFlag = atoi(tokens[1]) != 0;
            if (bFlag)
            {
                st.m_data[0] = 1;
                st.m_data[1] = (char)atoi(tokens[2]);
                st.m_data[2] = (char)atoi(tokens[3]);
                m_info.m_schedule.push_back(st);
            }
            break;
        case 6:
            bFlag = atoi(tokens[1]) != 0;
            if (bFlag)
            {
                st.m_data[0] = 2;
                st.m_data[1] = (char)atoi(tokens[2]);
                st.m_data[2] = (char)atoi(tokens[3]);
                m_info.m_schedule.push_back(st);
            }
            break;
        case 7:
            bFlag = atoi(tokens[1]) != 0;
            if (bFlag)
            {
                st.m_data[0] = 3;
                st.m_data[1] = (char)atoi(tokens[2]);
                st.m_data[2] = (char)atoi(tokens[3]);
                m_info.m_schedule.push_back(st);
            }
            break;
        case 8:
            bFlag = atoi(tokens[1]) != 0;
            if (bFlag)
            {
                st.m_data[0] = 4;
                st.m_data[1] = (char)atoi(tokens[2]);
                st.m_data[2] = (char)atoi(tokens[3]);
                m_info.m_schedule.push_back(st);
            }
            break;
        case 9:
            bFlag = atoi(tokens[1]) != 0;
            if (bFlag)
            {
                st.m_data[0] = 5;
                st.m_data[1] = (char)atoi(tokens[2]);
                st.m_data[2] = (char)atoi(tokens[3]);
                m_info.m_schedule.push_back(st);
            }
            break;
        case 10:
            bFlag = atoi(tokens[1]) != 0;
            if (bFlag)
            {
                st.m_data[0] = 6;
                st.m_data[1] = (char)atoi(tokens[2]);
                st.m_data[2] = (char)atoi(tokens[3]);
                m_info.m_schedule.push_back(st);
            }
            break;
        default:
            return 0;
        }
        return 1;
    }
    return 0;
}

void CPowerWarConfig::Load_Table(const std::string& path)
{
    int rc = Load_Txt_Table_Data(path.c_str(), 100);
    if (0 < rc && rc < 0x65)
    {
        return;
    }
    DNF_LOG_SCOPE_LINE(0xcc, "./log/TableError", "Power War Config Table - ReturnCode = %d\n", rc);
    throw CDNFException("CPowerWarConfig::Load_Setup_Table() Exception break!");
}

CPowerWarConfig::CPowerWarConfig()
{
}

ST_PowerWarEventStartTimeConfig* CPowerWarConfig::GetInfo() const
{
    return (ST_PowerWarEventStartTimeConfig*)((char*)this + 4);
}
