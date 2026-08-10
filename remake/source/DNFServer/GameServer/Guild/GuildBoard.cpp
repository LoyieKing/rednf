// df_guild_r — GuildBoard (split from source/guild per ORIG layout)
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

#include "GuildBoard.h"
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

STGuildBoardDBInfo::STGuildBoardDBInfo()
{
    new ((char*)this + 0x84) STGuildMemberCharacData;
    *(unsigned int*)((char*)this + 0x78) = 0;
    *(unsigned int*)((char*)this + 0x7c) = 0;
    *(unsigned int*)((char*)this + 0x80) = 0;
    memset((char*)this, 0, 0x78);
}

void CGuildBoard::setWebGuildBoardAction(bool flag)
{
    *(unsigned char*)(m_data + 4) = flag ? 1 : 0;
}

bool CGuildBoard::isWebGuildBoardAction()
{
    return *(unsigned char*)(m_data + 4) != 0;
}

void CGuildBoard::setGuildBoardDBLoadState(ENUM_DB_LOAD_STATE state)
{
    *(unsigned int*)(m_data + 8) = (unsigned int)state;
}

int CGuildBoard::getGuildBoardDBLoadState()
{
    return *(unsigned int*)(m_data + 8);
}

CGuildBoard::CGuildBoard()
{
    new (m_data + 0xc) std::map<unsigned int, STGuildBoardDBInfo,
                               std::greater<unsigned int> >();   // class +0xc
    reset();
}

CGuildBoard::~CGuildBoard()
{
    reset();
    ((std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >*)(m_data + 0xc))
        ->~map();
}

void CGuildBoard::reset()
{
    ((std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >*)(m_data + 0xc))
        ->clear();
    *(unsigned int*)(m_data + 0) = 0;    // field0
    *(unsigned char*)(m_data + 4) = 0;   // webAction
    *(unsigned int*)(m_data + 8) = 0;    // dbLoadState
    *(unsigned int*)(m_data + 0x24) = 0; // dbAccessTime
}

bool CGuildBoard::isGuildBoardDBAccess()
{
    return 5 < (unsigned int)(time(0) - *(unsigned int*)(m_data + 0x24));
}

void CGuildBoard::setGuildBoardDBAccess()
{
    *(unsigned int*)(m_data + 0x24) = (unsigned int)time(0);
}

void CGuildBoard::clearGuildBoardData()
{
    ((std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >*)(m_data + 0xc))
        ->clear();
}

void CGuildBoard::setGuildBoardData(unsigned int a, unsigned int b, CGuild* guild, int c,
                                    STGuildBoardDBInfo* info)
{
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >* map =
        (std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >*)(m_data + 0xc);
    for (int i = 0; i < c; i++)
    {
        STGuildBoardDBInfo entry;
        char* src = (char*)info + i * 0xa5;
        memcpy(&entry, src, 0x78);
        *(unsigned int*)((char*)&entry + 0x78) = *(unsigned int*)(src + 0x78);
        unsigned int key = *(unsigned int*)(src + 0x7c);
        *(unsigned int*)((char*)&entry + 0x7c) = key;
        *(unsigned int*)((char*)&entry + 0x80) = *(unsigned int*)(src + 0x80);
        memcpy((char*)&entry + 0x84, src + 0x84, 0x21);
        if (guild->IsGuildMaster(*(unsigned int*)(src + 0x80)) != 0)
        {
            *(unsigned char*)((char*)&entry + 0x86) = 1;
        }
        map->insert(std::make_pair(key, entry));
    }
    DNF_LOG_SCOPE_AT("setGuildBoardData", 0x5f, "./log/GuildBoard", "SET SUCCESS - GUILD:%u, CHARAC:%u, COUNT:%u", a, b, c);
}

