// df_dbmw_r - DNFPacketTranslater (ORIG DNFPacketTranslater.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();
int parse_string(std::vector<std::string>& v, std::string& s, char c);
int get_day_interval(struct tm* a, struct tm* b);

// ---- CGuildManager / WongWork ----
Packet_Monitor_Notify_New_Mail::Packet_Monitor_Notify_New_Mail()
    : PacketHeader(0x514, 0x12)
{
}
Packet_DBMW_Reply_Guild_Mail::Packet_DBMW_Reply_Guild_Mail()
    : PacketHeader(0x433, 0x13)
{
}
Packet_DBMW_Save_Guild_Join_Reply::Packet_DBMW_Save_Guild_Join_Reply()
    : PacketHeader(0x438, 0x1a)
{
}
Packet_DBMW_Add_Buddy_Reply::Packet_DBMW_Add_Buddy_Reply()
    : PacketHeader(0x673, 0x36)
{
    memset((char*)this + 0xe, 0, 0x27);
}
Packet_DBMW_Del_Buddy_Reply::Packet_DBMW_Del_Buddy_Reply()
    : PacketHeader(0x675, 0x31)
{
    memset((char*)this + 0x12, 0, 0x1e);
}
Packet_DBMW_Reponse_BlackList::Packet_DBMW_Reponse_BlackList()
    : PacketHeader(0x5e1, 0x19e)
{
    *(int*)((char*)this + 0xa) = 0;
    for (int i = 9; i != -1; i--)
        new ((STBlackUserDBType*)((char*)this + 0xe + i * 0x28)) STBlackUserDBType;
    memset((char*)this + 0xe, 0, 0x190);
}
Packet_DBMW_Reply_Guild_Create::Packet_DBMW_Reply_Guild_Create()
    : PacketHeader(0x440, 0x2d)
{
    memset((char*)this + 0x16, 0, 0x17);
}
Packet_DBMW_Query_Buddy_Info_Reply::Packet_DBMW_Query_Buddy_Info_Reply()
    : PacketHeader(0x676, 0x4ef)
{
    memset((char*)this + 0xf, 0, 0x4e0);
}
LimitNpcBuyItemResultInfo::LimitNpcBuyItemResultInfo()
    : PacketHeader(0x27dc, 0x176)
{
    *(int*)((char*)this + 0xa) = 0;
    memset((char*)this + 0xe, 0, 0x168);
}
Packet_Monitor_UDP_HeartBeat::Packet_Monitor_UDP_HeartBeat()
    : PacketHeader(0x3ec, 0xb)
{
    m_fieldA = 0xff;
}
Packet_Notice_Guild_Mail_Arrived::Packet_Notice_Guild_Mail_Arrived()
    : PacketHeader(0x415, 0x33)
{
    memset((char*)this + 0xb, 0, 0x28);
}
CApplication* CPacketTranslater::m_pclApp;

