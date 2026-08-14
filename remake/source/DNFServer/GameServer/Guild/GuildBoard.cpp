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
    m_guildKey = 0;
    m_boardId = 0;
    m_writerCharNo = 0;
    memset(m_memo, 0, 0x78);
}

void CGuildBoard::setWebGuildBoardAction(bool flag)
{
    m_webAction = flag;
}

unsigned char CGuildBoard::isWebGuildBoardAction()
{
    return m_webAction;
}

void CGuildBoard::setGuildBoardDBLoadState(ENUM_DB_LOAD_STATE state)
{
    struct BoardLoadView { char pad[8]; int m_loadState; };
    ((BoardLoadView*)this)->m_loadState = (int)state;
}

int CGuildBoard::getGuildBoardDBLoadState()
{
    return m_loadState;
}

CGuildBoard::CGuildBoard()
{
    reset();
}

CGuildBoard::~CGuildBoard()
{
    reset();
}

void CGuildBoard::reset()
{
    m_board.clear();
    m_field0 = 0;
    m_webAction = 0;
    m_loadState = 0;
    m_dbAccessTime = 0;
}

bool CGuildBoard::isGuildBoardDBAccess()
{
    unsigned int t = (unsigned int)time(0);
    if (5 < (t - m_dbAccessTime))
    {
        return true;
    }
    return false;
}

void CGuildBoard::setGuildBoardDBAccess()
{
    unsigned int t = (unsigned int)time(0);
    m_dbAccessTime = t;
}

void CGuildBoard::clearGuildBoardData()
{
    m_board.clear();
}

void CGuildBoard::setGuildBoardData(unsigned int a, unsigned int b, CGuild* guild, int c,
                                    STGuildBoardDBInfo* info)
{
    for (int i = 0; i < c; i++)
    {
        STGuildBoardDBInfo entry;
        entry.m_boardId = info[i].m_boardId;
        entry.m_guildKey = info[i].m_guildKey;
        memcpy(&entry, &info[i], 0x78);
        entry.m_writerCharNo = info[i].m_writerCharNo;
        memcpy(&entry.m_char, &info[i].m_char, 0x21);
        if (guild->IsGuildMaster(entry.m_writerCharNo) != 0)
        {
            entry.m_char.m_masterFlag = 1;
        }
        m_board.insert(std::make_pair(entry.m_boardId, entry));
    }
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x5f, "./log/GuildBoard", "SET SUCCESS - GUILD:%u, CHARAC:%u, COUNT:%u", a, b, c);
}

void CGuildBoard::sendGuildBoardData(unsigned int a, unsigned int b, unsigned int c,
                                     CUser* user)
{
    if (user == 0)
    {
        return;
    }
    int total = (int)m_board.size();
    if (total == 0)
    {
        Packet_Guild_Reply_Guild_Board reply;
        reply.m_c = (unsigned short)c;
        reply.m_f = user->GetIdByChannel();
        reply.m_13 = user->GetUniqCharNo();
        reply.m_e = 0;
        reply.m_17 = 0;
        user->SendTcpGameserver(&reply);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0x77,"./log/GuildBoard", "SEND SUCCESS - CODE TYPE:%u, GUILD:%u, CHARAC:%u, COUNT:%u",
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
        m_board.begin();
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >::iterator endit =
        m_board.end();
    for (int page = 0; page < fullPages; page++)
    {
        Packet_Guild_Reply_Guild_Board reply;
        reply.m_c = (unsigned short)c;
        reply.m_f = user->GetIdByChannel();
        reply.m_13 = user->GetUniqCharNo();
        reply.m_e = (unsigned char)total;
        reply.m_17 = 10;
        for (int i = 0; i < 10; i++)
        {
            reply.m_boards[i].m_boardId = it->first;
            memcpy(&reply.m_boards[i], &it->second, 0x78);
            reply.m_boards[i].m_guildKey = it->second.m_guildKey;
            reply.m_boards[i].m_writerCharNo = it->second.m_writerCharNo;
            memcpy(&reply.m_boards[i].m_char, &it->second.m_char, 0x21);
            ++it;
        }
        user->SendTcpGameserver(&reply);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0xa5,"./log/GuildBoard", "SEND SUCCESS - CODE TYPE:%u, GUILD:%u, CHARAC:%u, COUNT:%u",
            c, a, b, total, 10);
    }
    if (remainder != 0)
    {
        Packet_Guild_Reply_Guild_Board reply;
        reply.m_c = (unsigned short)c;
        reply.m_f = user->GetIdByChannel();
        reply.m_13 = user->GetUniqCharNo();
        reply.m_e = (unsigned char)total;
        reply.m_17 = (unsigned char)remainder;
        for (int i = 0; i < remainder; i++)
        {
            reply.m_boards[i].m_boardId = it->first;
            memcpy(&reply.m_boards[i], &it->second, 0x78);
            reply.m_boards[i].m_guildKey = it->second.m_guildKey;
            reply.m_boards[i].m_writerCharNo = it->second.m_writerCharNo;
            memcpy(&reply.m_boards[i].m_char, &it->second.m_char, 0x21);
            ++it;
            if (it == endit)
            {
                break;
            }
        }
        user->SendTcpGameserver(&reply);
        DNF_LOG_SCOPE_AT(__FUNCTION__, 0xcb,"./log/GuildBoard", "SEND SUCCESS - CODE TYPE:%u, GUILD:%u, CHARAC:%u, COUNT:%u",
            c, a, b, total, remainder);
    }
}