void CGuildBoard::sendGuildBoardData(unsigned int a, unsigned int b, unsigned int c,
                                     CUser* user)
{
    if (user == 0)
    {
        return;
    }
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >* map =
        (std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >*)(m_data + 0xc);
    int total = (int)map->size();
    unsigned short codeType = (unsigned short)c;
    if (total == 0)
    {
        Packet_Guild_Reply_Guild_Board reply;
        *(unsigned short*)((char*)&reply + 0xc) = codeType;
        *(unsigned int*)((char*)&reply + 0xf) = user->GetIdByChannel();
        *(unsigned int*)((char*)&reply + 0x13) = user->GetUniqCharNo();
        *(unsigned char*)((char*)&reply + 0xe) = 0;
        *(unsigned char*)((char*)&reply + 0x17) = 0;
        user->SendTcpGameserver(&reply);
        DNF_LOG_SCOPE_AT("sendGuildBoardData", 0x77,"./log/GuildBoard", "SEND SUCCESS - CODE TYPE:%u, GUILD:%u, CHARAC:%u, COUNT:%u",
            c, a, b, 0);
        return;
    }
    if (0x32 < total)
    {
        total = 0x32;
    }
    int fullPages = total / 10;
    int remainder = total % 10;
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >::iterator it =
        map->begin();
    for (int page = 0; page < fullPages; page++)
    {
        Packet_Guild_Reply_Guild_Board reply;
        *(unsigned short*)((char*)&reply + 0xc) = codeType;
        *(unsigned int*)((char*)&reply + 0xf) = user->GetIdByChannel();
        *(unsigned int*)((char*)&reply + 0x13) = user->GetUniqCharNo();
        *(unsigned char*)((char*)&reply + 0xe) = (unsigned char)total;
        *(unsigned char*)((char*)&reply + 0x17) = 10;
        for (int i = 0; i < 10; i++)
        {
            char* out = (char*)&reply + 0x18 + i * 0xa5;
            char* stored = (char*)&it->second;
            memcpy(out, stored + 4, 0x78);
            *(unsigned int*)(out + 0x78) = *(unsigned int*)(stored + 0x7c);
            *(unsigned int*)(out + 0x7c) = it->first;
            *(unsigned int*)(out + 0x80) = *(unsigned int*)(stored + 0x84);
            memcpy(out + 0x84, stored + 0x88, 0x21);
            ++it;
        }
        user->SendTcpGameserver(&reply);
        DNF_LOG_SCOPE_AT("sendGuildBoardData", 0xa5,"./log/GuildBoard", "SEND SUCCESS - CODE TYPE:%u, GUILD:%u, CHARAC:%u, COUNT:%u",
            c, a, b, total, 10);
    }
    if (remainder != 0)
    {
        Packet_Guild_Reply_Guild_Board reply;
        *(unsigned short*)((char*)&reply + 0xc) = codeType;
        *(unsigned int*)((char*)&reply + 0xf) = user->GetIdByChannel();
        *(unsigned int*)((char*)&reply + 0x13) = user->GetUniqCharNo();
        *(unsigned char*)((char*)&reply + 0xe) = (unsigned char)total;
        *(unsigned char*)((char*)&reply + 0x17) = (unsigned char)remainder;
        for (int i = 0; i < remainder; i++)
        {
            char* out = (char*)&reply + 0x18 + i * 0xa5;
            char* stored = (char*)&it->second;
            memcpy(out, stored + 4, 0x78);
            *(unsigned int*)(out + 0x78) = *(unsigned int*)(stored + 0x7c);
            *(unsigned int*)(out + 0x7c) = it->first;
            *(unsigned int*)(out + 0x80) = *(unsigned int*)(stored + 0x84);
            memcpy(out + 0x84, stored + 0x88, 0x21);
            ++it;
        }
        user->SendTcpGameserver(&reply);
        DNF_LOG_SCOPE_AT("sendGuildBoardData", 0xcb,"./log/GuildBoard", "SEND SUCCESS - CODE TYPE:%u, GUILD:%u, CHARAC:%u, COUNT:%u",
            c, a, b, total, remainder);
    }
}

void CGuildBoard::deleteGuildBoardData(unsigned int a, unsigned int b, unsigned int c)
{
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >* map =
        (std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >*)(m_data + 0xc);
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >::iterator it =
        map->find(a);
    if (it == map->end())
    {
        DNF_LOG_SCOPE_LINE(0xe3, "./log/GuildBoard", "DELETE FAIL - GUILD:%u, CHARAC:%u, NO:%u", b, c, a);
    }
    else
    {
        map->erase(it);
        DNF_LOG_SCOPE_LINE(0xdc, "./log/GuildBoard", "DELETE SUCCESS - GUILD:%u, CHARAC:%u, NO:%u", b, c, a);
    }
}