void CPacketTranslater::OnSecretShopStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSecretShopStatistic(
            (Packet_Secret_Shop_Statistic*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnSecretShopStatistic() Exception Break",
                  0xeed, 0xef2);
}
void CPacketTranslater::onLoadLimitNpcBuyItemInfo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    LimitNpcBuyItemResultInfo result;
    try
    {
        if (!m_pclApp->m_dbManager.loadLimitNpcBuyItemInfo(
                (LimitNpcBuyItemRequestInfo*)header, &result))
        {
            CMyFileLog log("onLoadLimitNpcBuyItemInfo", 0x132e);
            log("./log/NpcBuyLimitItem",
                "CPacketTranslater::onLoadLimitNpcBuyItemInfo data load fail\n");
            return;
        }
        m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
            (char*)&result, 0x176);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::onLoadLimitNpcBuyItemInfo Exception Break",
                  0x1338, 0x133d);
}
void CPacketTranslater::onUpdateLimitNpcBuyItemInfo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.updateLimitNpcBuyItemInfo(
                (LimitNpcBuyItemUpdate*)header))
        {
            CMyFileLog log("onUpdateLimitNpcBuyItemInfo", 0x134c);
            log("./log/NpcBuyLimitItem",
                "CPacketTranslater::onUpdateLimitNpcBuyItemInfo data update fail\n");
            return;
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::onUpdateLimitNpcBuyItemInfo Exception Break",
                  0x1352, 0x1357);
}
void CPacketTranslater::OnChangeGuildNotifyMessage(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_Guild_Notify_Message* pkt =
            (Packet_DBMW_Request_Guild_Notify_Message*)header;
        m_pclApp->m_dbManager.ChangeGuildNotifyMessage(
            pkt->m_guildId, pkt->m_id, pkt->m_msg);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnChangeGuildNotifyMessage() Exception Break",
                  0x4b9, 0x4be);
}
void CPacketTranslater::OnGuildMasterDelegate(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        char* h = (char*)header;
        if (*(unsigned int*)(h + 0xa) == 0)
            return;
        Packet_DB_Reply_Guild_Master_Delegate reply;
        *(unsigned int*)((char*)&reply + 0xa) = *(unsigned int*)(h + 0xa);
        *(unsigned int*)((char*)&reply + 0xe) = *(unsigned int*)(h + 0xe);
        memcpy((char*)&reply + 0x16, h + 0x13, 0x1d);
        unsigned char grade = 0;
        unsigned int result = 0;
        m_pclApp->m_dbManager.QueryGuildMemberGradeByName(
            *(unsigned char*)(h + 0x12), *(unsigned int*)(h + 0xa), h + 0x13,
            grade, *(unsigned int*)((char*)&reply + 0x12), result);
        int resultCode = 0;
        if (grade == 2)
        {
            if (*(unsigned int*)((char*)&reply + 0x12) == 0 || result == 0)
            {
                resultCode = 0x22;
            }
            else if (!m_pclApp->m_dbManager.GuildMasterDelegate(
                         *(unsigned int*)(h + 0x12),
                         *(unsigned int*)(h + 0xa), *(unsigned int*)(h + 0xe),
                         result, *(unsigned int*)((char*)&reply + 0x12),
                         h + 0x13))
            {
                CMyFileLog log("OnGuildMasterDelegate", 0x628);
                log("./log/GuildModify",
                    "OnGuildMasterDelegate Err(%d) : return false",
                    *(unsigned int*)(h + 0xa));
                resultCode = 2;
            }
        }
        else
        {
            resultCode = 0x56;
        }
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
        CMyFileLog log("OnGuildMasterDelegate", 0x636);
        log("./log/GuildModify",
            "::OnGuildMasterDelegate g(%d) delegater(%d) delegatee(%s) r(%d)",
            *(unsigned int*)(h + 0xa), *(unsigned int*)(h + 0xe), h + 0x13,
            resultCode);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnGuildMasterDelegate() Exception Break",
                  0x63b, 0x640);
}
void CPacketTranslater::OnSendGuildLetter(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Send_Guild_Letter* pkt =
            (Packet_DBMW_Send_Guild_Letter*)header;
        if (pkt->m_guildId == 0)
            return;
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        if (pkt->m_flag == 1)
        {
            if (!m_pclApp->m_dbManager.SendGuildCoinByMail(
                    pkt->m_serverId, pkt->m_guildId,
                    1, 0, 1, pkt->m_subject, pkt->m_content))
            {
                CMyFileLog log("OnSendGuildLetter", 0x4da);
                log("./log/GuildModify",
                    "AwardGuildItemByMail Err(%d) : return false",
                    pkt->m_guildId);
                return;
            }
        }
        else if (pkt->m_flag == -1)
        {
            if (!m_pclApp->m_dbManager.SendGuildLetter(
                    pkt->m_serverId, pkt->m_guildId, pkt->m_content))
            {
                CMyFileLog log("OnSendGuildLetter", 0x4e2);
                log("./log/GuildModify",
                    "OnSendGuildLetter Err(%d) : return false",
                    pkt->m_guildId);
                return;
            }
        }
        Packet_DBMW_Reply_Guild_Mail reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_fieldA;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_guildId;
        *(char*)((char*)&reply + 0x12) = 0;
        gs->SendToServer((char*)&reply, reply.packetSize);
        Packet_Notice_Guild_Mail_Arrived notice;
        *(char*)((char*)&notice + 0xa) = 1;
        *(unsigned int*)((char*)&notice + 0xb) = pkt->m_guildId;
        gs->SendToServer((char*)&notice, notice.packetSize);
        CMyFileLog log("OnSendGuildLetter", 0x507);
        log("./log/GuildMail", "Guild(%d) Message(%s)",
            pkt->m_guildId, pkt->m_content);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSendGuildLetter() Exception Break",
                  0x50c, 0x511);
}
void CPacketTranslater::OnGuildJoin(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_Guild_Join* pkt =
            (Packet_DBMW_Request_Guild_Join*)header;
        if (pkt->m_guildId == 0)
            return;
        Packet_DBMW_Save_Guild_Join_Reply reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_guildId;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_field13;
        *(unsigned int*)((char*)&reply + 0x12) = pkt->m_characNo;
        STGuildJoinInfo join;
        memset(&join, 0, 0x3c);
        join.m_serverId = pkt->m_serverId;
        join.m_guildId = pkt->m_guildId;
        join.m_id = pkt->m_id;
        join.m_fieldC = pkt->m_field13;
        join.m_characNo = pkt->m_characNo;
        strncpy(join.m_characName, pkt->m_characName, 0x1d);
        join.m_lev = pkt->m_lev;
        join.m_growType = pkt->m_growType;
        join.m_job = pkt->m_job;
        join.m_sex = pkt->m_sex;
        memcpy(join.m_bornYear, pkt->m_bornYear, 3);
        if (!m_pclApp->m_dbManager.GuildJoin(
                &join, *(unsigned int*)((char*)&reply + 0x16)))
        {
            CMyFileLog log("OnGuildJoin", 0x56a);
            log("./log/GuildModify",
                "OnGuildJoin Err(g:%d,c:%d,r:%d) : return false",
                pkt->m_guildId, pkt->m_characNo,
                *(unsigned int*)((char*)&reply + 0x16));
            if (*(unsigned int*)((char*)&reply + 0x16) == 0)
                m_pclApp->m_dbManager.DeleteJoinListByInvite(
                    pkt->m_characNo, pkt->m_guildId);
            m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
                (char*)&reply, reply.packetSize);
        }
        CMyFileLog log2("OnGuildJoin", 0x576);
        log2("./log/GuildModify", "::OnGuildJoin g(%d) c(%d) r(%d)",
             pkt->m_guildId, pkt->m_characNo,
             *(unsigned int*)((char*)&reply + 0x16));
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnGuildJoin() Exception Break",
                  0x57b, 0x580);
}
void CPacketTranslater::OnSendMailCoinGuildEvent(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Send_Mail_Coin_Guild_Event* pkt =
            (Packet_DBMW_Send_Mail_Coin_Guild_Event*)header;
        if (pkt->m_guildId == 0)
            return;
        if (pkt->m_count > 0x10)
        {
            CMyFileLog log("OnSendMailCoinGuildEvent", 0x3d4);
            log("./log/GuildEvent",
                "CApplication.OnSendMailCoinGuildEvent Err : return false(%d)",
                pkt->m_guildId);
            return;
        }
        int count = pkt->m_count;
        int absCount = count < 0 ? -count : count;
        std::vector<int> characNos;
        characNos.clear();
        if (!m_pclApp->m_dbManager.AwardGuildCoinByMail(
                pkt->m_serverId, pkt->m_guildId, 1, 1,
                absCount, 0, characNos))
        {
            CMyFileLog log("OnSendMailCoinGuildEvent", 0x3f5);
            log("./log/GuildEvent",
                "CApplication.AwardGuildCoinByMail Err(%d) : return false",
                pkt->m_guildId);
        }
        else if (!characNos.empty())
        {
            Packet_Notify_New_Group_Mail notice;
            int n = characNos.size() > 0x12b ? 0x12c : characNos.size();
            for (int i = 0; i < n; i++)
                *(int*)((char*)&notice + 0xe + i * 4) = characNos[i];
            m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
                (char*)&notice, notice.packetSize);
            characNos.clear();
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSendMailCoinGuildEvent() Exception Break",
                  0x449, 0x44e);
}
void CPacketTranslater::OnDBLoadRequestGuildBoardWrite(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Load_Request_Guild_Board_Write* pkt =
            (Packet_DB_Load_Request_Guild_Board_Write*)header;
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        STGuildBoardDBInfo info;
        if (!m_pclApp->m_dbManager.OnWriteGuildBoard(
                pkt, &info))
        {
            CMyFileLog log("OnDBLoadRequestGuildBoardWrite", 0xfbc);
            log("./log/Except",
                "CPacketTranslater::OnDBLoadRequestGuildBoardWrite()\tGuild Id : %d,\t Query Result : %d\n",
                pkt->m_guildId, 0);
            Packet_DB_Load_Reply_Guild_Board_Write reply;
            *(unsigned short*)((char*)&reply + 0xa) = 1;
            *(unsigned int*)((char*)&reply + 0xc) = pkt->m_guildId;
            *(unsigned int*)((char*)&reply + 0x10) = pkt->m_field13;
            gs->SendToServer((char*)&reply, 0xb9);
            return;
        }
        Packet_DB_Load_Reply_Guild_Board_Write reply;
        *(unsigned int*)((char*)&reply + 0xc) = pkt->m_guildId;
        *(unsigned int*)((char*)&reply + 0x10) = pkt->m_field13;
        memcpy((char*)&reply + 0x14, &info, 0xa5);
        gs->SendToServer((char*)&reply, 0xb9);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnDBLoadRequestGuildBoardWrite() Exception Break",
                  0xfc8, 0xfcd);
}
void CPacketTranslater::OnDBLoadRequestWebGuildBoardWrite(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Load_Request_Web_Guild_Board_Write* pkt =
            (Packet_DB_Load_Request_Web_Guild_Board_Write*)header;
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        STGuildBoardDBInfo info;
        if (!m_pclApp->m_dbManager.OnWriteWebGuildBoard(
                pkt, &info))
        {
            CMyFileLog log("OnDBLoadRequestWebGuildBoardWrite", 0x101a);
            log("./log/Except",
                "CPacketTranslater::OnDBLoadRequestWebGuildBoardWrite()\tGuild Id : %d,\t Query Result : %d\n",
                pkt->m_guildId, 0);
            Packet_DB_Load_Reply_Web_Guild_Board_Write reply;
            *(unsigned short*)((char*)&reply + 0xa) = 1;
            *(unsigned int*)((char*)&reply + 0xc) = pkt->m_guildId;
            *(unsigned int*)((char*)&reply + 0x10) = pkt->m_fieldE;
            gs->SendToServer((char*)&reply, 0xb9);
            return;
        }
        Packet_DB_Load_Reply_Web_Guild_Board_Write reply;
        *(unsigned int*)((char*)&reply + 0xc) = pkt->m_guildId;
        *(unsigned int*)((char*)&reply + 0x10) = pkt->m_fieldE;
        memcpy((char*)&reply + 0x14, &info, 0xa5);
        gs->SendToServer((char*)&reply, 0xb9);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnDBLoadRequestWebGuildBoardWrite() Exception Break",
                  0x1026, 0x102b);
}
void CPacketTranslater::OnDBLoadRequestGuildBoardDelete(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Load_Request_Guild_Board_Delete* pkt =
            (Packet_DB_Load_Request_Guild_Board_Delete*)header;
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        if (!m_pclApp->m_dbManager.OnDeleteGuildBoard(pkt->m_fieldB))
        {
            CMyFileLog log("OnDBLoadRequestGuildBoardDelete", 0xfe9);
            log("./log/Except",
                "CPacketTranslater::OnDBLoadRequestGuildBoardDelete()\tGuild Id : %d,\t Query Result : %d\n",
                pkt->m_fieldF, 0);
            Packet_DB_Load_Reply_Guild_Board_Delete reply;
            *(unsigned short*)((char*)&reply + 0xa) = 1;
            *(unsigned int*)((char*)&reply + 0xc) = pkt->m_fieldF;
            *(unsigned int*)((char*)&reply + 0x10) = pkt->m_field13;
            gs->SendToServer((char*)&reply, 0x18);
            return;
        }
        Packet_DB_Load_Reply_Guild_Board_Delete reply;
        *(unsigned int*)((char*)&reply + 0xc) = pkt->m_fieldF;
        *(unsigned int*)((char*)&reply + 0x10) = pkt->m_field13;
        *(unsigned int*)((char*)&reply + 0x14) = pkt->m_fieldB;
        gs->SendToServer((char*)&reply, 0x18);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnDBLoadRequestGuildBoardDelete() Exception Break",
                  0xff6, 0xffb);
}
void CPacketTranslater::OnLoadGuildAgit(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnLoadGuildAgit() : 0 == m_pclApp"));
    try
    {
        Packet_DB_Load_Guild_Agit* pkt = (Packet_DB_Load_Guild_Agit*)header;
        Packet_Guild_Load_Guild_Agit reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_guildId;
        m_pclApp->m_dbManager.OnLoadGuildAgit(pkt, reply);
        Packet_Guild_Load_Guild_Cargo cargo;
        *(unsigned int*)((char*)&cargo + 0xa) = pkt->m_guildId;
        m_pclApp->m_dbManager.OnLoadGuildCargo(pkt->m_guildId, cargo);
        Packet_Guild_Load_Guild_Cargo_History history;
        *(unsigned int*)((char*)&history + 0xa) = pkt->m_guildId;
        m_pclApp->m_dbManager.OnLoadGuildCargoHistory(
            pkt->m_guildId, history);
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, 0xf);
        gs->SendToServer((char*)&cargo, 0x18ea);
        gs->SendToServer((char*)&history, 0x972);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnLoadGuildAgit() Exception Break",
                  0x95f, 0x964);
}
void CPacketTranslater::OnCreateGuildAgit(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnCreateGuildAgit() : 0 == m_pclApp"));
    try
    {
        Packet_DB_Create_Guild_Agit* pkt =
            (Packet_DB_Create_Guild_Agit*)header;
        Packet_DB_Create_Guild_Agit_Reply reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_guildId;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_fieldE;
        m_pclApp->m_dbManager.OnCreateGuildAgit(pkt, reply);
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, 0x16);
        if (*(int*)((char*)&reply + 0x12) == 0)
        {
            Packet_Guild_Load_Guild_Cargo cargo;
            *(unsigned int*)((char*)&cargo + 0xa) = pkt->m_guildId;
            m_pclApp->m_dbManager.OnLoadGuildCargo(pkt->m_guildId, cargo);
            gs->SendToServer((char*)&cargo, 0x18ea);
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnCreateGuildAgit() Exception Break",
                  0x911, 0x916);
}
void CPacketTranslater::OnDeleteGuildAgit(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnDeleteGuildAgit() : 0 == m_pclApp"));
    try
    {
        Packet_DB_Delete_Guild_Agit* pkt =
            (Packet_DB_Delete_Guild_Agit*)header;
        Packet_DB_Delete_Guild_Agit_Reply reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_guildId;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_fieldE;
        m_pclApp->m_dbManager.OnDeleteGuildAgit(pkt, reply);
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, 0x16);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDeleteGuildAgit() Exception Break",
                  0x932, 0x937);
}
void CPacketTranslater::OnUpgradeGuildAgit(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnUpgradeGuildAgit() : 0 == m_pclApp"));
    try
    {
        Packet_DB_Upgrade_Guild_Agit* pkt =
            (Packet_DB_Upgrade_Guild_Agit*)header;
        Packet_DB_Upgrade_Guild_Agit_Reply reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_guildId;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_fieldE;
        m_pclApp->m_dbManager.OnUpgradeGuildAgit(pkt, reply);
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, 0x16);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnUpgradeGuildAgit() Exception Break",
                  0x980, 0x985);
}
void CPacketTranslater::OnUpdateGuildCargo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnUpdateGuildCargo(
            (Packet_DB_Update_Guild_Cargo*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnUpdateGuildCargo() Exception Break",
                  0xa5e, 0xa63);
}
void CPacketTranslater::OnUpgradeGuildCargo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnUpgradeGuildCargo(
            (Packet_DB_Guild_Cargo_Upgrade*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnUpgradeGuildCargo() Exception Break",
                  0xa8e, 0xa93);
}
void CPacketTranslater::OnInsertGuildCargoHistory(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnInsertGuildCargoHistory(
            (Packet_DB_Insert_Guild_Cargo_History*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnInsertGuildCargoHistory() Exception Break",
                  0xa75, 0xa7a);
}
void CPacketTranslater::OnDeleteToBlackList(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_BlackList* pkt =
            (Packet_DBMW_Request_BlackList*)header;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        int characNo = pkt->m_characNo;
        if (characNo == -1)
        {
            if (!m_pclApp->m_dbManager.QueryCharacNoByName(pkt->m_name, *(unsigned int*)&characNo, 0))
            {
                m_pclApp->m_dbManager.DeleteToBlackListOnly(
                    pkt->m_mid, pkt->m_name);
                CMyFileLog log("OnDeleteToBlackList", 0x287);
                log("./log/BlackList",
                    "m_clDBManager.QueryCharacNoByName Err : return false");
                ms->SendToServer((char*)pkt, pkt->packetSize);
                return;
            }
        }
        if (!m_pclApp->m_dbManager.DeleteToBlackList(
                pkt->m_mid, characNo))
        {
            CMyFileLog log("OnDeleteToBlackList", 0x28f);
            log("./log/BlackList",
                "m_clDBManager.DeleteToBlackList Err : return false");
            ms->SendToServer((char*)pkt, pkt->packetSize);
            return;
        }
        if (pkt->m_characNo == -1)
            pkt->m_characNo = characNo;
        ms->SendToServer((char*)pkt, pkt->packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDeleteToBlackList() Exception Break",
                  0x29c, 0x2a1);
}
void CPacketTranslater::OnRegisterToBlackList(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_BlackList* pkt =
            (Packet_DBMW_Request_BlackList*)header;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        pkt->m_result = 0;
        int characNo = pkt->m_characNo;
        if (characNo == -1)
        {
            if (!m_pclApp->m_dbManager.QueryCharacNoByName(
                    pkt->m_name, *(unsigned int*)&characNo,
                    &pkt->m_result))
            {
                CMyFileLog log("OnRegisterToBlackList", 0x251);
                log("./log/BlackList",
                    "m_clDBManager.QueryCharacNoByName Err : return false");
                ms->SendToServer((char*)pkt, pkt->packetSize);
                return;
            }
        }
        if (!m_pclApp->m_dbManager.RegisterToBlackList(
                pkt->m_mid, characNo, pkt->m_name))
        {
            CMyFileLog log("OnRegisterToBlackList", 0x259);
            log("./log/BlackList",
                "m_clDBManager.RegisterToBlackList Err : return false");
            ms->SendToServer((char*)pkt, pkt->packetSize);
            return;
        }
        if (pkt->m_characNo == -1)
            pkt->m_characNo = characNo;
        ms->SendToServer((char*)pkt, pkt->packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRegisterToBlackList() Exception Break",
                  0x266, 0x26b);
}
void CPacketTranslater::OnRequestBlackListOnLogin(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_BlackList_Login* pkt =
            (Packet_DBMW_Request_BlackList_Login*)header;
        Packet_DBMW_Reponse_BlackList reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_mid;
        if (!m_pclApp->m_dbManager.QueryBlackList(
                pkt->m_mid,
                (STBlackUserDBType*)((char*)&reply + 0xe)))
        {
            CMyFileLog log("OnRequestBlackListOnLogin", 0x2b8);
            log("./log/BlackList",
                "m_clDBManager.QueryBlackList Err : return false");
            return;
        }
        if (pkt->m_fieldE == 0xc9)
            m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
                (char*)&reply, reply.packetSize);
        else if (pkt->m_fieldE == 0xcb)
            m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
                (char*)&reply, reply.packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestBlackListOnLogin() Exception Break",
                  0x2ca, 0x2cf);
}
void CPacketTranslater::OnSaveGuild(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Save_Guild* pkt = (Packet_DBMW_Save_Guild*)header;
        if (pkt->m_fieldCC == 0)
        {
            m_pclApp->m_dbManager.SaveGuildInfo(
                pkt->m_serverId, pkt->m_guildId, pkt->m_info);
        }
        else if (pkt->m_fieldCC == 1)
        {
            m_pclApp->m_dbManager.SaveGuildSkill(
                pkt->m_serverId, pkt->m_guildId, pkt->m_info);
            CMyFileLog log("OnSaveGuild", 0xca);
            log("./log/GuildModify",
                "::OnSaveGuild s(%d) g(%d) k(%d) g_level(%d) g_cnt(%d) g_exp(%d) g_sub_cnt(%d) power_side(%d) power_war_p(%d) agit(%d) power_join_cnt(%d)",
                pkt->m_serverId, pkt->m_guildId, pkt->m_fieldCC,
                pkt->m_info.m_lev, pkt->m_info.m_memberCount,
                pkt->m_info.m_guildExp, pkt->m_info.m_pad2D[0],
                pkt->m_info.m_powerSide, pkt->m_info.m_powerWarPoint,
                pkt->m_info.m_guildAgitFlag, pkt->m_info.m_powerJoinCount);
        }
    }
    DNF_CATCH_LOG_PRINTF("./log/Except",
                         "CPacketTranslater::OnSaveGuild() Exception Break",
                         0xd8, 0xde);
}
void CPacketTranslater::OnGuildSecede(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Request_Guild_Secede* pkt =
            (Packet_DB_Request_Guild_Secede*)header;
        if (pkt->m_guildId == 0)
            return;
        Packet_DB_Reply_Guild_Secede reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_guildId;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_characNo;
        *(char*)((char*)&reply + 0x1a) = pkt->m_grade;
        *(unsigned int*)((char*)&reply + 0x1b) = pkt->m_secedeType;
        memcpy((char*)&reply + 0x1f, pkt->m_characName, 0x1d);
        if (!m_pclApp->m_dbManager.GuildSecede(
                pkt,
                *(unsigned int*)((char*)&reply + 0x12),
                *(unsigned int*)((char*)&reply + 0x3d),
                *(unsigned int*)((char*)&reply + 0x16)))
        {
            CMyFileLog log("OnGuildSecede", 0x5e1);
            log("./log/GuildModify",
                "::OnGuildSecede Err g(%d) n(%s) c(%d) r(%d) f(%d)",
                pkt->m_guildId, pkt->m_characName,
                *(unsigned int*)((char*)&reply + 0x12),
                *(unsigned int*)((char*)&reply + 0x16),
                pkt->m_grade);
        }
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
        CMyFileLog log2("OnGuildSecede", 0x5ed);
        log2("./log/GuildModify", "::OnGuildSecede g(%d) c(%d) r(%d) f(%d)",
             pkt->m_guildId,
             *(unsigned int*)((char*)&reply + 0x12),
             *(unsigned int*)((char*)&reply + 0x16),
             pkt->m_grade);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnGuildSecede() Exception Break",
                  0x5f6, 0x5fb);
}
void CPacketTranslater::OnSaveGuildMember(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Save_Guild_Member* pkt =
            (Packet_DBMW_Save_Guild_Member*)header;
        m_pclApp->m_dbManager.SaveGuildMember(
            pkt->m_serverId, pkt->m_guildId, pkt->m_info, pkt->m_fieldF,
            pkt->m_field2D);
    }
    DNF_CATCH_LOG_PRINTF("./log/Except.log",
                         "CPacketTranslater::OnSaveGuildMember() Exception Break",
                         0x1c7, 0x1cd);
}
void CPacketTranslater::OnSaveGuildWarInfo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Save_Guild_War_Info* pkt =
            (Packet_DB_Save_Guild_War_Info*)header;
        if (!m_pclApp->m_dbManager.SaveGuildWarPointList(
                pkt->m_serverId, pkt->m_guildIds, pkt->m_points))
        {
            CMyFileLog log("OnSaveGuildWarInfo", 0x201);
            log("./log/Guild",
                "m_clDBManager.OnSaveGuildWarInfo Err : return false");
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSaveGuildWarInfo() Exception Break",
                  0x207, 0x20c);
}
void CPacketTranslater::OnSavePowerWarBonusPoint(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnSavePowerWarBonusPoint() : 0 == m_pclApp"));
    try
    {
        Packet_DB_Save_Power_War_Bonus_Point* pkt =
            (Packet_DB_Save_Power_War_Bonus_Point*)header;
        m_pclApp->m_dbManager.OnSavePowerWarBonusPoint(
            pkt);
        if (pkt->m_count > 0)
        {
            Packet_Notify_New_Group_Mail notice;
            int n = pkt->m_count > 0x12c ? 0x12c : pkt->m_count;
            for (int i = 0; i < n; i++)
                *(int*)((char*)&notice + 0xe + i * 4) =
                    pkt->m_entries[i].m_characNo;
            m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
                (char*)&notice, notice.packetSize);
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSavePowerWarBonusPoint() Exception Break",
                  0xea5, 0xeaa);
}
void CPacketTranslater::OnSavePowerWarPoint(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Save_Power_War_Point* pkt =
            (Packet_DB_Save_Power_War_Point*)header;
        m_pclApp->m_dbManager.SavePowerWarPoint(
            pkt);
        CMyFileLog log("OnSavePowerWarPoint", 0x6a4);
        log("./log/QueryGuildBooting", "QueryGuildBooting A_Side(%d) B_Side(%d)",
            pkt->m_bSidePoint, pkt->m_aSidePoint);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestGuildCreate() Exception Break",
                  0x6a8, 0x6ad);
}
void CPacketTranslater::OnSavePowerWarStatueRanker(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnSavePowerWarStatueRanker() : 0 == m_pclApp"));
    try
    {
        m_pclApp->m_dbManager.OnSavePowerWarStatueRanker(
            (Packet_DB_Save_Power_War_Statue_Ranker*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSavePowerWarStatueRanker() Exception Break",
                  0x8e4, 0x8e9);
}
void CPacketTranslater::OnSavePowerWarPointReward(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnSavePowerWarPoint() : 0 == m_pclApp"));
    try
    {
        m_pclApp->m_dbManager.OnSavePowerWarPointReward(
            (Packet_DB_Save_Power_War_Point_Reward*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSavePowerWarPoint() Exception Break",
                  0x8cb, 0x8d0);
}
void CPacketTranslater::OnLoadPeriodicMessage(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_Result_Loading_Periodic_Message reply;
        if (!m_pclApp->m_dbManager.OnLoadPeriodicMessage(
                (Packet_Load_Periodic_Message*)header, &reply))
            return;
        m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
            (char*)&reply, 0x212);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnLoadPeriodicMessage() Exception Break",
                  0x104d, 0x1052);
}
void CPacketTranslater::OnDBMWInsertMail(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Insert_Mail* pkt = (Packet_DBMW_Insert_Mail*)header;
        if (pkt->m_delayHours != 0)
        {
            time_t now = time(0);
            struct tm* lt = localtime(&now);
            lt->tm_hour += 1;
            lt->tm_min = 0;
            lt->tm_sec = 0;
            long nextHour = mktime(lt);
            long occTime = nextHour + pkt->m_delayHours * 0x15180;
            int letterNo = (occTime - 0x44a53c70) / 0x15180;
            if (!m_pclApp->m_dbManager.InsertMail(
                    pkt->m_characNo, pkt->m_subject, pkt->m_content,
                    pkt->m_fieldE, letterNo, pkt->m_field12, pkt->m_field16))
            {
                CMyFileLog log("OnDBMWInsertMail", 0xd1e);
                log("./log/GuildEvent",
                    "CPacketTranslater.OnDBMWInsertMail Err(%d) : return false",
                    pkt->m_characNo);
                return;
            }
        }
        Packet_Monitor_Notify_New_Mail notice;
        *(unsigned int*)((char*)&notice + 0xa) = pkt->m_characNo;
        m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
            (char*)&notice, notice.packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDBMWInsertMail() Exception Break",
                  0xd29, 0xd2e);
}
void CPacketTranslater::OnQueryMember(PacketHeader* header)
{
    try
    {
        Packet_DB_Reply_Query_Member reply;
        if (m_pclApp)
        {
            Packet_DBMW_Query_Member* pkt = (Packet_DBMW_Query_Member*)header;
            reply.m_fieldB = pkt->m_characNo;
            if (!m_pclApp->m_dbManager.QueryMember(
                    pkt->m_characNo, reply))
            {
                CMyFileLog log("OnQueryMember", 0x117);
                log("./log/QueryErr",
                    "CPacketTranslater::OnQueryMember() Error, member_id(%d)",
                    pkt->m_characNo);
                CMonitorServer* ms =
                    m_pclApp->m_serverHandler->GetMonitorServer();
                int size =
                    *(unsigned char*)((char*)&reply + 0x3e) * 0x27 + 0x3f;
                ms->SendToServer((char*)&reply, size);
            }
        }
    }
    DNF_CATCH_LOG_PRINTF("./log/Except",
                         "CPacketTranslater::OnQueryMember() Exception Break",
                         0x124, 0x12a);
}
void CPacketTranslater::OnQueryGuildMember(PacketHeader* header)
{
    try
    {
        Packet_DB_Reply_Query_Guild_Member reply;
        if (m_pclApp)
        {
            Packet_DBMW_Query_Guild_Member* pkt =
                (Packet_DBMW_Query_Guild_Member*)header;
            reply.m_fieldF = pkt->m_guildId;
            if (!m_pclApp->m_dbManager.QueryGuildMember(
                    pkt->m_serverId, pkt->m_guildId,
                    reply))
            {
                CMyFileLog log("OnQueryGuildMember", 0x98);
                log("./log/Except",
                    "CPacketTranslater::OnQueryGuildMember() Query Error : %d, Char No : %d, Guild Id : %d",
                    *(unsigned int*)((char*)&reply + 0xb),
                    pkt->m_guildId,
                    *(unsigned char*)((char*)&reply + 0xa));
                m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
                    (char*)&reply, 0x2d);
            }
        }
    }
    DNF_CATCH_LOG_PRINTF("./log/Except",
                         "CPacketTranslater::OnQueryGuildMember() Exception Break",
                         0xa8, 0xae);
}
void CPacketTranslater::OnQueryTodayGuildMemeber(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnQueryTodayGuildMemeber() : 0 == m_pclApp"));
    try
    {
        Packet_DBMW_Request_Today_Guild_Member* pkt =
            (Packet_DBMW_Request_Today_Guild_Member*)header;
        Packet_Reply_Today_Guild_Member reply;
        m_pclApp->m_dbManager.QueryTodayGuildMember(
            pkt->m_guildId, reply);
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryTodayGuildMemeber() Exception Break",
                  0x106a, 0x106f);
}
void CPacketTranslater::OnRequestGuildCreate(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_Guild_Create* pkt =
            (Packet_DBMW_Request_Guild_Create*)header;
        Packet_DBMW_Reply_Guild_Create reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_characNo;
        m_pclApp->m_dbManager.QueryGuildCreate(
            pkt,
            *(unsigned int*)((char*)&reply + 0xe),
            *(unsigned int*)((char*)&reply + 0x12));
        memcpy((char*)&reply + 0x16, pkt->m_guildName, 0x16);
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
        CMyFileLog log("OnRequestGuildCreate", 0x65f);
        log("./log/GuildModify",
            "::OnRequestGuildCreate g(%d) c(%d) r(%d)",
            *(unsigned int*)((char*)&reply + 0x12),
            pkt->m_characNo,
            *(unsigned int*)((char*)&reply + 0xe));
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestGuildCreate() Exception Break",
                  0x663, 0x668);
}
void CPacketTranslater::OnSavePowerWarUserRank(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnSavePowerWarUserRank() : 0 == m_pclApp"));
    try
    {
        m_pclApp->m_dbManager.OnSavePowerWarUserRank(
            (Packet_DB_Save_Power_War_User_Rank*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSavePowerWarUserRank() Exception Break",
                  0x899, 0x89e);
}
void CPacketTranslater::OnSavePowerWarGuildRank(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnSavePowerWarGuildRank() : 0 == m_pclApp"));
    try
    {
        m_pclApp->m_dbManager.OnSavePowerWarGuildRank(
            (Packet_DB_Save_Power_War_Guild_Rank*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSavePowerWarGuildRank() Exception Break",
                  0x8b2, 0x8b7);
}
void CPacketTranslater::OnRequestApproveJoinGuild(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_Approve_Join_Guild* pkt =
            (Packet_DBMW_Request_Approve_Join_Guild*)header;
        Packet_DB_Response_Approve_Join_Guild reply;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_guildId;
        *(unsigned int*)((char*)&reply + 0x12) = pkt->m_id;
        *(unsigned int*)((char*)&reply + 0x16) = pkt->m_characNo;
        if (!m_pclApp->m_dbManager.OnGuildJoinByListApprove(
                pkt->m_guildId, pkt->m_serverId,
                pkt->m_id, pkt->m_characNo,
                *(STGuildJoinInfo*)((char*)&reply + 0x1a),
                *(unsigned int*)((char*)&reply + 0xa)))
        {
            if (*(unsigned int*)((char*)&reply + 0xa) == 0)
                *(int*)((char*)&reply + 0xa) = 1;
            CMyFileLog log("OnRequestApproveJoinGuild", 0x52a);
            log("./log/GuildModify",
                "OnGuildJoin Err(g:%d,c:%d,r:%d) : return false",
                *(unsigned int*)((char*)&reply + 0xa),
                pkt->m_characNo, pkt->m_guildId);
            m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
                (char*)&reply, reply.packetSize);
            CMyFileLog log2("OnRequestApproveJoinGuild", 0x530);
            log2("./log/GuildModify", "::OnGuildJoin g(%d) c(%d) r(%d)",
                 *(unsigned int*)((char*)&reply + 0xa),
                 pkt->m_characNo, pkt->m_guildId);
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestApproveJoinGuild() Exception Break",
                  0x534, 0x539);
}
void CPacketTranslater::OnInsertUdpCharacteristic(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(std::string(
            "CPacketTranslater::OnInsertUdpCharacteristic() : 0 == m_pclApp"));
    try
    {
        m_pclApp->m_dbManager.InsertUdpCharacteristic(
            (Packet_Udp_Characteristic*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnInsertUdpCharacteristic() Exception Break",
                  0x87f, 0x884);
}
void CPacketTranslater::OnDelBuddy(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Del_Buddy* pkt = (Packet_DBMW_Del_Buddy*)header;
        Packet_DBMW_Del_Buddy_Reply reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_mid;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_characNo;
        memcpy((char*)&reply + 0x12, pkt->m_name, 0x1d);
        m_pclApp->m_dbManager.DelBuddy(pkt->m_mid, pkt->m_characNo);
        m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
            (char*)&reply, reply.packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDelBuddy() Exception Break",
                  0x319, 0x31e);
}
void CPacketTranslater::OnEndGuildWar(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Save_Guild_War_Info* pkt =
            (Packet_DB_Save_Guild_War_Info*)header;
        if (!m_pclApp->m_dbManager.SaveGuildWarPointList(
                pkt->m_serverId, pkt->m_guildIds, pkt->m_points))
        {
            CMyFileLog log("OnEndGuildWar", 0x223);
            log("./log/GuildWar",
                "m_clDBManager.OnEndGuildWar Err : return false");
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnEndGuildWar() Exception Break",
                  0x229, 0x22e);
}
void CPacketTranslater::onStartGameEventFromServer(PacketHeader* header)
{
    try
    {
        Packet_StartGameEventFromServer* pkt =
            (Packet_StartGameEventFromServer*)header;
        CMyFileLog log("onStartGameEventFromServer", 0x1567);
        log("./log/AradOnly",
            "CPacketTranslater::onStartGameEventFromServer data. (event:%d)\n",
            pkt->m_eventType);
        if (!m_pclApp->m_dbManager.insertServerGameEvent(
                pkt))
        {
            CMyFileLog log2("onStartGameEventFromServer", 0x156b);
            log2("./log/AradOnly",
                 "CPacketTranslater::onStartGameEventFromServer fail\n");
            return;
        }
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* buf = tcp->makePacketHeader(0x27fb, 0x1a);
            *(int*)(buf + 0xa) = pkt->m_eventType;
            *(int*)(buf + 0xe) = pkt->m_eventFlag;
            *(int*)(buf + 0x12) = pkt->m_serverId;
            *(int*)(buf + 0x16) = pkt->m_parameter1;
            tcp->SendToServer(buf);
        }
        else
        {
            m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
                (char*)pkt, 0x1a);
        }
    }
    DNF_CATCH_LOG2("./log/AradOnly",
                   "CPacketTranslater::onStartGameEventFromServer Exception Break",
                   "CPacketTranslater::onRegistServerEvent Exception Break",
                   0x1583, 0x1588);
}
void CPacketTranslater::onEndGameEventFromServer(PacketHeader* header)
{
    try
    {
        Packet_StopGameEventFromServer* pkt =
            (Packet_StopGameEventFromServer*)header;
        CMyFileLog log("onEndGameEventFromServer", 0x1592);
        log("./log/AradOnly",
            "CPacketTranslater::onEndGameEventFromServer data. (event:%d)\n",
            pkt->m_eventType);
        if (!m_pclApp->m_dbManager.updateServerGameEvent(
                pkt))
        {
            CMyFileLog log2("onEndGameEventFromServer", 0x1596);
            log2("./log/AradOnly",
                 "CPacketTranslater::onEndGameEventFromServer fail\n");
            return;
        }
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* buf = tcp->makePacketHeader(0x27fc, 0x16);
            *(int*)(buf + 0xa) = pkt->m_eventType;
            *(int*)(buf + 0x12) = pkt->m_endTime;
            *(int*)(buf + 0xe) = pkt->m_serverId;
            tcp->SendToServer(buf);
        }
        else
        {
            m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
                (char*)pkt, 0x16);
        }
    }
    DNF_CATCH_LOG("./log/AradOnly",
                  "CPacketTranslater::onEndGameEventFromServer Exception Break",
                  0x15ac, 0x15b1);
}
void CPacketTranslater::OnSaveMember(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Save_Member* pkt = (Packet_DBMW_Save_Member*)header;
        if (pkt->m_type == 1)
        {
            m_pclApp->m_dbManager.SaveMemberInsert(
                pkt->m_fieldB, pkt->m_fieldF, pkt->m_field13);
        }
        else if (pkt->m_type == 2)
        {
            m_pclApp->m_dbManager.SaveMemberDelete(
                pkt->m_fieldB, pkt->m_fieldF, pkt->m_field13);
        }
    }
    DNF_CATCH_LOG_PRINTF("./log/Except.log",
                         "CPacketTranslater::OnSaveMember() Exception Break",
                         0x148, 0x14e);
}
void CPacketTranslater::OnSaveMemberUpdateCharInfo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Save_Member_Update_CharInfo* pkt =
            (Packet_DBMW_Save_Member_Update_CharInfo*)header;
        if (!m_pclApp->m_dbManager.UpdateMemberKeyInCharacInfo(
                pkt->m_serverId, pkt->m_characNo))
        {
            CMyFileLog log("OnSaveMemberUpdateCharInfo", 0x1a4);
            log("./log/DBMemberErr", "Member key Update Error %d\n",
                pkt->m_characNo);
        }
    }
    DNF_CATCH_LOG_PRINTF("./log/Except.log",
                         "CPacketTranslater::OnSaveMemberUpdateCharInfo() Exception Break",
                         0x1a9, 0x1af);
}
void CPacketTranslater::OnQueryGuildBooting(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_Guild_Booting* pkt =
            (Packet_DBMW_Request_Guild_Booting*)header;
        Packet_DB_Query_Reply_On_Guild_Booting reply;
        m_pclApp->m_dbManager.QueryGuildBooting(
            reply, pkt->m_serverId);
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
        CMyFileLog log("OnQueryGuildBooting", 0x685);
        log("./log/QueryGuildBooting",
            "QueryGuildBooting A_Side(%d) B_Side(%d) Winner(%d)",
            *(unsigned int*)((char*)&reply + 0xa),
            *(unsigned int*)((char*)&reply + 0xe),
            *(signed char*)((char*)&reply + 0x12));
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestGuildCreate() Exception Break",
                  0x689, 0x68e);
}
void CPacketTranslater::OnUpdateTowerOfDespairStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_TowerOfDespair_Statistic_STD* pkt =
            (Packet_TowerOfDespair_Statistic_STD*)header;
        if (!m_pclApp->m_dbManager.QueryTowerOfDespairStatistic(
                pkt))
            return;
        CMyFileLog log("OnUpdateTowerOfDespairStatistic", 0x12e5);
        log("./log/statistic", "TOD Statistic Error\nTOD uv(%d)",
            pkt->m_uv);
        for (int i = 1; i <= 0x64; i++)
        {
            CMyFileLog log2("OnUpdateTowerOfDespairStatistic", 0x12e7);
            log2("./log/statistic", "TOD Layer(%d), enter(%d), succ(%d)", i,
                 pkt->m_entries[i].m_fieldE, pkt->m_entries[i].m_field12);
        }
    }
    DNF_CATCH_LOG_CDNF("./log/Except.log",
                       "CPacketTranslater::OnUpdateTowerOfDespairStatistic() Exception Break",
                       0x12ed);
}
void CPacketTranslater::OnDBMWVillageAttackRank(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        bool flag = false;
        int a = 0;
        int b = 0;
        if (!m_pclApp->m_dbManager.GetVillageAttackedRank(
                (Packet_DB_VillageAttackedRank*)header, flag, a, b))
        {
            CMyFileLog log("OnDBMWVillageAttackRank", 0xd5a);
            log("./log/GuildEvent",
                "CPacketTranslater.OnDBMWVillageAttackRank Err : return false");
            return;
        }
        if (flag)
        {
            Packet_Monitor_Event_Start ev;
            *(int*)((char*)&ev + 0xa) = 4;
            *(unsigned short*)((char*)&ev + 0xe) = (unsigned short)a;
            *(unsigned short*)((char*)&ev + 0x10) = (unsigned short)b;
            m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
                (char*)&ev, ev.packetSize);
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDBMWInsertMail() Exception Break",
                  0xd69, 0xd6e);
}
void CPacketTranslater::onItemLimitEditionLoadDataReq(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_Item_Limit_Edition_Load_Data_Rpy reply;
        if (!m_pclApp->m_dbManager.onItemLimitEditionLoadData(
                (const Packet_Item_Limit_Edition_Load_Data_Req*)header,
                &reply))
        {
            CMyFileLog log("onItemLimitEditionLoadDataReq", 0xaa4);
            log("./log/LimitEdition",
                "CPacketTranslater::onItemLimitEditionLoadDataReq data load fail\n");
            return;
        }
        m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
            (char*)&reply, 0x7ef);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break",
                  0xaae, 0xab3);
}
void CPacketTranslater::onCollectItemsGm(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Collect_Items_Gm* pkt =
            (Packet_DBMW_Collect_Items_Gm*)header;
        m_pclApp->m_dbManager.updateCollectItemsGm(
            pkt->m_serverInfo, pkt->m_fieldF, pkt->m_fieldB,
            pkt->m_field13);
        Packet_CollectItemsResult reply;
        *(unsigned int*)((char*)&reply + 0xa) = pkt->m_fieldF;
        *(unsigned int*)((char*)&reply + 0xe) = pkt->m_fieldB;
        *(int*)((char*)&reply + 0x12) = pkt->m_field13;
        m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
            (char*)&reply, reply.packetSize);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::onCollectItemsGm Exception Break",
                  0x1406, 0x140b);
}
void CPacketTranslater::onInsertHolePunchingResult(PacketHeader* header)
{
    try
    {
        CMyFileLog log("onInsertHolePunchingResult", 0x15bd);
        log("./log/AradOnly",
            "CPacketTranslater::onInsertHolePunchingResult.\n");
        if (!m_pclApp->m_dbManager.insertHolePunchingResult(
                (Packet_GameServer2Statisctics2DBServer*)header))
        {
            CMyFileLog log2("onInsertHolePunchingResult", 0x15c0);
            log2("./log/AradOnly",
                 "CPacketTranslater::insertHolePunchingResult fail\n");
            return;
        }
    }
    DNF_CATCH_LOG("./log/AradOnly",
                  "CPacketTranslater::onInsertHolePunchingResult Exception Break",
                  0x15c6, 0x15cb);
}
void CPacketTranslater::OnSaveMemberExp(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Save_Member_Exp* pkt =
            (Packet_DBMW_Save_Member_Exp*)header;
        m_pclApp->m_dbManager.SaveMemberExp(
            pkt->m_characNo, pkt->m_masterNo, pkt->m_exp);
    }
    DNF_CATCH_LOG_PRINTF("./log/Except.log",
                         "CPacketTranslater::OnSaveMember() Exception Break",
                         0x168, 0x16e);
}
void CPacketTranslater::OnSavePowerSecedeTime(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Save_Power_Secede_Time* pkt =
            (Packet_DBMW_Save_Power_Secede_Time*)header;
        m_pclApp->m_dbManager.UpdatePowerSecedeTime(
            pkt->m_serverId, pkt->m_secedeTime);
    }
    DNF_CATCH_LOG_PRINTF("./log/Except",
                         "CPacketTranslater::OnSaveGuild() Exception Break",
                         0xf8, 0xfe);
}
void CPacketTranslater::onItemLimitEditionUpdateData(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.onItemLimitEditionUpdateData(
                (const Packet_Item_Limit_Edition_Update*)header))
        {
            CMyFileLog log("onItemLimitEditionUpdateData", 0xac1);
            log("./log/LimitEdition",
                "CPacketTranslater::onItemLimitEditionLoadDataReq data load fail\n");
            return;
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::onItemLimitEditionBuyableRequest Exception Break",
                  0xac7, 0xacc);
}
void CPacketTranslater::onCompatibilityIndex(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.updateCompatibilityIndex(
                (Packet_Stat_Compatibility_Index*)header))
        {
            CMyFileLog log("onCompatibilityIndex", 0x13a5);
            log("./log/Query",
                "CPacketTranslater::onCompatibilityIndex data update fail\n");
            return;
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::onCompatibilityIndex Exception Break",
                  0x13ab, 0x13b0);
}
void CPacketTranslater::OnP2PStatistics(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.QueryP2PStatistics(
                (Packet_P2P_Statistics*)header))
        {
            CMyFileLog log("OnP2PStatistics", 0x13c1);
            log("./log/Query", "CPacketTranslater::OnP2PStatistics()");
            return;
        }
    }
    DNF_CATCH_LOG2_NN("./log/Except.log", "./log/Except",
                      "CPacketTranslater::OnP2PStatistics() Exception Break",
                      "CPacketTranslater::onCompatibilityIndex Exception Break",
                      0x13c6, 0x13cb);
}
void CPacketTranslater::OnDBMWQueryMsg(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.QueryMsg(
                (Packet_DBMW_Query_Msg*)header))
        {
            CMyFileLog log("OnDBMWQueryMsg", 0xd3d);
            log("./log/GuildEvent",
                "CPacketTranslater.OnDBMWQueryMsg Err : return false");
            return;
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDBMWQueryMsg() Exception Break",
                  0xd43, 0xd48);
}
void CPacketTranslater::OnDBMWRandomboxStatic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.UpdateRandomboxStatistic(
                (Packet_Randombox_statistic_DB*)header))
        {
            CMyFileLog log("OnDBMWRandomboxStatic", 0xdbc);
            log("./log/statistic",
                "CPacketTranslater.OnDBMWRandomboxStatic Err : return false");
            return;
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDBMWRandomboxStatic() Exception Break",
                  0xdc1, 0xdc6);
}
void CPacketTranslater::OnBloodDungeonStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.GetDBMWStatistic(
                (Packet_DBMW_Query_String*)header))
        {
            CMyFileLog log("OnBloodDungeonStatistic", 0xb66);
            log("./log/PowerStatistic",
                "CPacketTranslater::OnBloodDungeonStatistic insert fail\n");
            return;
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnBloodDungeonStatistic Exception Break",
                  0xb6c, 0xb71);
}
void CPacketTranslater::OnDBMWCreateEmblemStatic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.UpdateCreateEmblemStatistic(
                (Packet_Emblem_Create_Statistic_DB*)header))
        {
            CMyFileLog log("OnDBMWCreateEmblemStatic", 0xd9f);
            log("./log/statistic",
                "CPacketTranslater.OnDBMWCreateEmblemStatic Err : return false");
            return;
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDBMWCreateEmblemStatic() Exception Break",
                  0xda4, 0xda9);
}
void CPacketTranslater::OnDBMWDisjointAvatarStatic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        puts("TEST : OnDBMWDisjointAvatarStatic");
        if (!m_pclApp->m_dbManager.UpdateDisjointAvatarStatistic(
                (Packet_Avater_Disjoint_Statistic_DB*)header))
        {
            CMyFileLog log("OnDBMWDisjointAvatarStatic", 0xd81);
            log("./log/statistic",
                "CPacketTranslater.OnDBMWDisjointAvatarStatic Err : return false");
            return;
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDBMWDisjointAvatarStatic() Exception Break",
                  0xd86, 0xd8b);
}
void CPacketTranslater::onCollectItemsUpdate(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Collect_Items_Update* pkt =
            (Packet_DBMW_Collect_Items_Update*)header;
        Packet_CollectItemsResult reply;
        unsigned char flag = 0;
        m_pclApp->m_dbManager.selectCollectItems(
            pkt->m_serverInfo,
            *(int*)((char*)&reply + 0xe), *(int*)((char*)&reply + 0xa),
            *(unsigned int*)((char*)&reply + 0x12), flag);
        int diff = pkt->m_fieldA - *(int*)((char*)&reply + 0xa);
        m_pclApp->m_dbManager.updateCollectItems(
            pkt->m_serverInfo, diff,
            *(unsigned int*)((char*)&reply + 0x12), flag);
        if (flag == 0 && pkt->m_field13 == 0 && diff < 0)
        {
            m_pclApp->m_serverHandler->GetMonitorServer()->SendToServer(
                (char*)&reply, reply.packetSize);
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::onCollectItemsUpdate Exception Break",
                  0x13e7, 0x13ec);
}
void CPacketTranslater::OnDBMWConnectionCheck(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Connection_Check* pkt =
            (Packet_DBMW_Connection_Check*)header;
        CMyFileLog log("OnDBMWConnectionCheck", 0x1df);
        log("./log/Udp", "%d Server Connection Complete!",
            pkt->m_serverId);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDBMWConnectionCheck() Exception Break",
                  0x1e3, 0x1e8);
}
void CPacketTranslater::OnChangeCharacName(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.ChangeCharName(
            (Packet_DBMW_Change_Char_Name*)header);
        m_pclApp->m_dbManager.ChangePvPBuddyName(
            (Packet_DBMW_Change_Char_Name*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestGuildCreate() Exception Break",
                  0x6e5, 0x6ea);
}
void CPacketTranslater::OnServerMatchData(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnServerMatchData(
            (Packet_Server_Match_data_DBMW*)header);
    }
    DNF_CATCH_LOG_NN("./log/Except.log",
                     "CPacketTranslater::OnServerMatchData() Exception Break",
                     0xed5, 0xeda);
}
void CPacketTranslater::OnManagerEventTriggerAck(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnManagerEventTriggerAck(
            (Packet_Manager_Event_Trigger_Ack*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnManagerEventTriggerAck() Exception Break",
                  0xf1e, 0xf23);
}
void CPacketTranslater::OnRecvLoadingTimeReport(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSaveLoadingTimeReport(
            (Packet_DBMW_Loading_Time_Report*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRecvLoadingTimeReport() Exception Break",
                  0xa09, 0xa0e);
}
void CPacketTranslater::OnSaveFatigueBatteryStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSaveFatigueBattery(
            (Packet_DBMW_Fatigue_Battery_Money_Statistic*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRecvLoadingTimeReport() Exception Break",
                  0xa46, 0xa4b);
}
void CPacketTranslater::OnServeQueueLoadStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    Packet_Server_Queue_Load_Statistic* pkt =
        (Packet_Server_Queue_Load_Statistic*)header;
    unsigned char fieldA;
    if (pkt->m_fieldA == 0xc8)
    {
        fieldA = (unsigned char)(pkt->m_fieldA - m_pclApp->m_appConfig->Get_DbmwType());
    }
    else
    {
        fieldA = pkt->m_fieldA;
    }
    try
    {
        m_pclApp->m_dbManager.SaveServerQueueLoadStatistic(
            fieldA, pkt->m_fieldB, pkt->m_fieldC);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnServeQueueLoadStatistic() Exception Break",
                  0xae8, 0xaed);
}
void CPacketTranslater::OnDBLoadRequestGuildBoardOpen(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Load_Request_Guild_Board_Open* pkt =
            (Packet_DB_Load_Request_Guild_Board_Open*)header;
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        int count = 0;
        STGuildBoardDBInfo boards[0x31];
        if (!m_pclApp->m_dbManager.OnLoadGuildBoard(
                *(int*)((char*)pkt + 0xa), count, boards))
        {
            CMyFileLog log("OnDBLoadRequestGuildBoardOpen", 0xf8a);
            log("./log/Except",
                "CPacketTranslater::OnDBLoadRequestGuildBoardOpen()\tGuild Id : %d,\t Query Result : %d\n",
                *(int*)((char*)pkt + 0xa), 0);
            Packet_DB_Load_Reply_Guild_Board_Open reply;
            *(unsigned short*)((char*)&reply + 0xa) = 1;
            *(char*)((char*)&reply + 0xc) = 1;
            *(int*)((char*)&reply + 0xd) = *(int*)((char*)pkt + 0xa);
            *(int*)((char*)&reply + 0x11) = *(int*)((char*)pkt + 0xe);
            *(char*)((char*)&reply + 0x15) = 0;
            gs->SendToServer((char*)&reply, 0x688);
            return;
        }
        int pageCount = count / 10;
        int rem = count - pageCount * 10;
        for (int page = 0; page < pageCount; page++)
        {
            Packet_DB_Load_Reply_Guild_Board_Open reply;
            *(char*)((char*)&reply + 0xc) = 0;
            *(int*)((char*)&reply + 0xd) = *(int*)((char*)pkt + 0xa);
            *(int*)((char*)&reply + 0x11) = *(int*)((char*)pkt + 0xe);
            *(char*)((char*)&reply + 0x15) = 0xa;
            for (int i = 0; i <= 9; i++)
            {
                char* dst = (char*)&reply + 0x16 + i * 0xa5;
                char* src = (char*)boards + (page * 10 + i) * 0xa5;
                memcpy(dst, src, 0x78);
                *(int*)(dst + 0x78) = *(int*)(src + 0x78);
                *(int*)(dst + 0x7c) = *(int*)(src + 0x7c);
                *(int*)(dst + 0x88) = *(int*)(src + 0x88);
                memcpy(dst + 0x84, src + 0x84, 0x21);
            }
            if (page + 1 == pageCount && rem == 0)
                *(char*)((char*)&reply + 0xc) = 1;
            gs->SendToServer((char*)&reply, 0x688);
        }
        if (rem != 0)
        {
            Packet_DB_Load_Reply_Guild_Board_Open reply;
            *(char*)((char*)&reply + 0xc) = 1;
            *(int*)((char*)&reply + 0xd) = *(int*)((char*)pkt + 0xa);
            *(int*)((char*)&reply + 0x11) = *(int*)((char*)pkt + 0xe);
            *(char*)((char*)&reply + 0x15) = (char)rem;
            for (int i = 0; i < rem; i++)
            {
                char* dst = (char*)&reply + 0x16 + i * 0xa5;
                char* src = (char*)boards + (pageCount * 10 + i) * 0xa5;
                memcpy(dst, src, 0x78);
                *(int*)(dst + 0x78) = *(int*)(src + 0x78);
                *(int*)(dst + 0x7c) = *(int*)(src + 0x7c);
                *(int*)(dst + 0x88) = *(int*)(src + 0x88);
                memcpy(dst + 0x84, src + 0x84, 0x21);
            }
            gs->SendToServer((char*)&reply, 0x688);
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnDBLoadRequestGuildBoardOpen() Exception Break",
                  0xf98, 0xf9d);
}
void CPacketTranslater::OnPcRoomPlayTimeReward(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_PcRoom_PlayTime_Reward* pkt =
            (Packet_DBMW_PcRoom_PlayTime_Reward*)header;
        unsigned char serverInfo = pkt->m_serverInfo;
        int accId = pkt->m_accId;
        int characNo = pkt->m_characNo;
        CMyFileLog log1("OnPcRoomPlayTimeReward", 0x141b);
        log1("./log/event",
             "CPacketTranslater::OnPcRoomPlayTimeReward Noti acc_id(%d), charac_no(%d), server_info(%d)",
             accId, characNo, serverInfo);
        unsigned int pinNo = 0;
        char pinBuf[0x15];
        memset(pinBuf, 0, 0x15);
        if (!m_pclApp->m_dbManager.updateNexonPinPcRoomPlayTimeEvent(
                serverInfo, accId, pinNo, pinBuf, 0x15))
        {
            CMyFileLog log("OnPcRoomPlayTimeReward", 0x1423);
            log("./log/event",
                "CPacketTranslater::OnPcRoomPlayTimeReward Error acc_id(%d), charac_no(%d), server_info(%d)",
                accId, characNo, serverInfo);
            return;
        }
        time_t now = time(0);
        tm* t = localtime(&now);
        t->tm_mday += 1;
        t->tm_hour = 0;
        t->tm_min = 0;
        time_t tomorrow = mktime(t);
        char str[0x100];
        memset(str, 0, 0x100);
        sprintf(str,
                "\xbf\xa9\xb8\xa7\xb9\xe6\xc7\xd0\x20\xb1\xe2\xb0\xa3\xbf\xa1\x20\x31\xbd\xc3\xb0\xa3\xb5\xbf\xbe\xc8\x20\x50\x43\xb9\xe6\xbf\xa1\xbc\xad\x20\xc1\xa2\xbc\xd3\xc0\xbb\x20\xc0\xaf\xc1\xf6\xc7\xcf\xbd\xc5\x20\x50\x43\xb9\xe6\x20\xc0\xaf\xc0\xfa\x20\xbf\xa9\xb7\xaf\xba\xd0\xb2\xb2\x20\xc6\xaf\xba\xb0\xc7\xd1\x20\xbc\xb1\xb9\xb0\xc0\xbb\x20\xb5\xe5\xb8\xb3\xb4\xcf\xb4\xd9\x2e\x0a\x20\x2d\x20\xb3\xd8\xbd\xbc\xc7\xc9\x3a\x20\x25\x73\x20",
                pinBuf);
        str[0x68] = 0x1e;
        str[0x7d] = 0x1f;
        int letterNo = 0;
        if (!m_pclApp->m_dbManager.InsertLetter(
                characNo, 0,
                "\xb4\xf8\xc6\xc4\x50\x43\xb9\xe6\x20\xbd\xe6\xb8\xd3\xc5\xb8\xc0\xd3",
                str, letterNo, tomorrow))
        {
            CMyFileLog log("OnPcRoomPlayTimeReward", 0x143d);
            log("./log/event",
                "CPacketTranslater::OnPcRoomPlayTimeReward Letter Error acc_id(%d), charac_no(%d), server_info(%d), pin_num(%d)",
                accId, characNo, serverInfo, pinNo);
            return;
        }
        Packet_Monitor_Notify_New_Mail mail;
        *(int*)((char*)&mail + 0xf) = characNo;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&mail, mail.packetSize);
        CMyFileLog log2("OnPcRoomPlayTimeReward", 0x1447);
        log2("./log/event",
             "CPacketTranslater::OnPcRoomPlayTimeReward reward acc_id(%d), charac_no(%d), server_info(%d), pin_num(%d), letter_id(%d)",
             accId, characNo, serverInfo, pinNo, letterNo);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnPcRoomPlayTimeReward Exception Break",
                  0x144c, 0x1451);
}
void CPacketTranslater::OnAddBuddy(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Add_Buddy* pkt = (Packet_DBMW_Add_Buddy*)header;
        Packet_DBMW_Add_Buddy_Reply reply;
        *(int*)((char*)&reply + 0xa) = pkt->m_mid;
        int result = 0;
        m_pclApp->m_dbManager.AddBuddy(pkt->m_mid, pkt->m_name,
                                       *(STBuddyDBInfo*)((char*)&reply + 0xe),
                                       result);
        *(char*)((char*)&reply + 0x35) = (char)result;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&reply, reply.packetSize);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnAddBuddy() Exception Break",
                  0x2f5, 0x2fa);
}
char CPacketTranslater::OnRequestIPCounterList(PacketHeader* header)
{
    std::vector<st_ip_counter_list> vec1;
    std::vector<st_full_ip_counter_list> vec2;
    if (!m_pclApp)
        return 0;
    try
    {
        Packet_DBMW_Request_IPCounter_List* pkt =
            (Packet_DBMW_Request_IPCounter_List*)header;
        if (!m_pclApp->m_dbManager.QueryIPCounter(
                pkt->m_serverGroup, vec1, vec2))
        {
            CMyFileLog log("OnRequestIPCounterList", 0xb8a);
            log("./log/Secu",
                "CPacketTranslater::OnRequestIPCounterList insert fail\n");
            return 0;
        }
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        if (!vec1.empty())
        {
            int size = vec1.size();
            int srcIdx = 0;
            int batch = 0;
            while (srcIdx < size)
            {
                Packet_Response_IPCounterList reply;
                int count = 0;
                while (count <= 0x95 && srcIdx < size)
                {
                    (*(st_ip_counter_list*)((char*)&reply + 0xc + count * 0x14))
                        .CopyStruct(vec1[srcIdx]);
                    srcIdx++;
                    count++;
                }
                *(char*)((char*)&reply + 0xb) = (char)count;
                if (batch == 0)
                    *(char*)((char*)&reply + 0xa) = 0;
                else if (srcIdx >= size)
                    *(char*)((char*)&reply + 0xa) = 2;
                else
                    *(char*)((char*)&reply + 0xa) = 1;
                unsigned short sendSize =
                    (unsigned short)(0xbc4 - (0x96 - count) * 0x14);
                ms->SendToServer((char*)&reply, sendSize);
                CMyFileLog log("OnRequestIPCounterList", 0xbab);
                log("./log/Secu",
                    "[IP Counter] Packet Send - Stats : %3d, Cnt : %3d",
                    batch, count);
                batch++;
            }
        }
        else
        {
            Packet_Response_IPCounterList reply;
            ms->SendToServer((char*)&reply, 0xc);
        }
        if (!vec2.empty() && pkt->m_fieldB != 0)
        {
            int size = vec2.size();
            int srcIdx = 0;
            int batch = 0;
            while (srcIdx < size)
            {
                Packet_Response_D_IPCounterList reply;
                int count = 0;
                while (count <= 0x95 && srcIdx < size)
                {
                    (*(st_full_ip_counter_list*)((char*)&reply + 0xc +
                                                 count * 0x18))
                        .CopyStruct(vec2[srcIdx]);
                    srcIdx++;
                    count++;
                }
                *(char*)((char*)&reply + 0xb) = (char)count;
                if (batch == 0)
                    *(char*)((char*)&reply + 0xa) = 0;
                else if (srcIdx >= size)
                    *(char*)((char*)&reply + 0xa) = 2;
                else
                    *(char*)((char*)&reply + 0xa) = 1;
                unsigned short sendSize =
                    (unsigned short)(0xe1c - (0x96 - count) * 0x18);
                ms->SendToServer((char*)&reply, sendSize);
                CMyFileLog log("OnRequestIPCounterList", 0xbd8);
                log("./log/Secu",
                    "[D_IP Counter] Packet Send - Stats : %3d, Cnt : %3d",
                    batch, count);
                batch++;
            }
        }
        else
        {
            Packet_Response_D_IPCounterList reply;
            ms->SendToServer((char*)&reply, 0xc);
        }
        return 1;
    }
    DNF_CATCH_LOG_RET0("./log/Except",
                       "CPacketTranslater::OnRequestIPCounterList Exception Break",
                       0xbe9, 0xbee);
}
void CPacketTranslater::onQueryTowerFullRank(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Query_Tower_Full_Rank* pkt =
            (Packet_DBMW_Query_Tower_Full_Rank*)header;
        std::vector<stTowerRank_t> ranks;
        if (!m_pclApp->m_dbManager.queryTowerFullRank(
                pkt->m_towerIndex, ranks, pkt->m_limit))
            return;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        Packet_Reply_Load_Tower_Full_Rank reply;
        *(char*)((char*)&reply + 0xa) = 1;
        if (*(unsigned int*)(pkt + 0xe) != 0x2710)
            *(char*)((char*)&reply + 0xa) = 0;
        CMyFileLog log1("onQueryTowerFullRank", 0x59b);
        log1("./log/DeathTower", "(tower_idx:%d)(rank count:%d)\n",
             *(unsigned int*)(pkt + 0xa), ranks.size());
        int i = 0;
        for (std::vector<stTowerRank_t>::const_iterator it = ranks.begin();
             it != ranks.end(); ++it)
        {
            memcpy((char*)&reply + 0x13 + i * 0x65, &*it, 0x65);
            i++;
            if (i > 0x3b)
            {
                *(char*)((char*)&reply + 0xb) = (char)i;
                ms->SendToServer((char*)&reply, 0x17bf);
                CMyFileLog log("onQueryTowerFullRank", 0x5a8);
                log("./log/DeathTower", "(tower_idx:%d)(send count:%d)\n",
                     *(unsigned int*)(pkt + 0xa), i);
                i = 0;
                *(char*)((char*)&reply + 0xa) = 0;
                DNFFLib::Sleep_Ext(0, 0x30d40);
            }
        }
        if (i != 0)
        {
            *(char*)((char*)&reply + 0xb) = (char)i;
            unsigned short sendSize = (unsigned short)(0x13 + i * 0x65);
            ms->SendToServer((char*)&reply, sendSize);
            CMyFileLog log("onQueryTowerFullRank", 0x5b3);
            log("./log/DeathTower", "(tower_idx:%d)(send count:%d)\n",
                 *(unsigned int*)(pkt + 0xa), i);
            i = 0;
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::onQueryTowerFullRank() Exception Break",
                  0x5b9, 0x5be);
}
void CPacketTranslater::OnRequestARSInfo(PacketHeader* header)
{
    std::vector<st_ars_info_list> list;
    if (!m_pclApp)
        return;
    try
    {
        if (!m_pclApp->m_dbManager.QueryLoadARSInfo(list))
        {
            CMyFileLog log("OnRequestARSInfo", 0xdf6);
            log("./log/Secu", "CPacketTranslater::OnRequestARSInfo Select fail\n");
            return;
        }
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        if (!list.empty())
        {
            int size = list.size();
            int srcIdx = 0;
            int batch = 0;
            while (srcIdx < size)
            {
                Packet_Set_ARS_Info reply;
                int count = 0;
                while (count <= 0x63 && srcIdx < size)
                {
                    (*(st_ars_info_list*)((char*)&reply + 0xf + count * 0xc))
                        .CopyStruct(list[srcIdx]);
                    srcIdx++;
                    count++;
                }
                *(char*)((char*)&reply + 0xa) = (char)count;
                if (batch == 0)
                    *(char*)((char*)&reply + 0xe) = 0;
                else if (srcIdx >= size)
                    *(char*)((char*)&reply + 0xe) = 2;
                else
                    *(char*)((char*)&reply + 0xe) = 1;
                unsigned short sendSize =
                    (unsigned short)(0x4bf - (0x64 - count) * 0xc);
                ms->SendToServer((char*)&reply, sendSize);
                CMyFileLog log("OnRequestARSInfo", 0xe1a);
                log("./log/Secu",
                    "[ARS_INFO] Packet Send - Stats : %3d, Cnt : %3d",
                    batch, count);
                batch++;
            }
        }
        else
        {
            Packet_Set_ARS_Info reply;
            *(char*)((char*)&reply + 0xa) = 0;
            *(char*)((char*)&reply + 0xe) = 0;
            ms->SendToServer((char*)&reply, 0x4bf);
            CMyFileLog log("OnRequestARSInfo", 0xe2d);
            log("./log/Secu", "[ARS_INFO] Packet Send - Stats : %3d, Cnt : 0",
                0);
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnRequestARSInfo Exception Break",
                  0xe32, 0xe37);
}
void CPacketTranslater::OnQueryGuild(PacketHeader* header)
{
    Packet_DB_Reply_Query_Guild reply;
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Query_Guild* pkt = (Packet_DBMW_Query_Guild*)header;
        unsigned int guildId = pkt->m_guildId;
        unsigned int serverGroup = pkt->m_serverGroup;
        if (!m_pclApp->m_dbManager.QueryGuild(
                pkt->m_serverId, guildId, reply))
        {
            CMyFileLog log("OnQueryGuild", 0x56);
            log("./log/Except",
                "CPacketTranslater::OnQueryGuild()\tGuild Id : %d,\t Query Result : %d\n",
                guildId, *(char*)((char*)&reply + 0xa));
            return;
        }
        if (!m_pclApp->m_dbManager.QuerySubGuildMaster(
                pkt->m_serverId, guildId, reply))
        {
            CMyFileLog log("OnQueryGuild", 0x5b);
            log("./log/Except",
                "CPacketTranslater::OnQueryGuild()\tGuild Id : %d,\t Query Result : %d\n",
                guildId, *(char*)((char*)&reply + 0xa));
            return;
        }
        if (!m_pclApp->m_dbManager.QueryGuildSkill(
                *(unsigned char*)(pkt + 0xa), guildId, reply))
        {
            CMyFileLog log("OnQueryGuild", 0x60);
            log("./log/Except",
                "CPacketTranslater::OnQueryGuild()\tGuild Id : %d,\t Query Result : %d\n",
                guildId, *(char*)((char*)&reply + 0xa));
            return;
        }
        if (!m_pclApp->m_dbManager.QueryGuildNotiMessage(
                *(unsigned char*)(pkt + 0xa), guildId, reply))
        {
            CMyFileLog log("OnQueryGuild", 0x65);
            log("./log/Except",
                "CPacketTranslater::OnQueryGuild()\tGuild Id : %d,\t Query Result : %d\n",
                guildId, *(char*)((char*)&reply + 0xa));
            return;
        }
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, 0x135);
    }
    DNF_CATCH_LOG_PRINTF("./log/Except",
                         "CPacketTranslater::OnQueryGuild() Exception Break",
                         0x74, 0x7a);
}
void CPacketTranslater::OnReqOntimeEventIdx(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(
            "CPacketTranslater::OnReqOntimeEventIdx() : 0 == m_pclApp");
    try
    {
        Packet_Result_OnTimeEvent_Idx reply;
        m_pclApp->m_dbManager.QueryOnTimeEventIdx(reply);
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* pkt = tcp->makePacketHeader(0x2341, 0xf);
            memcpy(pkt + 0xa, (char*)&reply + 0xa, 5);
            tcp->SendToServer(pkt);
        }
        else
        {
            CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
            ms->SendToServer((char*)&reply, 0xf);
            CMyFileLog log("OnReqOntimeEventIdx", 0x1098);
            log("./log/TcpServer", "OnReqOntimeEventIdx:GetTcpServer null");
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryTodayGuildMemeber() Exception Break",
                  0x109d, 0x10a2);
}
void CPacketTranslater::OnReqOntimeEventIdxUpdate(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(
            "CPacketTranslater::OnReqOntimeEventIdxUpdate() : 0 == m_pclApp");
    try
    {
        Packet_Req_Ontime_Event_Idx_Update* pktIn =
            (Packet_Req_Ontime_Event_Idx_Update*)header;
        m_pclApp->m_dbManager.QueryOnTimeEventIdxUpdate(
            pktIn);
        Packet_Result_Ontime_Event_Idx_Update reply;
        *(int*)((char*)&reply + 0xa) = pktIn->m_no;
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* pkt = tcp->makePacketHeader(0x2348, 0xe);
            memcpy(pkt + 0xa, (char*)&reply + 0xa, 4);
            tcp->SendToServer(pkt);
        }
        else
        {
            CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
            ms->SendToServer((char*)&reply, 0xf);
            CMyFileLog log("OnReqOntimeEventIdxUpdate", 0x114c);
            log("./log/TcpServer", "OnReqOntimeEventItem:GetTcpServer null");
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryTodayGuildMemeber() Exception Break",
                  0x1151, 0x1156);
}
void CPacketTranslater::OnReqOntimeEventItem(PacketHeader* header)
{
    if (!m_pclApp)
        throw CDNFException(
            "CPacketTranslater::OnReqOntimeEventItem() : 0 == m_pclApp");
    try
    {
        Packet_Result_Ontime_Event_Item reply;
        m_pclApp->m_dbManager.QueryOnTimeEventItem(reply);
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* pkt = tcp->makePacketHeader(0x2346, 0x14);
            memcpy(pkt + 0xa, (char*)&reply + 0xa, 0xa);
            tcp->SendToServer(pkt);
        }
        else
        {
            CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
            ms->SendToServer((char*)&reply, 0xf);
            CMyFileLog log("OnReqOntimeEventItem", 0x1118);
            log("./log/TcpServer", "OnReqOntimeEventItem:GetTcpServer null");
        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryTodayGuildMemeber() Exception Break",
                  0x111d, 0x1122);
}
void CPacketTranslater::OnQueryBuddyInfo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Query_Buddy* pkt = (Packet_DBMW_Query_Buddy*)header;
        Packet_DBMW_Query_Buddy_Info_Reply reply;
        *(int*)((char*)&reply + 0xa) = pkt->m_characNo;
        if (!m_pclApp->m_dbManager.QueryBuddyInfo(
                pkt->m_characNo,
                (STBuddyDBInfo*)((char*)&reply + 0xf),
                *(unsigned char*)((char*)&reply + 0xe)))
            return;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&reply, reply.packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryBuddyInfo() Exception Break",
                  0x33c, 0x341);
}
void CPacketTranslater::OnQueryUnconnGuildMemberProxy(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        Packet_DB_Reply_Unconn_Guild_Member reply;
        Packet_DB_Call_Unconn_Guild_Member* pkt =
            (Packet_DB_Call_Unconn_Guild_Member*)header;
        reply.m_fieldA = pkt->m_fieldA;
        reply.m_fieldE = pkt->m_fieldE;
        if (!m_pclApp->m_dbManager.QueryGuildMemberProxy(
                reply.m_fieldA, reply.m_fieldE, reply.m_proxy))
        {
            CMyFileLog log("OnQueryUnconnGuildMemberProxy", 0x3ae);
            log("./log/GuildMemberErr",
                "Query Guild Member List Error g(%d), c(%d)\n",
                reply.m_fieldA, reply.m_fieldE);
            return;
        }
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, reply.packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryGuildAllMembersProxy() Exception Break",
                  0x3b7, 0x3bc);
}
void CPacketTranslater::OnQueryGuildAllMembersProxy(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        Packet_DB_Reply_Guild_All_Members reply;
        Packet_DB_Call_Guild_All_Members* pkt =
            (Packet_DB_Call_Guild_All_Members*)header;
        reply.m_fieldA = pkt->m_fieldA;
        reply.m_fieldE = pkt->m_fieldE;
        unsigned short tot = 0;
        STGuildMemberProxy* temp = m_pclApp->m_guildManager->GetArrayTempGuildMemberList();
        memset(temp, 0, 0x4c2c);
        if (!m_pclApp->m_dbManager.QueryGuildAllMembersProxy(reply.m_fieldA, temp, tot))
        {
            CMyFileLog log("OnQueryGuildAllMembersProxy", 0x362);
            log("./log/GuildMemberErr",
                "Query All Guild Member List Error g(%d), c(%d)\n",
                reply.m_fieldA, reply.m_fieldE);
            return;
        }
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        int pages = tot / 0x5d;
        int remain = tot % 0x5d;
        int i = 0;
        for (; i < pages; i++)
        {
            if (i == 0)
                reply.m_flag = 0;
            else
                reply.m_flag = 1;
            if (remain == 0 && (i + 1) * 0x5d == (int)tot)
                reply.m_flag = 2;
            reply.m_count = 0x5d;
            memcpy((char*)&reply + 0x14, (char*)temp + i * 0x179d, 0x179d);
            gs->SendToServer((char*)&reply, reply.packetSize);
        }
        if (remain != 0)
        {
            reply.m_flag = 2;
            reply.m_count = (unsigned char)remain;
            memcpy((char*)&reply + 0x14, (char*)temp + i * 0x179d,
                   remain * 0x41);
            *(unsigned short*)((char*)&reply + 2) =
                (unsigned short)(remain * 0x41 + 0x14);
            gs->SendToServer((char*)&reply, reply.packetSize);
        }
        CMyFileLog log("OnQueryGuildAllMembersProxy", 0x38b);
        log("./log/GuildModify",
            "Query All Guild Member List g(%d), c(%d), tot(%d)\n",
            reply.m_fieldA, reply.m_fieldE, tot);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryGuildAllMembersProxy() Exception Break",
                  0x38f, 0x394);
}
void CPacketTranslater::attach(CApplication* app)
{
    m_pclApp = app;
}
void CPacketTranslater::OnHeartBeat(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    CServerHandler* handler = m_pclApp->m_serverHandler;
    if (!handler)
        return;
    unsigned char idx = ((char*)header)[0xa];
    if (idx > 0x64)
        throw CDNFException(
            "CPacketTranslater::OnHeartBeat() \xc3\xa4\xb3\xce \xc0\xce\xb5\xa6\xbd\xba \xbf\xc0\xb7\xf9\n");
    handler->ResetHeartBeat(idx);
    if (!handler->IsConnectedMonitorServer(idx))
    {
        handler->SetConnectFlag(idx, 1);
        Packet_Monitor_Manager_Connect_OK pkt;
        handler->SendToTcpServer(&pkt, idx);
        printf("First Heart Beat Arrived From %d Group Monitor!\n", idx);
        CMyFileLog log("OnHeartBeat", 0x43);
        log("./log/Monitor", "First Heart Beat Arrived From %d Group Monitor!", idx);
    }
}
void CPacketTranslater::OnEventStart(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnEventStart : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SendAllTcpServer(header);
        CMyFileLog log("OnEventStart", 0x70);
        log("./log/Web",
            "CPacketTranslater::OnEventStart() eventCode(%d), eventParam1(%d), eventParam2(%d)\n",
            *(int*)((char*)header + 0xa),
            *(unsigned short*)((char*)header + 0xe),
            *(unsigned short*)((char*)header + 0x10));
    }
    DNF_CATCH_LOG_THROW("CPacketTranslater::OnEventStart() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
}
void CPacketTranslater::OnEventEnd(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnEventEnd : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SendAllTcpServer(header);
        CMyFileLog log("OnEventEnd", 0x92);
        log("./log/Web", "CPacketTranslater::OnEventEnd() eventCode(%d)\n",
            *(int*)((char*)header + 0xa));
    }
    DNF_CATCH_LOG_THROW("CPacketTranslater::OnEventEnd() \xbf\xb9\xbf\xdc \xb9\xdf\xbb\xfd");
}
void CPacketTranslater::OnCommonPacket(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnCommonPacket : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SendAllTcpServer(header);
        CMyFileLog log("OnCommonPacket", 0xb5);
        log("./log/Web", "CPacketTranslater::OnCommonPacket() packet_id(%d)\n", header->packetId);
    }
    DNF_CATCH_LOG_THROW("CPacketTranslater::OnEventEnd() Exception Break");
}
void CPacketTranslater::OnInnerPacketLogin(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
        {
            CMyFileLog log("OnInnerPacketLogin", 0xbfa);
            log("./log/Except", "CPacketTranslater::OnInnerPacketLogin : 0 == m_pclApp");
            return;
        }
        CMyFileLog log("OnInnerPacketLogin", 0xc00);
        log("./log/TcpServer", "CPacketTranslater::OnInnerPacketLogin (sock:%d)",
            (int)((Packet_InnerPakcet_Login*)header)->reversed2);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnInnerPacketLogin Exception Break",
                  0xc04, 0xc09);
}
void CPacketTranslater::OnInnerPacketLogout(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
        {
            CMyFileLog log("OnInnerPacketLogout", 0xc14);
            log("./log/Except", "CPacketTranslater::OnInnerPacketLogout : 0 == m_pclApp");
            return;
        }
        int port = (int)((Packet_InnerPakcet_Logout*)header)->reversed2;
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        CTcpServer* server = handler->GetTcpServer((unsigned int)port);
        if (!server)
        {
            CMyFileLog log("OnInnerPacketLogout", 0xc1f);
            log("./log/TcpServer", "CPacketTranslater::OnInnerPacketLogout Invalid Server Instance(sock:%d)",
                port);
            return;
        }
        unsigned char idx = server->m_index;
        handler = m_pclApp->Get_ServerHandler();
        if (!handler->DeleteTcpServer(idx))
        {
            CMyFileLog log("OnInnerPacketLogout", 0xc27);
            log("./log/TcpServer", "CPacketTranslater::OnInnerPacketLogout DeleteTcpServer fail(sock:%d)",
                port);
            return;
        }
        CMyFileLog log("OnInnerPacketLogout", 0xc2b);
        log("./log/TcpServer",
            "CPacketTranslater::OnInnerPacketLogout DeleteTcpServer Success(TYPE:%d, sock:%d)",
            idx, port);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnInnerPacketLogout Exception Break",
                  0xc2f, 0xc34);
}
void CPacketTranslater::OnTcpServerLogin(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        Packet_Tcp_Server_Login* pkt = (Packet_Tcp_Server_Login*)header;
        int port = (int)pkt->reversed2;
        unsigned char idx = pkt->m_idx;
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        if (handler->GetTcpServer(idx))
        {
            CMyFileLog log("OnTcpServerLogin", 0x239);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogin Duplicate Server Instance(TYPE:%d, sock:%d)",
                idx, port);
            return;
        }
        handler = m_pclApp->Get_ServerHandler();
        if (!handler->CreateTcpServer(idx, port))
        {
            CMyFileLog log("OnTcpServerLogin", 0x242);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogin CreateTcpServer fail(TYPE:%d, sock:%d)\n",
                idx, port);
            return;
        }
        printf("CPacketTranslater::OnTcpServerLogin(TYPE:%d, sock:%d)", idx, port);
        CMyFileLog log("OnTcpServerLogin", 0x250);
        log("./log/TcpServer", "CPacketTranslater::OnTcpServerLogin(TYPE:%d, sock:%d)", idx, port);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnTcpServerLogin Exception Break",
                  0x254, 0x259);
}
void CPacketTranslater::OnTcpServerLogout(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        Packet_Tcp_Server_Logout* pkt = (Packet_Tcp_Server_Logout*)header;
        unsigned char idx = pkt->m_idx;
        int port = (int)pkt->reversed2;
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        if (!handler->GetTcpServer(idx))
        {
            CMyFileLog log("OnTcpServerLogout", 0xc74);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogout Invalid Server Instance(TYPE:%d, sock:%d)",
                idx, port);
            return;
        }
        if (!handler->DeleteTcpServer(idx))
        {
            CMyFileLog log("OnTcpServerLogout", 0xc7a);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogout DeleteTcpServer fail(TYPE:%d, sock:%d)",
                idx, port);
            return;
        }
        printf("CPacketTranslater::OnTcpServerLogout(TYPE:%d, sock:%d)", idx, port);
        CMyFileLog log("OnTcpServerLogout", 0xc7e);
        log("./log/TcpServer", "CPacketTranslater::OnTcpServerLogout(TYPE:%d, sock:%d)", idx, port);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnTcpServerLogout Exception Break",
                  0xc82, 0xc87);
}
void CPacketTranslater::OnTcpServerHeartbeat(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        Packet_Tcp_Server_Heartbeat* pkt =
            (Packet_Tcp_Server_Heartbeat*)header;
        unsigned char idx = pkt->m_idx;
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        CTcpServer* server = handler->GetTcpServer(idx);
        if (!server)
        {
            CMyFileLog log("OnTcpServerHeartbeat", 0xc98);
            log("./log/TcpServer",
                "CPacketTranslater::OnTcpServerHeartbeat Invalid Server Instance(TYPE:%d, sock:%d)",
                idx, (int)pkt->reversed2);
            return;
        }
        server->NotifyHeartbeat();
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnTcpServerHeartbeat Exception Break",
                  0xca4, 0xca9);
}
void CPacketTranslater::OnWebNoticeInGameAD(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        Packet_Web_Notice_InGame_Advertisement pkt;
        m_pclApp->m_serverHandler->SendAllTcpServer(&pkt);
        CMyFileLog log("OnWebNoticeInGameAD", 0x2ae);
        log("./log/Web", "OnWebNoticeInGameAD() packet_id(%d)\n", header->packetId);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnWebNoticeInGameAD Exception Break",
                  0x2b2, 0x2b7);
}
void CPacketTranslater::OnWebNoticeBroadcast(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnWebNoticeBroadcast : 0 == m_pclApp");
        int len = ((char*)header)[0x10a];
        CMyFileLog log("OnWebNoticeBroadcast", 0x1b2);
        log("./log/test", "%d, %s, %d, %s\n",
            ((char*)header)[0xa], (char*)header + 0xb, len, (char*)header + 0x10b);
        if (m_pclApp && m_pclApp->m_serverHandler)
        {
            std::vector<std::string> parts;
            std::string s((char*)header + 0xb, len);
            parse_string(parts, s, ',');
            for (std::vector<std::string>::iterator it = parts.begin();
                 it != parts.end(); ++it)
            {
                std::string tok = *it;
                int ch = atoi(tok.c_str());
                if (ch <= 0x64)
                {
                    Packet_Web_Notice_Single pkt;
                    ((char*)&pkt)[0xa] = len;
                    memset((char*)&pkt + 0xb, 0, 0xff);
                    strncpy((char*)&pkt + 0xb, (char*)header + 0x10b, len);
                    m_pclApp->m_serverHandler->SendToTcpServer((char*)&pkt, 0x10a, ch);
                }
            }
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnWebNoticeBroadcast() Exception Break",
                  0x1e0, 0x1e5);
}
void CPacketTranslater::OnMemberDeleteAsCharDelete(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnMemberDeleteAsCharDelete(
            ((Packet_DB_Request_Member_Delete*)header)->m_characNo);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnMemberDeleteAsCharDelete() Exception Break",
                  0x189, 0x18f);
}
void CPacketTranslater::OnSendHWspec(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryHWspecCreate(
            (Packet_DBMW_Save_Client_Spec_Statistic*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSendHWspec() Exception Break",
                  0x6fe, 0x703);
}
void CPacketTranslater::OnSaveUnchangableGuildInfo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.SaveUnchangableGuildInfo(
            (Packet_UnChangable_GuildInfo_Save*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestGuildCreate() Exception Break",
                  0x6c6, 0x6cb);
}
void CPacketTranslater::OnErrorLineSave(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryErrorLineStatisticCreate(
            (Packet_DBMW_Save_Error_Line_Statistic*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnErrorLineSave() Exception Break",
                  0x716, 0x71b);
}
void CPacketTranslater::OnPartyStatisticSave(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryPartyStatisticCreate(
            (Packet_DBMW_Dungeon_Statistic_Party*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnPartyStatisticSave() Exception Break",
                  0x72e, 0x733);
}
void CPacketTranslater::OnPartyJobStatisticSave(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryPartyJobStatisticCreate(
            (Packet_DBMW_Dungeon_Statistic_Party_Job*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnPartyJobStatisticSave() Exception Break",
                  0x747, 0x74c);
}
void CPacketTranslater::OnPartyCharacStatisticSave(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryPartyCharacStatisticCreate(
            (Packet_DBMW_Dungeon_Statistic_Party_Charac*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnPartyCharacStatisticSave() Exception Break",
                  0x760, 0x765);
}
void CPacketTranslater::OnDeathTowerValueStatisticSave(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryDeathTowerValueStatisticCreate(
            (Packet_DBMW_DeathTower_Statistic_Value*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDeathTowerValueStatisticSave() Exception Break",
                  0x779, 0x77e);
}
void CPacketTranslater::OnDeathTowerPlayDataJobStatisticSave(
    PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryDeathTowerPlayDataJobStatisticCreate(
            (Packet_DBMW_DeathTower_Statistic_Playdata_Job*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDeathTowerPlayDataJobStatisticSave() Exception Break",
                  0x792, 0x797);
}
void CPacketTranslater::OnDeathTowerPlayDataPartyStatisticSave(
    PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryDeathTowerPlayDataPartyStatisticCreate(
            (Packet_DBMW_DeathTower_Statistic_Playdata_Party*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDeathTowerPlayDataPartyStatisticSave() Exception Break",
                  0x7ab, 0x7b0);
}
void CPacketTranslater::OnHellPartyStatisticItemSave(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryHellPartyStatisticItemCreate(
            (Packet_DBMW_HellParty_Statistic_Item*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnHellPartyStatisticItemSave() Exception Break",
                  0x7c3, 0x7c8);
}
void CPacketTranslater::OnCubeStatisticDataSave(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.QueryCubeStatisticCreate(
            (Packet_DBMW_Cube_Statistic*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnCubeStatisticDataSave() Exception Break",
                  0x7da, 0x7df);
}
void CPacketTranslater::OnQueryFirstLoadSpecDb(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    CStatisticsServer* stats =
        m_pclApp->m_serverHandler->GetStatisticsServerPtr();
    try
    {
        m_pclApp->m_dbManager.QueryFirstLoadSpecDb(
            (Packet_Frame_Lag_Statistic_Load_Spec*)header, stats);
    }
    DNF_CATCH_LOG2("./log/Except.log",
                   "CPacketTranslater::OnQueryFirstLoadSpecDb Exception Break",
                   "CPacketTranslater::OnQueryFirstLoadSpecDb() Exception Break",
                   0x7f5, 0x7fa);
}
void CPacketTranslater::OnQueryReloadSpecDb(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    CStatisticsServer* stats =
        m_pclApp->m_serverHandler->GetStatisticsServerPtr();
    try
    {
        m_pclApp->m_dbManager.QueryReloadSpecDb(
            (Packet_Frame_Lag_Statistic_Reload_Spec*)header, stats);
    }
    DNF_CATCH_LOG2("./log/Except.log",
                   "CPacketTranslater::OnQueryReloadSpecDb Exception Break",
                   "CPacketTranslater::OnQueryReloadSpecDb() Exception Break",
                   0x80b, 0x810);
}
void CPacketTranslater::OnInsertFrameLagStatistics(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    CStatisticsServer* stats =
        m_pclApp->m_serverHandler->GetStatisticsServerPtr();
    try
    {
        m_pclApp->m_dbManager.InsertFrameLagStatistics(
            (Packet_Frame_Lag_Statistic_Write_Lag_Index*)header, stats);
    }
    DNF_CATCH_LOG2("./log/Except.log",
                   "CPacketTranslater::OnInsertFrameLagStatistics Exception Break",
                   "CPacketTranslater::OnInsertFrameLagStatistics() Exception Break",
                   0x821, 0x826);
}
void CPacketTranslater::OnWriteQueryStatistics(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.SunAhWriteQuery(
            (Packet_Frame_Lag_Statistic_Write_Query*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnInsertDirectxVersiongStatistics() Exception Break",
                  0x838, 0x83d);
}
void CPacketTranslater::OnInsertDailyBadSpecStatistics(PacketHeader* header)
{
}
void CPacketTranslater::OnInsertUsedMemoryStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSaveUsedMemoryWriteQuery(
            (Packet_Frame_Lag_Used_Memory_Write_Query*)header);
    }
    DNF_CATCH_LOG2("./log/Except",
                   "CPacketTranslater::OnInsertUsedMemoryStatistic Exception Break",
                   "CPacketTranslater::OnInsertUsedMemoryStatistic() Exception Break",
                   0x84f, 0x854);
}
void CPacketTranslater::OnSaveAssertManagerInfo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSaveAssertManagerInfoWrite(
            (Packet_DBMW_Assert_Manager_Info_Write_Query*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnSaveAssertManagerInfo() Exception Break",
                  0x998, 0x99d);
}
void CPacketTranslater::OnSavePacketOverflowStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSavePacketOverflowWrite(
            (Packet_DBMW_Packet_Overflow_Statistic*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnSavePacketOverflowStatistic() Exception Break",
                  0x9af, 0x9b4);
}
void CPacketTranslater::OnWriteGuildMemberMemo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnWriteGuildMemberMemo(
            (Packet_DB_Write_Guild_Member_Memo*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnWriteGuildMemberMemo() Exception Break",
                  0x9c6, 0x9cb);
}
void CPacketTranslater::OnWriteUserTingTimeCheck(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSaveUserTingTimeCheckWrite(
            (Packet_DBMW_User_Ting_TimeCheck_Write_Query*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnWriteUserTingTimeCheck() Exception Break",
                  0x9dd, 0x9e2);
}
void CPacketTranslater::OnTingUserCollect(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSaveTingUserAccount(
            (Packet_DBMW_Ting_User_TimeCheck_Write_Query*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnTingUserCollect() Exception Break",
                  0x9f4, 0x9f9);
}
void CPacketTranslater::OnRecvPowerwarLoadingReport(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSavePowerwarLoadingReport(
            (Packet_DBMW_Powerwar_Loading_Time_Report*)header);
    }
    DNF_CATCH_LOG_NN("./log/Except",
                     "CPacketTranslater::OnRecvPowerwarLoadingReport() Exception Break",
                     0xa1d, 0xa22);
}
void CPacketTranslater::OnRecvPowerwarLagReport(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnSavePowerwarLagReport(
            (Packet_DBMW_Powerwar_Lag_Report*)header);
    }
    DNF_CATCH_LOG_NN("./log/Except",
                     "CPacketTranslater::OnRecvPowerwarLagReport() Exception Break",
                     0xa31, 0xa36);
}
void CPacketTranslater::OnReasonCrashDownQuery(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnReasonCrashDownQueryWrite(
            (Packet_DBMW_Reason_Crash_Down_Query*)header);
    }
    DNF_CATCH_LOG_NN("./log/Except",
                     "CPacketTranslater::OnReasonCrashDownQuery() Exception Break",
                     0xcb8, 0xcbd);
}
void CPacketTranslater::OnDBMWLoginLogoutStatistics(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnStatisticNumOfOccupations(
            (Packet_DBMW_Statistic_Login_Logout*)header);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnDBMWLoginLogoutStatistics() Exception Break",
                  0xde0, 0xde5);
}
void CPacketTranslater::OnDBMWTechnicalReportCommonQuery(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnTechnicalReportCommonQuery(
            (Packet_DBMW_TechnicalReport_Common_Query*)header);
    }
    DNF_CATCH_LOG_NN("./log/Except",
                     "CPacketTranslater::OnDBMWTechnicalReportCommonQuery() Exception Break",
                     0xebd, 0xec2);
}
void CPacketTranslater::OnGoldcardEventStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        m_pclApp->m_dbManager.OnGoldcardEventStatistic(
            (Packet_Goldcard_Event_Statistic_STD*)header);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnGoldcardEventStatistic() Exception Break",
                  0xf36, 0xf3b);
}
void CPacketTranslater::OnRenew_GM_List(PacketHeader* header)
{
    WongWork::CGMAccounts* gm =
        (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
    if (gm && header)
    {
        Packet_GM_List* pkt = (Packet_GM_List*)header;
        if (pkt->m_flag == 0)
            gm->clearGmList();
        for (int i = 0; i < pkt->m_count; i++)
        {
            gm->AppendGM_Sys(pkt->m_ids[i], pkt->m_flags[i]);
        }
    }
}
void CPacketTranslater::OnUpdateChannelOccNum(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_User_Count_Statistic* pkt =
            (Packet_User_Count_Statistic*)header;
        char ok = m_pclApp->m_dbManager.QueryUpdateChannelOccNum(
            pkt);
        if (ok != 1 && header)
        {
            CMyFileLog log("OnUpdateChannelOccNum", 0x1300);
            log("./log/statistic",
                "CPacketTranslater::OnUpdateChannelOccNum Error ch_no(%d), count(%d)",
                pkt->m_gcNo, pkt->m_userCount);
        }
    }
    DNF_CATCH_LOG_CDNF("./log/Except.log",
                       "CPacketTranslater::OnUpdateChannelOccNum() Exception Break",
                       0x1305);
}
void CPacketTranslater::OnInsertUpdate(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        char ok = m_pclApp->m_dbManager.QueryInsertUpdate(
            (PacketInsertUpdate*)header);
        if (ok != 1 && header)
        {
            CMyFileLog log("OnInsertUpdate", 0x1318);
            log("./log/statistic",
                "CPacketTranslater::PacketInsertUpdate Error");
        }
    }
    DNF_CATCH_LOG_CDNF("./log/Except.log",
                       "CPacketTranslater::PacketInsertUpdate() Exception Break",
                       0x131d);
}