void CGuildBoard::deleteGuildBoardData(unsigned int a, unsigned int b, unsigned int c)
{
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >::iterator it =
        m_board.find(a);
    if (it != m_board.end())
    {
        m_board.erase(a);
        DNF_LOG_SCOPE_LINE(0xdc, "./log/GuildBoard", "DELETE SUCCESS - GUILD:%u, CHARAC:%u, NO:%u", b, c, a);
    }
    else
    {
        DNF_LOG_SCOPE_LINE(0xe3, "./log/GuildBoard", "DELETE FAIL - GUILD:%u, CHARAC:%u, NO:%u", b, c, a);
    }
}

void CGuildBoard::sendMessageToDBMW_GuildLevelUP(CServerHandler* handler, int level,
                                                 CUser* user)
{
    Packet_DB_Load_Request_Guild_Board_Write pkt;
    pkt.m_b = user->GetGuildKey();
    pkt.m_d = user->GetUniqCharNo();
    std::string msg = g_ServerString_.GetServerString(0x3ee, 0);
    std::string str = g_ServerString_.GetServerString(0x3ef, 0);
    char buf[255] = {0};
    sprintf(buf, "%d", level + 1);
    msg += buf;
    msg += str;
    if (msg.length() > 0x77)
    {
        return;
    }
    memcpy((char*)&pkt + 0x17, msg.c_str(), msg.length());
    handler->SendToDB(&pkt);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x107,"./log/GuildBoard", "SET SUCCESS - GUILD:%u, CHARAC:%u, LEVEL:%u",
        pkt.m_b, pkt.m_d, level + 1);
}

void CGuildBoard::sendMessageToDBMW_GuildAttendance(CServerHandler* handler, int a, int b,
                                                    unsigned int c, unsigned int d)
{
}

void CGuildBoard::sendMessageToDBMW_GuildFund(CServerHandler* handler, int fund, CUser* user)
{
    Packet_DB_Load_Request_Guild_Board_Write pkt;
    pkt.m_b = user->GetGuildKey();
    pkt.m_d = user->GetUniqCharNo();
    std::string msg;
    std::string str1 = g_ServerString_.GetServerString(0x3f0, 0);
    char buf[255] = {0};
    char buf2[255] = {0};
    sprintf(buf, "%s %s ", user->GetCharName(),
            g_ServerString_.GetServerString(0x3e8, 0).c_str());
    msg += buf;
    sprintf(buf2, "%d ", fund);
    msg += buf2;
    msg += str1;
    if (msg.length() > 0x77)
    {
        return;
    }
    memcpy((char*)&pkt + 0x17, msg.c_str(), msg.length());
    handler->SendToDB(&pkt);
    DNF_LOG_SCOPE_LINE(0x15d,"./log/GuildBoard", "SET SUCCESS - GUILD:%u, CHARAC:%u, gold :%d",
        pkt.m_b, pkt.m_d, fund);
}

void CGuildBoard::sendMessageToDBMW_GuildMasterChanging(CServerHandler* handler, CUser* user,
                                                        const char* name)
{
    Packet_DB_Load_Request_Guild_Board_Write pkt;
    pkt.m_b = user->GetGuildKey();
    pkt.m_d = user->GetUniqCharNo();
    std::string msg(name);
    std::string str = g_ServerString_.GetServerString(0x3f1, 0);
    msg += str;
    if (msg.length() > 0x77)
    {
        return;
    }
    memcpy((char*)&pkt + 0x17, msg.c_str(), msg.length());
    handler->SendToDB(&pkt);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x17b,"./log/GuildBoard", "SET SUCCESS - GUILD:%u, CHARAC:%u",
        pkt.m_b, pkt.m_d);
}

void CGuildBoard::printGuildBoard()
{
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >::iterator it =
        m_board.begin();
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >::iterator endit =
        m_board.end();
    std::map<unsigned int, STGuildBoardDBInfo, std::greater<unsigned int> >::iterator cur = it;
    while (cur != endit)
    {
        DNF_LOG_SCOPE_LINE(0x188,"./log/GuildBoard", "\n*%d* %d %s\n",
            cur->first, cur->second.m_guildKey,
            cur->second.m_memo);
        ++cur;
    }
}