void CGuildBoard::sendMessageToDBMW_GuildLevelUP(CServerHandler* handler, int level,
                                                 CUser* user)
{
    char buf[255];
    std::string msg;
    Packet_DB_Load_Request_Guild_Board_Write pkt;
    *(unsigned int*)((char*)&pkt + 0xb) = user->GetGuildKey();
    *(unsigned int*)((char*)&pkt + 0x13) = user->GetUniqCharNo();
    msg = g_ServerString_.GetServerString(0x3ee, 0);
    std::string str = g_ServerString_.GetServerString(0x3ef, 0);
    sprintf(buf, "%d", level + 1);
    msg += buf;
    msg += str;
    if (msg.length() < 0x78)
    {
        memcpy((char*)&pkt + 0x17, msg.c_str(), msg.length());
        handler->SendToDB(&pkt);
        DNF_LOG_SCOPE_AT("sendMessageToDBMW_GuildLevelUP", 0x107,"./log/GuildBoard", "SET SUCCESS - GUILD:%u, CHARAC:%u, LEVEL:%u",
            *(unsigned int*)((char*)&pkt + 0xb), *(unsigned int*)((char*)&pkt + 0x13),
            level + 1);
    }
}

void CGuildBoard::sendMessageToDBMW_GuildAttendance(CServerHandler* handler, int a, int b,
                                                    unsigned int c, unsigned int d)
{
}

void CGuildBoard::sendMessageToDBMW_GuildFund(CServerHandler* handler, int fund, CUser* user)
{
    char buf[255];
    char buf2[255];
    std::string msg;
    Packet_DB_Load_Request_Guild_Board_Write pkt;
    *(unsigned int*)((char*)&pkt + 0xb) = user->GetGuildKey();
    *(unsigned int*)((char*)&pkt + 0x13) = user->GetUniqCharNo();
    std::string str1 = g_ServerString_.GetServerString(0x3f0, 0);
    std::string str2 = g_ServerString_.GetServerString(0x3e8, 0);
    sprintf(buf, "%s %s ", user->GetCharName(), str2.c_str());
    msg += buf;
    sprintf(buf2, "%d ", fund);
    msg += buf2;
    msg += str1;
    if (msg.length() < 0x78)
    {
        memcpy((char*)&pkt + 0x17, msg.c_str(), msg.length());
        handler->SendToDB(&pkt);
        DNF_LOG_SCOPE_LINE(0x15d,"./log/GuildBoard", "SET SUCCESS - GUILD:%u, CHARAC:%u, gold :%d",
            *(unsigned int*)((char*)&pkt + 0xb), *(unsigned int*)((char*)&pkt + 0x13), fund);
    }
}

void CGuildBoard::sendMessageToDBMW_GuildMasterChanging(CServerHandler* handler, CUser* user,
                                                        const char* name)
{
    Packet_DB_Load_Request_Guild_Board_Write pkt;
    *(unsigned int*)((char*)&pkt + 0xb) = user->GetGuildKey();
    *(unsigned int*)((char*)&pkt + 0x13) = user->GetUniqCharNo();
    std::string msg = name;
    std::string str = g_ServerString_.GetServerString(0x3f1, 0);
    msg += str;
    if (msg.length() < 0x78)
    {
        memcpy((char*)&pkt + 0x17, msg.c_str(), msg.length());
        handler->SendToDB(&pkt);
        DNF_LOG_SCOPE_AT("sendMessageToDBMW_GuildMasterChanging", 0x17b,"./log/GuildBoard", "SET SUCCESS - GUILD:%u, CHARAC:%u",
            *(unsigned int*)((char*)&pkt + 0xb), *(unsigned int*)((char*)&pkt + 0x13));
    }
}

void CGuildBoard::printGuildBoard()
{
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >* map =
        (std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >*)(m_data + 0xc);
    for (std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >::iterator it =
             map->begin();
         it != map->end(); ++it)
    {
        DNF_LOG_SCOPE_LINE(0x188,"./log/GuildBoard", "\n*%d* %d %s\n",
            it->first, *(unsigned int*)((char*)&it->second + 0x7c),
            (char*)&it->second + 4);
    }
}

