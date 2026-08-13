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

// 本文件私有视图：Packet_DBMW_Collect_Items_Gm 的字段类型以调用签名
// updateCollectItemsGm(uchar,int,int,uint) 为准（布局不变）。
struct Packet_DBMW_Collect_Items_Gm_View : PacketHeader
{
    unsigned char m_serverInfo;  // +0xa
    int m_fieldB;      // +0xb
    int m_fieldF;      // +0xf
    unsigned int m_field13;  // +0x13
} __attribute__((packed));

struct Packet_DB_Load_Request_Guild_Board_Write_View : PacketHeader
{
    unsigned char m_serverId;  // +0xa
    unsigned int m_guildId;    // +0xb
    unsigned int m_fieldF;     // +0xf
    unsigned int m_field13;    // +0x13
} __attribute__((packed));

struct Packet_DB_Load_Request_Guild_Board_Delete_View : PacketHeader
{
    unsigned char m_serverId;  // +0xa
    unsigned int m_fieldB;     // +0xb
    unsigned int m_fieldF;     // +0xf
    unsigned int m_field13;    // +0x13
} __attribute__((packed));

struct Packet_DB_Request_Guild_Master_Delegate_View : PacketHeader
{
    unsigned int m_guildId;      // +0xa
    unsigned int m_oldMasterNo;  // +0xe
    unsigned char m_serverId;    // +0x12
    char m_name[0x1d];           // +0x13
} __attribute__((packed));

struct Packet_DBMW_Collect_Items_Update_View : PacketHeader
{
    int m_fieldA;              // +0xa
    unsigned char m_serverInfo; // +0xe
    char m_pad[4];             // +0xf..+0x12
    unsigned char m_field13;   // +0x13
} __attribute__((packed));

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
    : PacketHeader(0x5e1, 0x19e), m_fieldA(0)
{
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
    m_count = 0;
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
        Packet_Secret_Shop_Statistic* pkt = (Packet_Secret_Shop_Statistic*)header;
        m_pclApp->m_dbManager.OnSecretShopStatistic(pkt);
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
        LimitNpcBuyItemRequestInfo* pkt =
            (LimitNpcBuyItemRequestInfo*)header;
        if (!m_pclApp->m_dbManager.loadLimitNpcBuyItemInfo(
                pkt, &result))
        {
            DNF_LOG_SCOPE_LINE(0x132e,
                "./log/NpcBuyLimitItem",
                "CPacketTranslater::onLoadLimitNpcBuyItemInfo data load fail\n"
            );

            return;
        }
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&result, 0x176);
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
        LimitNpcBuyItemUpdate* pkt = (LimitNpcBuyItemUpdate*)header;
        if (!m_pclApp->m_dbManager.updateLimitNpcBuyItemInfo(
                pkt))
        {
            DNF_LOG_SCOPE_LINE(0x134c,
                "./log/NpcBuyLimitItem",
                "CPacketTranslater::onUpdateLimitNpcBuyItemInfo data update fail\n"
            );

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
        if (!m_pclApp->m_dbManager.ChangeGuildNotifyMessage(
                pkt->m_guildId, pkt->m_id, pkt->m_msg))
            return;
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
        Packet_DB_Request_Guild_Master_Delegate_View* pkt =
            (Packet_DB_Request_Guild_Master_Delegate_View*)header;
        if (pkt->m_guildId == 0)
            return;
        Packet_DB_Reply_Guild_Master_Delegate reply;
        reply.m_fieldA = pkt->m_guildId;
        reply.m_fieldE = pkt->m_oldMasterNo;
        memcpy((char*)&reply + 0x16, pkt->m_name, 0x1d);
        unsigned char grade = 0;
        unsigned int result = 0;
        m_pclApp->m_dbManager.QueryGuildMemberGradeByName(
            pkt->m_serverId, pkt->m_guildId, pkt->m_name,
            grade, *(unsigned int*)((char*)&reply + 0x12), result);
        int resultCode = 0;
        if (grade == 2)
        {
            if (reply.m_field12 == 0 || result == 0)
            {
                resultCode = 0x22;
            }
            else if (!m_pclApp->m_dbManager.GuildMasterDelegate(
                         (unsigned int)pkt->m_serverId,
                         pkt->m_guildId, pkt->m_oldMasterNo,
                         result, reply.m_field12,
                         pkt->m_name))
            {
                DNF_LOG_SCOPE_LINE(0x628,
                    "./log/GuildModify",
                    "OnGuildMasterDelegate Err(%d) : return false",
                    pkt->m_guildId
                );

                resultCode = 2;
            }
        }
        else
        {
            resultCode = 0x56;
        }
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
        DNF_LOG_SCOPE_LINE(0x636,
            "./log/GuildModify",
            "::OnGuildMasterDelegate g(%d) delegater(%d) delegatee(%s) r(%d)",
            pkt->m_guildId,
            pkt->m_oldMasterNo,
            pkt->m_name,
            resultCode
        );

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
        struct SendGuildLetterLocal
        {
            char m_pad[0xa];       // +0x0
            unsigned int m_fieldA;   // +0xa
            unsigned char m_serverId; // +0xe
            unsigned int m_guildId;  // +0xf
            char m_content[0x100];   // +0x13
            char m_subject[0x11];    // +0x113
            int m_flag;              // +0x124
        } __attribute__((packed));
        SendGuildLetterLocal* pkt = (SendGuildLetterLocal*)header;
        if (pkt->m_guildId == 0)
            return;
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        if (pkt->m_flag == 1)
        {
            if (!m_pclApp->m_dbManager.SendGuildCoinByMail(
                    pkt->m_serverId, pkt->m_guildId,
                    1, 0, 1, pkt->m_subject, pkt->m_content))
            {
                DNF_LOG_SCOPE_LINE(0x4da,
                    "./log/GuildModify",
                    "AwardGuildItemByMail Err(%d) : return false",
                    pkt->m_guildId
                );

                return;
            }
        }
        else if (pkt->m_flag == -1)
        {
            if (!m_pclApp->m_dbManager.SendGuildLetter(
                    pkt->m_serverId, pkt->m_guildId, pkt->m_content))
            {
                DNF_LOG_SCOPE_LINE(0x4e2,
                    "./log/GuildModify",
                    "OnSendGuildLetter Err(%d) : return false",
                    pkt->m_guildId
                );

                return;
            }
            Packet_DBMW_Reply_Guild_Mail reply;
            reply.m_fieldE = pkt->m_guildId;
            reply.m_fieldA = pkt->m_fieldA;
            reply.m_field12 = 0;
            gs->SendToServer((char*)&reply, reply.packetSize);
        }
        Packet_Notice_Guild_Mail_Arrived notice;
        notice.m_fieldA = 1;
        notice.m_guildId = pkt->m_guildId;
        gs->SendToServer((char*)&notice, notice.packetSize);
        DNF_LOG_SCOPE_LINE(0x507,
            "./log/GuildMail",
            "Guild(%d) Message(%s)",
            pkt->m_guildId,
            pkt->m_content
        );

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
        reply.m_fieldA = pkt->m_guildId;
        reply.m_fieldE = pkt->m_field13;
        reply.m_field12 = pkt->m_characNo;
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
            DNF_LOG_SCOPE_LINE(0x56a,
                "./log/GuildModify",
                "OnGuildJoin Err(g:%d,c:%d,r:%d) : return false", pkt->m_guildId, pkt->m_characNo,
                reply.m_field16
            );

            if (reply.m_field16 == 0)
                m_pclApp->m_dbManager.DeleteJoinListByInvite(
                    pkt->m_guildId, pkt->m_characNo);
            m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
                (char*)&reply, reply.packetSize);
        }
        CMyFileLog log2(__FUNCTION__, 0x576);
        log2("./log/GuildModify", "::OnGuildJoin g(%d) c(%d) r(%d)",
             pkt->m_guildId, pkt->m_characNo,
             reply.m_field16);
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
        if (pkt->m_count < 1 || pkt->m_count > 0x10)
        {
            DNF_LOG_SCOPE_LINE(0x3d4,
                "./log/GuildEvent",
                "CApplication.OnSendMailCoinGuildEvent Err : return false(%d)",
                pkt->m_guildId
            );

            return;
        }
        int count = pkt->m_count;
        int absCount = count < 0 ? -count : count;
        std::vector<int> characNos;
        characNos.clear();
        if (!m_pclApp->m_dbManager.AwardGuildCoinByMail(
                pkt->m_serverId, pkt->m_guildId, 1, 1,
                absCount, pkt->m_count, characNos))
        {
            DNF_LOG_SCOPE_LINE(0x3f5,
                "./log/GuildEvent",
                "CApplication.AwardGuildCoinByMail Err(%d) : return false",
                pkt->m_guildId
            );

        }
        else if (!characNos.empty())
        {
            Packet_Notify_New_Group_Mail notice;
            int n = characNos.size() > 0x12b ? 0x12c : characNos.size();
            for (int i = 0; i < n; i++)
                notice.m_characNos[i] = characNos.at(i);
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
        Packet_DB_Load_Request_Guild_Board_Write_View* pkt =
            (Packet_DB_Load_Request_Guild_Board_Write_View*)header;
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        STGuildBoardDBInfo info;
        Packet_DB_Load_Reply_Guild_Board_Write reply;
        if (m_pclApp->m_dbManager.OnWriteGuildBoard(
                (Packet_DB_Load_Request_Guild_Board_Write*)pkt, &info))
        {
            reply.m_fieldC = pkt->m_guildId;
            reply.m_field10 = pkt->m_field13;
            memcpy((char*)&reply + 0x14, &info, 0xa5);
            gs->SendToServer((char*)&reply, 0xb9);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0xfbc,
                "./log/Except",
                "CPacketTranslater::OnDBLoadRequestGuildBoardWrite()\tGuild Id : %d,\t Query Result : %d\n", pkt->m_guildId,
                0
            );

            reply.m_fieldA = 1;
            reply.m_fieldC = pkt->m_guildId;
            reply.m_field10 = pkt->m_field13;
            gs->SendToServer((char*)&reply, 0xb9);
        }
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
        Packet_DB_Load_Reply_Web_Guild_Board_Write reply;
        if (m_pclApp->m_dbManager.OnWriteWebGuildBoard(
                pkt, &info))
        {
            reply.m_fieldC = pkt->m_guildId;
            reply.m_field10 = pkt->m_fieldE;
            memcpy((char*)&reply + 0x14, &info, 0xa5);
            gs->SendToServer((char*)&reply, 0xb9);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x101a,
                "./log/Except",
                "CPacketTranslater::OnDBLoadRequestWebGuildBoardWrite()\tGuild Id : %d,\t Query Result : %d\n", pkt->m_guildId,
                0
            );

            reply.m_fieldA = 1;
            reply.m_fieldC = pkt->m_guildId;
            reply.m_field10 = pkt->m_fieldE;
            gs->SendToServer((char*)&reply, 0xb9);
        }
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
        Packet_DB_Load_Request_Guild_Board_Delete_View* pkt =
            (Packet_DB_Load_Request_Guild_Board_Delete_View*)header;
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        if (m_pclApp->m_dbManager.OnDeleteGuildBoard(
                pkt->m_fieldB))
        {
            Packet_DB_Load_Reply_Guild_Board_Delete reply;
            reply.m_fieldC = pkt->m_fieldF;
            reply.m_field10 = pkt->m_field13;
            reply.m_field14 = pkt->m_fieldB;
            gs->SendToServer((char*)&reply, 0x18);
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0xfe9,
                "./log/Except",
                "CPacketTranslater::OnDBLoadRequestGuildBoardDelete()\tGuild Id : %d,\t Query Result : %d\n", pkt->m_fieldF,
                0
            );

            Packet_DB_Load_Reply_Guild_Board_Delete reply;
            reply.m_fieldA = 1;
            reply.m_fieldC = pkt->m_fieldF;
            reply.m_field10 = pkt->m_field13;
            gs->SendToServer((char*)&reply, 0x18);
        }
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnDBLoadRequestGuildBoardDelete() Exception Break",
                  0xff6, 0xffb);
}
void CPacketTranslater::OnLoadGuildAgit(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnLoadGuildAgit() : 0 == m_pclApp"));
        Packet_DB_Load_Guild_Agit* pkt = (Packet_DB_Load_Guild_Agit*)header;
        Packet_Guild_Load_Guild_Agit reply;
        reply.m_fieldA = pkt->m_guildId;
        m_pclApp->m_dbManager.OnLoadGuildAgit(pkt, reply);
        Packet_Guild_Load_Guild_Cargo cargo;
        cargo.m_fieldA = pkt->m_guildId;
        m_pclApp->m_dbManager.OnLoadGuildCargo(pkt->m_guildId, cargo);
        Packet_Guild_Load_Guild_Cargo_History history;
        history.m_fieldA = pkt->m_guildId;
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
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnCreateGuildAgit() : 0 == m_pclApp"));
        Packet_DB_Create_Guild_Agit* pkt =
            (Packet_DB_Create_Guild_Agit*)header;
        Packet_DB_Create_Guild_Agit_Reply reply;
        reply.m_fieldE = pkt->m_fieldE;
        reply.m_fieldA = pkt->m_guildId;
        m_pclApp->m_dbManager.OnCreateGuildAgit(pkt, reply);
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, 0x16);
        if (reply.m_field12 == 0)
        {
            Packet_Guild_Load_Guild_Cargo cargo;
            cargo.m_fieldA = pkt->m_guildId;
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
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnDeleteGuildAgit() : 0 == m_pclApp"));
        Packet_DB_Delete_Guild_Agit* pkt =
            (Packet_DB_Delete_Guild_Agit*)header;
        Packet_DB_Delete_Guild_Agit_Reply reply;
        reply.m_fieldE = pkt->m_fieldE;
        reply.m_fieldA = pkt->m_guildId;
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
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnUpgradeGuildAgit() : 0 == m_pclApp"));
        Packet_DB_Upgrade_Guild_Agit* pkt =
            (Packet_DB_Upgrade_Guild_Agit*)header;
        Packet_DB_Upgrade_Guild_Agit_Reply reply;
        reply.m_fieldA = pkt->m_guildId;
        reply.m_fieldE = pkt->m_fieldE;
        m_pclApp->m_dbManager.OnUpgradeGuildAgit(pkt, reply);
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, 0x16);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnLoadGuildAgit() Exception Break",
                  0x980, 0x985);
}
void CPacketTranslater::OnUpdateGuildCargo(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DB_Update_Guild_Cargo* pkt = (Packet_DB_Update_Guild_Cargo*)header;
        m_pclApp->m_dbManager.OnUpdateGuildCargo(pkt);
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
        Packet_DB_Guild_Cargo_Upgrade* pkt = (Packet_DB_Guild_Cargo_Upgrade*)header;
        m_pclApp->m_dbManager.OnUpgradeGuildCargo(pkt);
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
        Packet_DB_Insert_Guild_Cargo_History* pkt = (Packet_DB_Insert_Guild_Cargo_History*)header;
        m_pclApp->m_dbManager.OnInsertGuildCargoHistory(pkt);
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
        unsigned int characNo = (unsigned int)pkt->m_characNo;
        if (characNo == -1)
        {
            if (!m_pclApp->m_dbManager.QueryCharacNoByName(pkt->m_name, characNo, 0))
            {
                m_pclApp->m_dbManager.DeleteToBlackListOnly(
                    pkt->m_mid, pkt->m_name);
                DNF_LOG_SCOPE_LINE(0x287,
                    "./log/BlackList",
                    "m_clDBManager.QueryCharacNoByName Err : return false"
                );

                ms->SendToServer((char*)pkt, pkt->packetSize);
                return;
            }
        }
        if (!m_pclApp->m_dbManager.DeleteToBlackList(
                pkt->m_mid, characNo))
        {
            DNF_LOG_SCOPE_LINE(0x28f,
                "./log/BlackList",
                "m_clDBManager.DeleteToBlackList Err : return false"
            );

            ms->SendToServer((char*)pkt, pkt->packetSize);
            return;
        }
        if (pkt->m_characNo == -1)
        {
            pkt->m_characNo = characNo;
            ms->SendToServer((char*)pkt, pkt->packetSize);
        }
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
        unsigned int characNo = (unsigned int)pkt->m_characNo;
        if (characNo == -1)
        {
            if (!m_pclApp->m_dbManager.QueryCharacNoByName(
                    pkt->m_name, characNo,
                    &pkt->m_result))
            {
                DNF_LOG_SCOPE_LINE(0x251,
                    "./log/BlackList",
                    "m_clDBManager.QueryCharacNoByName Err : return false"
                );

                ms->SendToServer((char*)pkt, pkt->packetSize);
                return;
            }
        }
        if (!m_pclApp->m_dbManager.RegisterToBlackList(
                pkt->m_mid, characNo, pkt->m_name))
        {
            DNF_LOG_SCOPE_LINE(0x259,
                "./log/BlackList",
                "m_clDBManager.RegisterToBlackList Err : return false"
            );

            ms->SendToServer((char*)pkt, pkt->packetSize);
            return;
        }
        if (pkt->m_characNo == -1)
        {
            pkt->m_characNo = characNo;
            ms->SendToServer((char*)pkt, pkt->packetSize);
        }
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
        reply.m_fieldA = pkt->m_mid;
        if (!m_pclApp->m_dbManager.QueryBlackList(
                pkt->m_mid,
                (STBlackUserDBType*)((char*)&reply + 0xe)))
        {
            DNF_LOG_SCOPE_LINE(0x2b8,
                "./log/BlackList",
                "m_clDBManager.QueryBlackList Err : return false"
            );

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
        }
        DNF_LOG_SCOPE_LINE(0xca,
            "./log/GuildModify",
            "::OnSaveGuild s(%d) g(%d) k(%d) g_level(%d) g_cnt(%d) g_exp(%d) g_sub_cnt(%d) power_side(%d) power_war_p(%d) agit(%d) power_join_cnt(%d)",
            pkt->m_serverId,
            pkt->m_guildId,
            pkt->m_fieldCC,
            pkt->m_info.m_lev,
            pkt->m_info.m_memberCount,
            pkt->m_info.m_guildExp,
            (unsigned char)pkt->m_info.m_field2D,
            pkt->m_info.m_powerSide,
            pkt->m_info.m_powerWarPoint,
            pkt->m_info.m_guildAgitFlag,
            pkt->m_info.m_powerJoinCount
        );
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
        reply.m_fieldA = pkt->m_guildId;
        reply.m_fieldE = pkt->m_characNo;
        reply.m_field1A = pkt->m_grade;
        reply.m_field1B = pkt->m_secedeType;
        memcpy((char*)&reply + 0x1f, pkt->m_characName, 0x1d);
        if (!m_pclApp->m_dbManager.GuildSecede(
                pkt,
                *(unsigned int*)((char*)&reply + 0x12),
                *(unsigned int*)((char*)&reply + 0x3d),
                *(unsigned int*)((char*)&reply + 0x16)))
        {
            DNF_LOG_SCOPE_LINE(0x5e1,
                "./log/GuildModify",
                "::OnGuildSecede Err g(%d) n(%s) c(%d) r(%d) f(%d)",
                pkt->m_guildId,
                pkt->m_characName,
                reply.m_field12,
                reply.m_field16,
                pkt->m_grade
            );

        }
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
        CMyFileLog log2(__FUNCTION__, 0x5ed);
        log2("./log/GuildModify", "::OnGuildSecede g(%d) c(%d) r(%d) f(%d)",
             pkt->m_guildId,
             reply.m_field12,
             reply.m_field16,
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
        struct SaveGuildWarInfoLocal
        {
            char m_pad[0xa];
            unsigned char m_serverId;   // +0xa
            unsigned int m_guildIds[10]; // +0xb
            unsigned int m_points[10];   // +0x33
        } __attribute__((packed));
        SaveGuildWarInfoLocal* pkt = (SaveGuildWarInfoLocal*)header;
        int serverId = pkt->m_serverId;
        if (!m_pclApp->m_dbManager.SaveGuildWarPointList(
                serverId, pkt->m_guildIds, pkt->m_points))
        {
            DNF_LOG_SCOPE_LINE(0x201,
                "./log/Guild",
                "m_clDBManager.OnSaveGuildWarInfo Err : return false"
            );

        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSaveGuildWarInfo() Exception Break",
                  0x207, 0x20c);
}
void CPacketTranslater::OnSavePowerWarBonusPoint(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnSavePowerWarBonusPoint() : 0 == m_pclApp"));
        Packet_DB_Save_Power_War_Bonus_Point* pkt =
            (Packet_DB_Save_Power_War_Bonus_Point*)header;
        m_pclApp->m_dbManager.OnSavePowerWarBonusPoint(
            pkt);
        if (pkt->m_count > 0)
        {
            Packet_Notify_New_Group_Mail notice;
            int n = pkt->m_count;
            if (n > 0x12c)
                n = 0x12c;
            for (int i = 0; i < n; i++)
                notice.m_characNos[i] = pkt->m_entries[i].m_characNo;
            CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
            gs->SendToServer((char*)&notice, notice.packetSize);
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
        DNF_LOG_SCOPE_LINE(0x6a4,
            "./log/QueryGuildBooting",
            "QueryGuildBooting A_Side(%d) B_Side(%d)",
            pkt->m_aSidePoint,
            pkt->m_bSidePoint
        );

    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestGuildCreate() Exception Break",
                  0x6a8, 0x6ad);
}
void CPacketTranslater::OnSavePowerWarStatueRanker(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnSavePowerWarStatueRanker() : 0 == m_pclApp"));
        Packet_DB_Save_Power_War_Statue_Ranker* pkt = (Packet_DB_Save_Power_War_Statue_Ranker*)header;
        m_pclApp->m_dbManager.OnSavePowerWarStatueRanker(pkt);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSavePowerWarStatueRanker() Exception Break",
                  0x8e4, 0x8e9);
}
void CPacketTranslater::OnSavePowerWarPointReward(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnSavePowerWarPoint() : 0 == m_pclApp"));
        Packet_DB_Save_Power_War_Point_Reward* pkt = (Packet_DB_Save_Power_War_Point_Reward*)header;
        m_pclApp->m_dbManager.OnSavePowerWarPointReward(pkt);
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
        Packet_Load_Periodic_Message* pkt =
            (Packet_Load_Periodic_Message*)header;
        if (m_pclApp->m_dbManager.OnLoadPeriodicMessage(
                pkt, &reply) == 0)
            return;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&reply, 0x212);
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
        unsigned int letterNo = 0;
        if (pkt->m_delayHours != 0)
        {
            time_t now = time(0);
            struct tm* lt = localtime(&now);
            lt->tm_hour += 1;
            lt->tm_min = 0;
            lt->tm_sec = 0;
            time_t nextHour = mktime(lt);
            unsigned int occTime =
                (unsigned int)nextHour +
                (unsigned int)pkt->m_delayHours * 0x15180U;
            letterNo = (occTime - 0x44a53c70U) / 0x15180U;
        }
        if (!m_pclApp->m_dbManager.InsertMail(
                pkt->m_characNo, pkt->m_subject, pkt->m_content,
                pkt->m_fieldE, letterNo, pkt->m_field12, pkt->m_field16))
        {
            DNF_LOG_SCOPE_LINE(0xd1e,
                "./log/GuildEvent",
                "CPacketTranslater.OnDBMWInsertMail Err(%d) : return false",
                pkt->m_characNo
            );

            return;
        }
        Packet_Monitor_Notify_New_Mail notice;
        notice.m_fieldA = pkt->m_characNo;
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
        if (!m_pclApp)
            return;
        Packet_DBMW_Query_Member* pkt = (Packet_DBMW_Query_Member*)header;
        reply.m_fieldB = pkt->m_characNo;
        if (!m_pclApp->m_dbManager.QueryMember(
                pkt->m_characNo, reply))
        {
            DNF_LOG_SCOPE_LINE(0x117,
                "./log/QueryErr",
                "CPacketTranslater::OnQueryMember() Error, member_id(%d)", pkt->m_characNo
            );
        }
        CMonitorServer* ms =
            m_pclApp->m_serverHandler->GetMonitorServer();
        reply.packetSize =
            (unsigned short)((unsigned char)reply.m_master.m_count * 0x27) + 0x3f;
        int size = reply.packetSize;
        ms->SendToServer((char*)&reply, size);
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
                DNF_LOG_SCOPE_LINE(0x98,
                    "./log/Except",
                    "CPacketTranslater::OnQueryGuildMember() Query Error : %d, Char No : %d, Guild Id : %d", (unsigned char)reply.m_flag, pkt->m_guildId,
                    reply.m_fieldB
                );
            }
            CGuildServer* gs =
                m_pclApp->m_serverHandler->GetGuildServer();
            gs->SendToServer((char*)&reply, 0x2d);
        }
    }
    DNF_CATCH_LOG_PRINTF("./log/Except",
                         "CPacketTranslater::OnQueryGuildMember() Exception Break",
                         0xa8, 0xae);
}
void CPacketTranslater::OnQueryTodayGuildMemeber(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnQueryTodayGuildMemeber() : 0 == m_pclApp"));
        Packet_DBMW_Request_Today_Guild_Member* pkt =
            (Packet_DBMW_Request_Today_Guild_Member*)header;
        CGuildServer* gs;
        Packet_Reply_Today_Guild_Member reply;
        m_pclApp->m_dbManager.QueryTodayGuildMember(
            pkt->m_guildId, reply);
        gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, reply.packetSize);
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
        reply.m_fieldA = pkt->m_characNo;
        m_pclApp->m_dbManager.QueryGuildCreate(
            pkt,
            *(unsigned int*)((char*)&reply + 0xe),
            *(unsigned int*)((char*)&reply + 0x12));
        memcpy((char*)&reply + 0x16, pkt->m_guildName, 0x16);
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
        DNF_LOG_SCOPE_LINE(0x65f,
            "./log/GuildModify",
            "::OnRequestGuildCreate g(%d) c(%d) r(%d)",
            reply.m_fieldE,
            pkt->m_characNo,
            reply.m_field12
        );

    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnRequestGuildCreate() Exception Break",
                  0x663, 0x668);
}
void CPacketTranslater::OnSavePowerWarUserRank(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnSavePowerWarUserRank() : 0 == m_pclApp"));
        Packet_DB_Save_Power_War_User_Rank* pkt = (Packet_DB_Save_Power_War_User_Rank*)header;
        m_pclApp->m_dbManager.OnSavePowerWarUserRank(pkt);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnSavePowerWarUserRank() Exception Break",
                  0x899, 0x89e);
}
void CPacketTranslater::OnSavePowerWarGuildRank(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnSavePowerWarGuildRank() : 0 == m_pclApp"));
        Packet_DB_Save_Power_War_Guild_Rank* pkt = (Packet_DB_Save_Power_War_Guild_Rank*)header;
        m_pclApp->m_dbManager.OnSavePowerWarGuildRank(pkt);
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
        reply.m_fieldE = pkt->m_guildId;
        reply.m_field12 = pkt->m_id;
        reply.m_field16 = pkt->m_characNo;
        if (!m_pclApp->m_dbManager.OnGuildJoinByListApprove(
                pkt->m_guildId, pkt->m_serverId,
                pkt->m_id, pkt->m_characNo,
                reply.m_joinInfo,
                (unsigned int&)reply.m_fieldA))
        {
            if (reply.m_fieldA == 0)
                reply.m_fieldA = 1;
            DNF_LOG_SCOPE_LINE(0x52a,
                "./log/GuildModify",
                "OnGuildJoin Err(g:%d,c:%d,r:%d) : return false", pkt->m_guildId, pkt->m_characNo,
                reply.m_fieldA
            );
        }
        m_pclApp->m_serverHandler->GetGuildServer()->SendToServer(
            (char*)&reply, reply.packetSize);
        CMyFileLog log2(__FUNCTION__, 0x530);
        log2("./log/GuildModify", "::OnGuildJoin g(%d) c(%d) r(%d)",
             pkt->m_guildId, pkt->m_characNo, reply.m_fieldA);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnGuildJoin() Exception Break",
                  0x534, 0x539);
}
void CPacketTranslater::OnInsertUdpCharacteristic(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnInsertUdpCharacteristic() : 0 == m_pclApp"));
        Packet_Udp_Characteristic* pkt = (Packet_Udp_Characteristic*)header;
        m_pclApp->m_dbManager.InsertUdpCharacteristic(pkt);
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
        reply.m_fieldA = pkt->m_mid;
        reply.m_fieldE = pkt->m_characNo;
        memcpy((char*)&reply + 0x12, pkt->m_name, 0x1d);
        reply.m_rest[0x1e] = !m_pclApp->m_dbManager.DelBuddy(
            pkt->m_mid, pkt->m_characNo);
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&reply, reply.packetSize);
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
        struct SaveGuildWarInfoLocal
        {
            char m_pad[0xa];
            unsigned char m_serverId;   // +0xa
            unsigned int m_guildIds[10]; // +0xb
            unsigned int m_points[10];   // +0x33
        } __attribute__((packed));
        SaveGuildWarInfoLocal* pkt = (SaveGuildWarInfoLocal*)header;
        int serverId = pkt->m_serverId;
        if (!m_pclApp->m_dbManager.SaveGuildWarPointList(
                serverId, pkt->m_guildIds, pkt->m_points))
        {
            DNF_LOG_SCOPE_LINE(0x223,
                "./log/GuildWar",
                "m_clDBManager.OnEndGuildWar Err : return false"
            );

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
        DNF_LOG_SCOPE_LINE(0x1567,
            "./log/AradOnly",
            "CPacketTranslater::onStartGameEventFromServer data. (event:%d)\n",
            pkt->m_eventType
        );

        if (!m_pclApp->m_dbManager.insertServerGameEvent(
                pkt))
        {
            CMyFileLog log2(__FUNCTION__, 0x156b);
            log2("./log/AradOnly",
                 "CPacketTranslater::onStartGameEventFromServer fail\n");
            return;
        }
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* buf = (char*)tcp->makePacketHeader(0x27fb, 0x1a);
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
        DNF_LOG_SCOPE_LINE(0x1592,
            "./log/AradOnly",
            "CPacketTranslater::onEndGameEventFromServer data. (event:%d)\n",
            pkt->m_eventType
        );

        if (!m_pclApp->m_dbManager.updateServerGameEvent(
                pkt))
        {
            CMyFileLog log2(__FUNCTION__, 0x1596);
            log2("./log/AradOnly",
                 "CPacketTranslater::onEndGameEventFromServer fail\n");
            return;
        }
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* buf = (char*)tcp->makePacketHeader(0x27fc, 0x16);
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
            DNF_LOG_SCOPE_LINE(0x1a4,
                "./log/DBMemberErr",
                "Member key Update Error %d\n",
                pkt->m_characNo
            );

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
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        gs->SendToServer((char*)&reply, reply.packetSize);
        DNF_LOG_SCOPE_LINE(0x685,
            "./log/QueryGuildBooting",
            "QueryGuildBooting A_Side(%d) B_Side(%d) Winner(%d)",
            reply.m_fieldA,
            reply.m_fieldE,
            reply.m_field12
        );

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
        {
            if (!pkt)
                return;
            DNF_LOG_SCOPE_LINE(0x12e5, "./log/statistic", "TOD Statistic Error\nTOD uv(%d)", pkt->m_uv);

            for (int i = 1; i <= 0x64; i++)
            {
                DNF_LOG_SCOPE_LINE(0x12e7, "./log/statistic",
                    "TOD Layer(%d), enter(%d), succ(%d)", i,
                    pkt->m_entries[i].m_field12, pkt->m_entries[i].m_fieldE);
            }
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
            DNF_LOG_SCOPE_LINE(0xd5a,
                "./log/GuildEvent",
                "CPacketTranslater.OnDBMWVillageAttackRank Err : return false"
            );

            return;
        }
        if (flag)
        {
            Packet_Monitor_Event_Start ev;
            ev.m_fieldA = 4;
            ev.m_fieldE = (unsigned short)a;
            ev.m_field10 = (unsigned short)b;
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
        Packet_Item_Limit_Edition_Load_Data_Req* pkt =
            (Packet_Item_Limit_Edition_Load_Data_Req*)header;
        if (!m_pclApp->m_dbManager.onItemLimitEditionLoadData(
                pkt, &reply))
        {
            DNF_LOG_SCOPE_LINE(0xaa4,
                "./log/LimitEdition",
                "CPacketTranslater::onItemLimitEditionLoadDataReq data load fail\n"
            );

            return;
        }
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&reply, 0x7ef);
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
        Packet_DBMW_Collect_Items_Gm_View* pkt =
            (Packet_DBMW_Collect_Items_Gm_View*)header;
        m_pclApp->m_dbManager.updateCollectItemsGm(
            pkt->m_serverInfo, pkt->m_fieldF,
            pkt->m_fieldB, pkt->m_field13);
        Packet_CollectItemsResult reply;
        reply.m_fieldE = pkt->m_fieldF;
        reply.m_fieldA = pkt->m_fieldB;
        reply.m_field12 = pkt->m_field13;
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
        Packet_GameServer2Statisctics2DBServer* pkt =
            (Packet_GameServer2Statisctics2DBServer*)header;
        DNF_LOG_SCOPE_LINE(0x15bd,
            "./log/AradOnly",
            "CPacketTranslater::onInsertHolePunchingResult.\n"
        );

        if (!m_pclApp->m_dbManager.insertHolePunchingResult(
                pkt))
        {
            DNF_LOG_SCOPE_LINE(0x15c0,
                "./log/AradOnly",
                "CPacketTranslater::insertHolePunchingResult fail\n"
            );
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
        const Packet_Item_Limit_Edition_Update* pkt =
            (const Packet_Item_Limit_Edition_Update*)header;
        if (!m_pclApp->m_dbManager.onItemLimitEditionUpdateData(
                pkt))
        {
            DNF_LOG_SCOPE_LINE(0xac1,
                "./log/LimitEdition",
                "CPacketTranslater::onItemLimitEditionLoadDataReq data load fail\n"
            );

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
        Packet_Stat_Compatibility_Index* pkt =
            (Packet_Stat_Compatibility_Index*)header;
        if (!m_pclApp->m_dbManager.updateCompatibilityIndex(pkt))
        {
            DNF_LOG_SCOPE_LINE(0x13a5,
                "./log/Query",
                "CPacketTranslater::onCompatibilityIndex data update fail\n"
            );

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
        Packet_P2P_Statistics* pkt =
            (Packet_P2P_Statistics*)header;
        if (!m_pclApp->m_dbManager.QueryP2PStatistics(pkt))
        {
            DNF_LOG_SCOPE_LINE(0x13c1, "./log/Query", "CPacketTranslater::OnP2PStatistics()");

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
        Packet_DBMW_Query_Msg* pkt =
            (Packet_DBMW_Query_Msg*)header;
        if (!m_pclApp->m_dbManager.QueryMsg(pkt))
        {
            DNF_LOG_SCOPE_LINE(0xd3d,
                "./log/GuildEvent",
                "CPacketTranslater.OnDBMWQueryMsg Err : return false"
            );

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
        Packet_Randombox_statistic_DB* pkt =
            (Packet_Randombox_statistic_DB*)header;
        if (!m_pclApp->m_dbManager.UpdateRandomboxStatistic(pkt))
        {
            DNF_LOG_SCOPE_LINE(0xdbc,
                "./log/statistic",
                "CPacketTranslater.OnDBMWRandomboxStatic Err : return false"
            );

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
        Packet_DBMW_Query_String* pkt =
            (Packet_DBMW_Query_String*)header;
        if (!m_pclApp->m_dbManager.GetDBMWStatistic(pkt))
        {
            DNF_LOG_SCOPE_LINE(0xb66,
                "./log/PowerStatistic",
                "CPacketTranslater::OnBloodDungeonStatistic insert fail\n"
            );

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
        Packet_Emblem_Create_Statistic_DB* pkt =
            (Packet_Emblem_Create_Statistic_DB*)header;
        if (!m_pclApp->m_dbManager.UpdateCreateEmblemStatistic(pkt))
        {
            DNF_LOG_SCOPE_LINE(0xd9f,
                "./log/statistic",
                "CPacketTranslater.OnDBMWCreateEmblemStatic Err : return false"
            );

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
        Packet_Avater_Disjoint_Statistic_DB* pkt =
            (Packet_Avater_Disjoint_Statistic_DB*)header;
        puts("TEST : OnDBMWDisjointAvatarStatic");
        if (!m_pclApp->m_dbManager.UpdateDisjointAvatarStatistic(pkt))
        {
            DNF_LOG_SCOPE_LINE(0xd81,
                "./log/statistic",
                "CPacketTranslater.OnDBMWDisjointAvatarStatic Err : return false"
            );

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
        Packet_DBMW_Collect_Items_Update_View* pkt =
            (Packet_DBMW_Collect_Items_Update_View*)header;
        Packet_CollectItemsResult reply;
        unsigned char flag = 0;
        m_pclApp->m_dbManager.selectCollectItems(
            pkt->m_serverInfo,
            *(int*)&reply.m_fieldE, *(int*)&reply.m_fieldA,
            *(unsigned int*)&reply.m_field12, flag);
        int diff = pkt->m_fieldA - reply.m_fieldE;
        m_pclApp->m_dbManager.updateCollectItems(
            pkt->m_serverInfo, diff,
            reply.m_field12, flag);
        if (flag == 0 &&
            pkt->m_field13 == 0 && diff < 0)
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
        DNF_LOG_SCOPE_LINE(0x1df, "./log/Udp", "%d Server Connection Complete!", pkt->m_serverId);

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
        Packet_DBMW_Change_Char_Name* pkt = (Packet_DBMW_Change_Char_Name*)header;
        m_pclApp->m_dbManager.ChangeCharName(pkt);
        m_pclApp->m_dbManager.ChangePvPBuddyName(pkt);
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
        Packet_Server_Match_data_DBMW* pkt = (Packet_Server_Match_data_DBMW*)header;
        m_pclApp->m_dbManager.OnServerMatchData(pkt);
    }
    DNF_CATCH_LOG_NN("./log/Except",
                     "CPacketTranslater::OnServerMatchData() Exception Break",
                     0xed5, 0xeda);
}
void CPacketTranslater::OnManagerEventTriggerAck(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_Manager_Event_Trigger_Ack* pkt = (Packet_Manager_Event_Trigger_Ack*)header;
        m_pclApp->m_dbManager.OnManagerEventTriggerAck(pkt);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnManagerEventTriggerAck() Exception Break",
                  0xf1e, 0xf23);
}
void CPacketTranslater::OnRecvLoadingTimeReport(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Loading_Time_Report* pkt = (Packet_DBMW_Loading_Time_Report*)header;
        m_pclApp->m_dbManager.OnSaveLoadingTimeReport(pkt);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnRecvLoadingTimeReport() Exception Break",
                  0xa09, 0xa0e);
}
void CPacketTranslater::OnSaveFatigueBatteryStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Fatigue_Battery_Money_Statistic* pkt = (Packet_DBMW_Fatigue_Battery_Money_Statistic*)header;
        m_pclApp->m_dbManager.OnSaveFatigueBattery(pkt);
    }
    DNF_CATCH_LOG("./log/Except",
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
        fieldA = (unsigned char)(pkt->m_fieldA -
                                 ((CAppConfig*)m_pclApp->m_appConfig)->Get_DbmwType());
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
        STGuildBoardDBInfo boards[0x32];
        if (!m_pclApp->m_dbManager.OnLoadGuildBoard(
                ((FieldViewP<0xa,int>*)pkt)->v, count, boards))
        {
            DNF_LOG_SCOPE_LINE(0xf8a,
                "./log/Except",
                "CPacketTranslater::OnDBLoadRequestGuildBoardOpen()\tGuild Id : %d, \t Query Result : %d\n", ((FieldViewP<0xa,int>*)pkt)->v,
                0
            );

            Packet_DB_Load_Reply_Guild_Board_Open reply;
            reply.m_fieldA = 1;
            reply.m_fieldC = 1;
            reply.m_fieldD = ((FieldViewP<0xa,int>*)pkt)->v;
            reply.m_field11 = ((FieldViewP<0xe,int>*)pkt)->v;
            reply.m_field15 = 0;
            gs->SendToServer((char*)&reply, 0x688);
            return;
        }
        int pageCount = count / 10;
        int rem = count - pageCount * 10;
        for (int page = 0; page < pageCount; page++)
        {
            Packet_DB_Load_Reply_Guild_Board_Open reply;
            reply.m_fieldC = 0;
            reply.m_fieldD = ((FieldViewP<0xa,int>*)pkt)->v;
            reply.m_field11 = ((FieldViewP<0xe,int>*)pkt)->v;
            reply.m_field15 = 0xa;
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
                reply.m_fieldC = 1;
            gs->SendToServer((char*)&reply, 0x688);
        }
        if (rem != 0)
        {
            Packet_DB_Load_Reply_Guild_Board_Open reply;
            reply.m_fieldC = 1;
            reply.m_fieldD = ((FieldViewP<0xa,int>*)pkt)->v;
            reply.m_field11 = ((FieldViewP<0xe,int>*)pkt)->v;
            reply.m_field15 = (char)rem;
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
        CMyFileLog log1(__FUNCTION__, 0x141b);
        log1("./log/event",
             "CPacketTranslater::OnPcRoomPlayTimeReward Noti acc_id(%d), charac_no(%d), server_info(%d)",
             accId, characNo, serverInfo);
        unsigned int pinNo = 0;
        char pinBuf[0x15] = {0};
        if (!m_pclApp->m_dbManager.updateNexonPinPcRoomPlayTimeEvent(
                serverInfo, accId, pinNo, pinBuf, 0x15))
        {
            DNF_LOG_SCOPE_LINE(0x1423,
                "./log/event",
                "CPacketTranslater::OnPcRoomPlayTimeReward Error acc_id(%d), charac_no(%d), server_info(%d)", accId, characNo,
                serverInfo
            );

            return;
        }
        time_t now = time(0);
        tm* t = localtime(&now);
        t->tm_hour += 1;
        t->tm_min = 0;
        t->tm_sec = 0;
        time_t tomorrow = mktime(t);
        char str[0x100] = {0};
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
            DNF_LOG_SCOPE_LINE(0x143d,
                "./log/event",
                "CPacketTranslater::OnPcRoomPlayTimeReward Letter Error acc_id(%d), charac_no(%d), server_info(%d), pin_num(%d)", accId, characNo,
                serverInfo,
                pinNo
            );

            return;
        }
        Packet_Monitor_Notify_New_Mail mail;
        mail.m_fieldA = characNo;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&mail, mail.packetSize);
        CMyFileLog log2(__FUNCTION__, 0x1447);
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
        reply.m_fieldA = pkt->m_mid;
        int result = 0;
        m_pclApp->m_dbManager.AddBuddy(pkt->m_mid, pkt->m_name,
                                       reply.m_info, result);
        reply.m_field35 = (char)result;
        CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
        ms->SendToServer((char*)&reply, reply.packetSize);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnAddBuddy() Exception Break",
                  0x2f5, 0x2fa);
}
void CPacketTranslater::OnRequestIPCounterList(PacketHeader* header)
{
    std::vector<st_ip_counter_list> vec1;
    std::vector<st_full_ip_counter_list> vec2;
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Request_IPCounter_List* pkt =
            (Packet_DBMW_Request_IPCounter_List*)header;
        if (!m_pclApp->m_dbManager.QueryIPCounter(
                pkt->m_serverGroup, vec1, vec2))
        {
            DNF_LOG_SCOPE_LINE(0xb8a,
                "./log/Secu",
                "CPacketTranslater::OnRequestIPCounterList insert fail\n"
            );

            return;
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
                    (*(st_ip_counter_list*)&reply.m_rest[count * 0x14])
                        .CopyStruct(vec1[srcIdx]);
                    srcIdx++;
                    count++;
                }
                reply.m_fieldB = (char)count;
                if (batch == 0)
                    reply.m_fieldA = 0;
                else if (srcIdx >= size)
                    reply.m_fieldA = 2;
                else
                    reply.m_fieldA = 1;
                reply.packetSize =
                    (unsigned short)(0xbc4 - (0x96 - count) * 0x14);
                ms->SendToServer((char*)&reply, reply.packetSize);
                DNF_LOG_SCOPE_LINE(0xbab,
                    "./log/Secu",
                    "[IP Counter] Packet Send - Stats : %3d, Cnt : %3d", (int)reply.m_fieldA,
                    count
                );

                batch++;
            }
        }
        else
        {
            Packet_Response_IPCounterList reply;
            reply.packetSize = 0xc;
            ms->SendToServer((char*)&reply, reply.packetSize);
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
                    (*(st_full_ip_counter_list*)&reply.m_rest[count * 0x18])
                        .CopyStruct(vec2[srcIdx]);
                    srcIdx++;
                    count++;
                }
                reply.m_fieldB = (char)count;
                if (batch == 0)
                    reply.m_fieldA = 0;
                else if (srcIdx >= size)
                    reply.m_fieldA = 2;
                else
                    reply.m_fieldA = 1;
                reply.packetSize =
                    (unsigned short)(0xe1c - (0x96 - count) * 0x18);
                ms->SendToServer((char*)&reply, reply.packetSize);
                DNF_LOG_SCOPE_LINE(0xbd8,
                    "./log/Secu",
                    "[D_IP Counter] Packet Send - Stats : %3d, Cnt : %3d", (int)reply.m_fieldA,
                    count
                );

                batch++;
            }
        }
        else
        {
            Packet_Response_D_IPCounterList reply;
            reply.packetSize = 0xc;
            ms->SendToServer((char*)&reply, reply.packetSize);
        }
        return;
    }
    DNF_CATCH_LOG("./log/Except",
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
        reply.m_fieldA = 1;
        if (pkt->m_limit != 0x2710)
            reply.m_fieldA = 0;
        CMyFileLog log1(__FUNCTION__, 0x59b);
        log1("./log/DeathTower", "(tower_idx:%d)(rank count:%d)\n",
             pkt->m_towerIndex, ranks.size());
        int i = 0;
        for (std::vector<stTowerRank_t>::const_iterator it = ranks.begin();
             it != ranks.end(); ++it)
        {
            memcpy((char*)&reply + 0x13 + i * 0x65, &*it, 0x65);
            i++;
            if (i > 0x3b)
            {
                reply.m_fieldB = (char)i;
                reply.packetSize = 0x17bf;
                ms->SendToServer((char*)&reply, 0x17bf);
                DNF_LOG_SCOPE_LINE(0x5a8,
                    "./log/DeathTower",
                    "(tower_idx:%d)(send count:%d)\n",
                    pkt->m_towerIndex,
                    i
                );

                i = 0;
                reply.m_fieldA = 0;
                DNFFLib::Sleep_Ext(0, 0x30d40);
            }
        }
        if (i != 0)
        {
            reply.m_fieldB = (char)i;
            reply.packetSize = (unsigned short)(0x13 + i * 0x65);
            ms->SendToServer((char*)&reply, reply.packetSize);
            DNF_LOG_SCOPE_LINE(0x5b3,
                "./log/DeathTower",
                "(tower_idx:%d)(send count:%d)\n",
                pkt->m_towerIndex,
                i
            );

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
            DNF_LOG_SCOPE_LINE(0xdf6,
                "./log/Secu",
                "CPacketTranslater::OnRequestARSInfo Select fail\n"
            );

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
                    (*(st_ars_info_list*)&reply.m_rest[count * 0xc])
                        .CopyStruct(list[srcIdx]);
                    srcIdx++;
                    count++;
                }
                reply.m_fieldA = (char)count;
                if (batch == 0)
                    reply.m_fieldE = 0;
                else if (srcIdx < size)
                    reply.m_fieldE = 1;
                else
                    reply.m_fieldE = 2;
                reply.packetSize =
                    (unsigned short)(0x4bf - (0x64 - count) * 0xc);
                ms->SendToServer((char*)&reply, reply.packetSize);
                DNF_LOG_SCOPE_LINE(0xe1a,
                    "./log/Secu",
                    "[ARS_INFO] Packet Send - Stats : %3d, Cnt : %3d", batch,
                    count
                );

                batch++;
            }
        }
        else
        {
            Packet_Set_ARS_Info reply;
            reply.m_fieldA = 0;
            reply.m_fieldE = 0;
            reply.packetSize = 0x4bf;
            ms->SendToServer((char*)&reply, reply.packetSize);
            DNF_LOG_SCOPE_LINE(0xe2d,
                "./log/Secu",
                "[ARS_INFO] Packet Send - Stats : %3d, Cnt : 0", 0
            );

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
        if (!m_pclApp->m_dbManager.QueryGuild(
                pkt->m_serverId, guildId, reply))
        {
            DNF_LOG_SCOPE_LINE(0x56,
                "./log/Except",
                "CPacketTranslater::OnQueryGuild()\tGuild Id : %d,\t Query Result : %d\n", guildId,
                reply.m_fieldA
            );
        }
        if (!m_pclApp->m_dbManager.QuerySubGuildMaster(
                pkt->m_serverId, guildId, reply))
        {
            DNF_LOG_SCOPE_LINE(0x5b,
                "./log/Except",
                "CPacketTranslater::OnQueryGuild()\tGuild Id : %d,\t Query Result : %d\n", guildId,
                reply.m_fieldA
            );
        }
        if (!m_pclApp->m_dbManager.QueryGuildSkill(
                pkt->m_serverId, guildId, reply))
        {
            DNF_LOG_SCOPE_LINE(0x60,
                "./log/Except",
                "CPacketTranslater::OnQueryGuild()\tGuild Id : %d,\t Query Result : %d\n", guildId,
                reply.m_fieldA
            );
        }
        if (!m_pclApp->m_dbManager.QueryGuildNotiMessage(
                pkt->m_serverId, guildId, reply))
        {
            DNF_LOG_SCOPE_LINE(0x65,
                "./log/Except",
                "CPacketTranslater::OnQueryGuild()\tGuild Id : %d,\t Query Result : %d\n", guildId,
                reply.m_fieldA
            );
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
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnReqOntimeEventIdx() : 0 == m_pclApp"));
        Packet_Result_OnTimeEvent_Idx reply;
        reply.m_fieldE = m_pclApp->m_dbManager.QueryOnTimeEventIdx(reply);
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* tmp = (char*)tcp->makePacketHeader(0x2341, 0xf);
            char* pkt = tmp;
            memcpy(pkt + 0xa, (char*)&reply + 0xa, 5);
            tcp->SendToServer(pkt);
        }
        else
        {
            CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
            ms->SendToServer((char*)&reply, 0xf);
            DNF_LOG_SCOPE_LINE(0x1098, "./log/TcpServer", "OnReqOntimeEventIdx:GetTcpServer null");

        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryTodayGuildMemeber() Exception Break",
                  0x109d, 0x10a2);
}
void CPacketTranslater::OnReqOntimeEventIdxUpdate(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnReqOntimeEventIdxUpdate() : 0 == m_pclApp"));
        Packet_Req_Ontime_Event_Idx_Update* pktIn =
            (Packet_Req_Ontime_Event_Idx_Update*)header;
        m_pclApp->m_dbManager.QueryOnTimeEventIdxUpdate(
            pktIn);
        Packet_Result_Ontime_Event_Idx_Update reply;
        reply.m_fieldA = pktIn->m_no;
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* tmp = (char*)tcp->makePacketHeader(0x2348, 0xe);
            char* pkt = tmp;
            memcpy(pkt + 0xa, (char*)&reply + 0xa, 4);
            tcp->SendToServer(pkt);
        }
        else
        {
            CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
            ms->SendToServer((char*)&reply, 0xf);
            DNF_LOG_SCOPE_LINE(0x114c, "./log/TcpServer", "OnReqOntimeEventItem:GetTcpServer null");

        }
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnQueryTodayGuildMemeber() Exception Break",
                  0x1151, 0x1156);
}
void CPacketTranslater::OnReqOntimeEventItem(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException(std::string(
                "CPacketTranslater::OnReqOntimeEventItem() : 0 == m_pclApp"));
        Packet_Result_Ontime_Event_Item reply;
        m_pclApp->m_dbManager.QueryOnTimeEventItem(reply);
        CTcpServer* tcp =
            m_pclApp->Get_ServerHandler()->GetTcpServer((unsigned char)0xa);
        if (tcp)
        {
            char* tmp = (char*)tcp->makePacketHeader(0x2346, 0x14);
            char* pkt = tmp;
            memcpy(pkt + 0xa, (char*)&reply + 0xa, 0xa);
            tcp->SendToServer(pkt);
        }
        else
        {
            CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
            ms->SendToServer((char*)&reply, 0xf);
            DNF_LOG_SCOPE_LINE(0x1118, "./log/TcpServer", "OnReqOntimeEventItem:GetTcpServer null");

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
        reply.m_fieldA = pkt->m_characNo;
        if (m_pclApp->m_dbManager.QueryBuddyInfo(
                pkt->m_characNo,
                reply.m_rest,
                reply.m_fieldE))
        {
            CMonitorServer* ms = m_pclApp->m_serverHandler->GetMonitorServer();
            ms->SendToServer((char*)&reply, reply.packetSize);
        }
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
        Packet_DB_Call_Unconn_Guild_Member* pkt =
            (Packet_DB_Call_Unconn_Guild_Member*)header;
        Packet_DB_Reply_Unconn_Guild_Member reply;
        reply.m_fieldA = pkt->m_fieldA;
        reply.m_fieldE = pkt->m_fieldE;
        if (!m_pclApp->m_dbManager.QueryGuildMemberProxy(
                pkt->m_fieldA, pkt->m_fieldE, reply.m_proxy))
        {
            DNF_LOG_SCOPE_LINE(0x3ae,
                "./log/GuildMemberErr",
                "Query Guild Member List Error g(%d), c(%d)\n", pkt->m_fieldA,
                pkt->m_fieldE
            );

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
        Packet_DB_Call_Guild_All_Members* pkt =
            (Packet_DB_Call_Guild_All_Members*)header;
        Packet_DB_Reply_Guild_All_Members reply;
        reply.m_fieldA = pkt->m_fieldA;
        reply.m_fieldE = pkt->m_fieldE;
        unsigned short tot = 0;
        STGuildMemberProxy* temp = m_pclApp->m_guildManager->GetArrayTempGuildMemberList();
        memset(temp, 0, 0x4c2c);
        if (!m_pclApp->m_dbManager.QueryGuildAllMembersProxy(pkt->m_fieldA, temp, tot))
        {
            DNF_LOG_SCOPE_LINE(0x362,
                "./log/GuildMemberErr",
                "Query All Guild Member List Error g(%d), c(%d)\n", pkt->m_fieldA,
                pkt->m_fieldE
            );

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
            reply.packetSize = (unsigned short)(remain * 0x41 + 0x14);
            gs->SendToServer((char*)&reply, reply.packetSize);
        }
        DNF_LOG_SCOPE_LINE(0x38b,
            "./log/GuildModify",
            "Query All Guild Member List g(%d), c(%d), tot(%d)\n", pkt->m_fieldA,
            pkt->m_fieldE,
            tot
        );

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
        DNF_LOG_SCOPE_LINE(0x43,
            "./log/Monitor",
            "First Heart Beat Arrived From %d Group Monitor!",
            idx
        );

    }
}
void CPacketTranslater::OnEventStart(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            throw CDNFException("CPacketTranslater::OnEventStart : 0 == m_pclApp");
        m_pclApp->m_serverHandler->SendAllTcpServer(header);
        DNF_LOG_SCOPE_LINE(0x70,
            "./log/Web",
            "CPacketTranslater::OnEventStart() eventCode(%d), eventParam1(%d), eventParam2(%d)\n", ((FieldViewP<0xa,int>*)header)->v,
            ((FieldView<0xe,unsigned short>*)header)->v,
            ((FieldView<0x10,unsigned short>*)header)->v
        );

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
        DNF_LOG_SCOPE_LINE(0x92,
            "./log/Web",
            "CPacketTranslater::OnEventEnd() eventCode(%d)\n",
            ((FieldViewP<0xa,int>*)header)->v
        );

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
        DNF_LOG_SCOPE_LINE(0xb5,
            "./log/Web",
            "CPacketTranslater::OnCommonPacket() packet_id(%d)\n",
            header->packetId
        );

    }
    DNF_CATCH_LOG_THROW("CPacketTranslater::OnEventEnd() Exception Break");
}
void CPacketTranslater::OnInnerPacketLogin(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
        {
            DNF_LOG_SCOPE_LINE(0xbfa,
                "./log/Except",
                "CPacketTranslater::OnInnerPacketLogin : 0 == m_pclApp"
            );

            return;
        }
        Packet_InnerPakcet_Login* pkt =
            (Packet_InnerPakcet_Login*)header;
            DNF_LOG_SCOPE_LINE(0xc00,
                "./log/TcpServer",
                "CPacketTranslater::OnInnerPacketLogin (sock:%d)",
            pkt->reversed2
        );

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
            DNF_LOG_SCOPE_LINE(0xc14,
                "./log/Except",
                "CPacketTranslater::OnInnerPacketLogout : 0 == m_pclApp"
            );

            return;
        }
        Packet_InnerPakcet_Logout* pkt = (Packet_InnerPakcet_Logout*)header;
        CTcpServer* server =
            m_pclApp->Get_ServerHandler()->GetTcpServer(pkt->reversed2);
        if (!server)
        {
            DNF_LOG_SCOPE_LINE(0xc1f,
                "./log/TcpServer",
                "CPacketTranslater::OnInnerPacketLogout Invalid Server Instance(sock:%d)",
                pkt->reversed2
            );

            return;
        }
        unsigned char idx = server->GetServerType();
        if (!m_pclApp->Get_ServerHandler()->DeleteTcpServer(idx))
        {
            DNF_LOG_SCOPE_LINE(0xc27,
                "./log/TcpServer",
                "CPacketTranslater::OnInnerPacketLogout DeleteTcpServer fail(sock:%d)",
                pkt->reversed2
            );

            return;
        }
        DNF_LOG_SCOPE_LINE(0xc2b,
            "./log/TcpServer",
            "CPacketTranslater::OnInnerPacketLogout DeleteTcpServer Success(TYPE:%d, sock:%d)", idx,
            pkt->reversed2
        );

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
        register unsigned char idx = pkt->m_idx;
        register int port = (int)pkt->reversed2;
        CServerHandler* handler = m_pclApp->Get_ServerHandler();
        if (handler->GetTcpServer(idx))
        {
            DNF_LOG_SCOPE_LINE(0xc44,
                "./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogin Duplicate Server Instance(TYPE:%d, sock:%d)",
                idx, port
            );

            return;
        }
        handler = m_pclApp->Get_ServerHandler();
        CTcpServer* ret = handler->CreateTcpServer(idx, port);
        if (ret == 0)
        {
            DNF_LOG_SCOPE_LINE(0xc4d,
                "./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogin CreateTcpServer fail(TYPE:%d, sock:%d)\n",
                idx, port
            );

            return;
        }
        printf("CPacketTranslater::OnTcpServerLogin(TYPE:%d, sock:%d)\n", idx,
               port);
        DNF_LOG_SCOPE_LINE(0xc5b,
            "./log/TcpServer",
            "CPacketTranslater::OnTcpServerLogin(TYPE:%d, sock:%d)", idx,
            port
        );

    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnTcpServerLogin Exception Break",
                  0xc5f, 0xc64);
}
void CPacketTranslater::OnTcpServerLogout(PacketHeader* header)
{
    try
    {
        if (!m_pclApp)
            return;
        Packet_Tcp_Server_Logout* pkt = (Packet_Tcp_Server_Logout*)header;
        if (!m_pclApp->Get_ServerHandler()->GetTcpServer(pkt->m_idx))
        {
            DNF_LOG_SCOPE_LINE(0xc74,
                "./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogout Invalid Server Instance(TYPE:%d, sock:%d)", pkt->m_idx,
                pkt->reversed2
            );

            return;
        }
        if (!m_pclApp->Get_ServerHandler()->DeleteTcpServer(pkt->m_idx))
        {
            DNF_LOG_SCOPE_LINE(0xc7a,
                "./log/TcpServer",
                "CPacketTranslater::OnTcpServerLogout DeleteTcpServer fail(TYPE:%d, sock:%d)", pkt->m_idx,
                pkt->reversed2
            );

            return;
        }
        printf("CPacketTranslater::OnTcpServerLogout(TYPE:%d, sock:%d)", pkt->m_idx, pkt->reversed2);
        DNF_LOG_SCOPE_LINE(0xc7e,
            "./log/TcpServer",
            "CPacketTranslater::OnTcpServerLogout(TYPE:%d, sock:%d)", pkt->m_idx,
            pkt->reversed2
        );

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
        CTcpServer* server =
            m_pclApp->Get_ServerHandler()->GetTcpServer(pkt->m_idx);
        if (!server)
        {
            DNF_LOG_SCOPE_LINE(0xc98,
                "./log/TcpServer",
                "CPacketTranslater::OnTcpServerHeartbeat Invalid Server Instance(TYPE:%d, sock:%d)", pkt->m_idx,
                pkt->reversed2
            );

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
        DNF_LOG_SCOPE_LINE(0x2ae,
            "./log/Web",
            "OnWebNoticeInGameAD() packet_id(%d)\n",
            header->packetId
        );

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
        DNF_LOG_SCOPE_LINE(0x1b2,
            "./log/test",
            "%d, %s, %d, %s\n", ((char*)header)[0xa],
            (char*)header + 0xb,
            len,
            (char*)header + 0x10b
        );

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
        Packet_DB_Request_Member_Delete* pkt =
            (Packet_DB_Request_Member_Delete*)header;
        m_pclApp->m_dbManager.OnMemberDeleteAsCharDelete(
            pkt->m_characNo);
    }
    DNF_CATCH_LOG_PRINTF("./log/Except.log",
                         "CPacketTranslater::OnMemberDeleteAsCharDelete() Exception Break",
                         0x189, 0x18f);
}
void CPacketTranslater::OnSendHWspec(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_DBMW_Save_Client_Spec_Statistic* pkt =
            (Packet_DBMW_Save_Client_Spec_Statistic*)header;
        m_pclApp->m_dbManager.QueryHWspecCreate(pkt);
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
        Packet_UnChangable_GuildInfo_Save* pkt = (Packet_UnChangable_GuildInfo_Save*)header;
        m_pclApp->m_dbManager.SaveUnchangableGuildInfo(pkt);
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
        Packet_DBMW_Save_Error_Line_Statistic* pkt = (Packet_DBMW_Save_Error_Line_Statistic*)header;
        m_pclApp->m_dbManager.QueryErrorLineStatisticCreate(pkt);
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
        Packet_DBMW_Dungeon_Statistic_Party* pkt = (Packet_DBMW_Dungeon_Statistic_Party*)header;
        m_pclApp->m_dbManager.QueryPartyStatisticCreate(pkt);
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
        Packet_DBMW_Dungeon_Statistic_Party_Job* pkt = (Packet_DBMW_Dungeon_Statistic_Party_Job*)header;
        m_pclApp->m_dbManager.QueryPartyJobStatisticCreate(pkt);
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
        Packet_DBMW_Dungeon_Statistic_Party_Charac* pkt = (Packet_DBMW_Dungeon_Statistic_Party_Charac*)header;
        m_pclApp->m_dbManager.QueryPartyCharacStatisticCreate(pkt);
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
        Packet_DBMW_DeathTower_Statistic_Value* pkt = (Packet_DBMW_DeathTower_Statistic_Value*)header;
        m_pclApp->m_dbManager.QueryDeathTowerValueStatisticCreate(pkt);
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
        Packet_DBMW_DeathTower_Statistic_Playdata_Job* pkt = (Packet_DBMW_DeathTower_Statistic_Playdata_Job*)header;
        m_pclApp->m_dbManager.QueryDeathTowerPlayDataJobStatisticCreate(pkt);
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
        Packet_DBMW_DeathTower_Statistic_Playdata_Party* pkt = (Packet_DBMW_DeathTower_Statistic_Playdata_Party*)header;
        m_pclApp->m_dbManager.QueryDeathTowerPlayDataPartyStatisticCreate(pkt);
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
        Packet_DBMW_HellParty_Statistic_Item* pkt = (Packet_DBMW_HellParty_Statistic_Item*)header;
        m_pclApp->m_dbManager.QueryHellPartyStatisticItemCreate(pkt);
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
        Packet_DBMW_Cube_Statistic* pkt = (Packet_DBMW_Cube_Statistic*)header;
        m_pclApp->m_dbManager.QueryCubeStatisticCreate(pkt);
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnCubeStatisticDataSave() Exception Break",
                  0x7da, 0x7df);
}
void CPacketTranslater::OnQueryFirstLoadSpecDb(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_Frame_Lag_Statistic_Load_Spec* pkt =
            (Packet_Frame_Lag_Statistic_Load_Spec*)header;
        m_pclApp->m_dbManager.QueryFirstLoadSpecDb(
            pkt, m_pclApp->m_serverHandler->GetStatisticsServerPtr());
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
    try
    {
        Packet_Frame_Lag_Statistic_Reload_Spec* pkt =
            (Packet_Frame_Lag_Statistic_Reload_Spec*)header;
        m_pclApp->m_dbManager.QueryReloadSpecDb(
            pkt, m_pclApp->m_serverHandler->GetStatisticsServerPtr());
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
    try
    {
        Packet_Frame_Lag_Statistic_Write_Lag_Index* pkt =
            (Packet_Frame_Lag_Statistic_Write_Lag_Index*)header;
        m_pclApp->m_dbManager.InsertFrameLagStatistics(
            pkt, m_pclApp->m_serverHandler->GetStatisticsServerPtr());
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
        Packet_Frame_Lag_Statistic_Write_Query* pkt = (Packet_Frame_Lag_Statistic_Write_Query*)header;
        m_pclApp->m_dbManager.SunAhWriteQuery(pkt);
    }
    DNF_CATCH_LOG2("./log/Except.log",
                   "CPacketTranslater::OnInsertDirectxVersiongStatistics Exception Break",
                   "CPacketTranslater::OnInsertDirectxVersiongStatistics() Exception Break",
                   0x838, 0x83d);
}
void CPacketTranslater::OnInsertDailyBadSpecStatistics(PacketHeader* header)
{
    if (m_pclApp)
    {
        char buf[0x14];
        PacketHeader* p = header;
    }
    else
    {
        return;
    }
}
void CPacketTranslater::OnInsertUsedMemoryStatistic(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        Packet_Frame_Lag_Used_Memory_Write_Query* pkt = (Packet_Frame_Lag_Used_Memory_Write_Query*)header;
        m_pclApp->m_dbManager.OnSaveUsedMemoryWriteQuery(pkt);
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
        Packet_DBMW_Assert_Manager_Info_Write_Query* pkt = (Packet_DBMW_Assert_Manager_Info_Write_Query*)header;
        m_pclApp->m_dbManager.OnSaveAssertManagerInfoWrite(pkt);
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
        Packet_DBMW_Packet_Overflow_Statistic* pkt = (Packet_DBMW_Packet_Overflow_Statistic*)header;
        m_pclApp->m_dbManager.OnSavePacketOverflowWrite(pkt);
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
        Packet_DB_Write_Guild_Member_Memo* pkt = (Packet_DB_Write_Guild_Member_Memo*)header;
        m_pclApp->m_dbManager.OnWriteGuildMemberMemo(pkt);
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
        Packet_DBMW_User_Ting_TimeCheck_Write_Query* pkt = (Packet_DBMW_User_Ting_TimeCheck_Write_Query*)header;
        m_pclApp->m_dbManager.OnSaveUserTingTimeCheckWrite(pkt);
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
        Packet_DBMW_Ting_User_TimeCheck_Write_Query* pkt = (Packet_DBMW_Ting_User_TimeCheck_Write_Query*)header;
        m_pclApp->m_dbManager.OnSaveTingUserAccount(pkt);
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
        Packet_DBMW_Powerwar_Loading_Time_Report* pkt = (Packet_DBMW_Powerwar_Loading_Time_Report*)header;
        m_pclApp->m_dbManager.OnSavePowerwarLoadingReport(pkt);
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
        Packet_DBMW_Powerwar_Lag_Report* pkt = (Packet_DBMW_Powerwar_Lag_Report*)header;
        m_pclApp->m_dbManager.OnSavePowerwarLagReport(pkt);
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
        Packet_DBMW_Reason_Crash_Down_Query* pkt = (Packet_DBMW_Reason_Crash_Down_Query*)header;
        m_pclApp->m_dbManager.OnReasonCrashDownQueryWrite(pkt);
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
        Packet_DBMW_Statistic_Login_Logout* pkt = (Packet_DBMW_Statistic_Login_Logout*)header;
        m_pclApp->m_dbManager.OnStatisticNumOfOccupations(pkt);
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
        Packet_DBMW_TechnicalReport_Common_Query* pkt = (Packet_DBMW_TechnicalReport_Common_Query*)header;
        m_pclApp->m_dbManager.OnTechnicalReportCommonQuery(pkt);
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
        Packet_Goldcard_Event_Statistic_STD* pkt = (Packet_Goldcard_Event_Statistic_STD*)header;
        m_pclApp->m_dbManager.OnGoldcardEventStatistic(pkt);
    }
    DNF_CATCH_LOG("./log/Except",
                  "CPacketTranslater::OnGoldcardEventStatistic() Exception Break",
                  0xf36, 0xf3b);
}
void CPacketTranslater::OnRenew_GM_List(PacketHeader* header)
{
    Packet_GM_List* pkt = (Packet_GM_List*)header;
    WongWork::CGMAccounts* gm =
        (WongWork::CGMAccounts*)m_pclApp->GetGMAccounts();
    if (gm && pkt)
    {
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
        if (!m_pclApp->m_dbManager.QueryUpdateChannelOccNum(
                pkt))
        {
            if (pkt)
            {
                DNF_LOG_SCOPE_LINE(0x1300,
                    "./log/statistic",
                    "CPacketTranslater::OnUpdateChannelOccNum Error ch_no(%d), count(%d)",
                    pkt->m_gcNo,
                    pkt->m_userCount
                );
            }
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
        PacketInsertUpdate* pkt = (PacketInsertUpdate*)header;
        if (!m_pclApp->m_dbManager.QueryInsertUpdate(pkt))
        {
            if (pkt)
            {
                DNF_LOG_SCOPE_LINE(0x1318,
                    "./log/statistic",
                    "CPacketTranslater::PacketInsertUpdate Error"
                );
            }
        }
    }
    DNF_CATCH_LOG_CDNF("./log/Except.log",
                       "CPacketTranslater::PacketInsertUpdate() Exception Break",
                       0x131d);
}
