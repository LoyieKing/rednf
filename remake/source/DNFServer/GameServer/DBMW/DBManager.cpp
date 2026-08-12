// df_dbmw_r - DBManager (ORIG DBManager.cpp)
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

#include "PacketNameTables.h"

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();
int getNotiPacketNameCount();
int getCmdPacketNameCount();
int parse_string(std::vector<std::string>& v, std::string& s, char c);
StackBuffer_char sformat(const char* fmt, ...);
CQueryCounter* CQueryCounterInstance();
char isDayTimeOver(unsigned int timestamp, unsigned int days);
int get_day_interval(struct tm* a, struct tm* b);

// ---- 本地 view 结构（仅用于生成与 ORIG 一致的成员访问形态） ----
struct UpgradeSeparateBits
{
    unsigned char b0 : 5;
    unsigned char b1 : 1;
    unsigned char b2 : 2;
};

struct GuildCargoUpgradeView
{
    char h[0xa];
    unsigned int m_fieldA;    // +0xa
    unsigned int m_fieldE;    // +0xe
    unsigned int m_field12;   // +0x12
} __attribute__((packed));

struct FatigueBatteryEntry
{
    int m_fieldA;             // +0
    int m_fieldE;             // +4
};

struct FatigueBatteryView
{
    char h[0xa];
    FatigueBatteryEntry m_entries[0x65];
} __attribute__((packed));

struct QueryMsgView
{
    char h[0xa];
    unsigned int m_fieldA;    // +0xa
    unsigned int m_fieldE;    // +0xe
    char m_data[0x1001];      // +0x12
} __attribute__((packed));

struct QueryStringView
{
    char h[0xa];
    unsigned int m_queryId;   // +0xa
    char m_data[0x1001];      // +0xe
} __attribute__((packed));

// ---- CGuildManager / WongWork ----
bool CDBManager::GuildMasterDelegate(int serverId,
                                     unsigned int guildId,
                                     unsigned int oldMasterNo,
                                     unsigned int newMasterMId,
                                     unsigned int newMasterNo,
                                     char* newMasterName)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (newMasterName[0] == 0)
    {
        CMyFileLog log(__FUNCTION__, 0x10b3);
        log("./log/TraceGuildErr",
            "CDBManager::GuildMasterDelegate server_group(%d), guild(%d), charac_no(%d)\n",
            serverId, guildId, newMasterNo);
        return 0;
    }
    if (!ChangeGuildMemberGrade((unsigned char)serverId, guildId, 3,
                                oldMasterNo))
        return 0;
    if (!ChangeGuildMemberGrade((unsigned char)serverId, guildId, 1,
                                newMasterNo))
        return 0;
    if (!h->set_query(0x4e6a,
                      "upDate guild_info set master_id=%s, master_no=%d, master_name='%s' where guild_id = %d and server_id= %d",
                      NumberToString(newMasterMId, 0), newMasterNo,
                      newMasterName, guildId, serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x10da);
        log("./log/DBQueryErr",
            "CDBManager::GuildMasterDelegate() set : upDate guild_info set master_id=%s, master_no=%d, master_name='%s' where guild_id = %d and server_id= %d",
            NumberToString(newMasterMId, 0), newMasterNo, newMasterName,
            guildId, serverId);
        return 0;
    }
    if (!h->exec(0x4e6a))
    {
        CMyFileLog log(__FUNCTION__, 0x10e7);
        log("./log/DBQueryErr",
            "CDBManager::GuildMasterDelegate() exec : upDate guild_info set master_id=%s, master_no=%d, master_name='%s' where guild_id = %d and server_id= %d",
            NumberToString(newMasterMId, 0), newMasterNo, newMasterName,
            guildId, serverId);
        return 0;
    }
    return 1;
}
bool CDBManager::SendGuildLetter(int serverId, unsigned int guildId, char* msg)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e39,
                      "seLect charac_no from guild_member where guild_id = %d and server_id = %d and member_flag = 1",
                      guildId, serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x7d3);
        log("./log/DBQueryErr",
            "CDBManager::AwardGuildTitleByMail() select charac_no from guild_member where server_id = %d and guild_id = %d and member_flag = 1\n",
            serverId, guildId);
        return 0;
    }
    if (!h->exec(0x4e39))
        return 0;
    int n = h->get_n_rows();
    time_t now = time(0);
    struct tm* lt = localtime(&now);
    lt->tm_hour += 1;
    lt->tm_min = 0;
    lt->tm_sec = 0;
    long expiry = mktime(lt);
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
        unsigned int characNo = 0;
        if (!h->get_uint(0, characNo))
            return 0;
        char subject[0x1e] = {0};
        std::string s = g_ServerString_.GetServerString(0x431, 0);
        strncpy(subject, s.c_str(), 0x1d);
        int letterNo = 0;
        if (!InsertLetter(characNo, 0, subject, msg, letterNo, expiry))
        {
            CMyFileLog log(__FUNCTION__, 0x80d);
            log("./log/Postal", "InsertLetter Err");
            return 0;
        }
    }
    return 1;
}
char CDBManager::OnWriteGuildBoard(
    Packet_DB_Load_Request_Guild_Board_Write* req, STGuildBoardDBInfo* info)
{
    CDBHandle* h = m_handles[5];    // web db
    char* r = (char*)req;
    if (*(unsigned int*)(r + 0xf) == 0)
        memset(r + 0x9e, 0, 0x1e);
    h->set_query(0x4f08,
                 "inSert into guild_memo set guild_id=%u, m_id=%s, charac_no=%u, charac_name='%s', memo='%s', create_time=now(), job=%d, grow_type=%d",
                 *(unsigned int*)(r + 0xb),
                 NumberToString(*(unsigned int*)(r + 0xf), 0),
                 *(unsigned int*)(r + 0x13), r + 0x9e,
                 h->blob_to_str(0, r + 0x17, 0x78),
                 *(char*)(r + 0x9b), *(char*)(r + 0x9c));
    if (!h->exec(0x4f08))
    {
        CMyFileLog log(__FUNCTION__, 0x22fc);
        log("./log/DBQueryErr", "OnWriteGuildBoard Query Error");
        return 0;
    }
    h->set_query(0x4f07,
                 "seLect no, unix_timestamp(create_time) from guild_memo where no=LAST_INSERT_ID()");
    if (!h->exec(0x4f07))
    {
        CMyFileLog log(__FUNCTION__, 0x2306);
        log("./log/DBQueryErr", "OnWriteGuildBoard Query Error");
        return 0;
    }
    if (!h->fetch())
        return 0;
    if (!h->get_uint(0, *(unsigned int*)((char*)info + 0x7c)))
        return 0;
    if (!h->get_uint(1, *(unsigned int*)((char*)info + 0x78)))
        return 0;
    if (*(unsigned int*)(r + 0xf) == 0)
    {
        *(int*)((char*)info + 0x80) = 0;
    }
    else
    {
        *(unsigned int*)((char*)info + 0x80) = *(unsigned int*)(r + 0x13);
        *(char*)((char*)info + 0x84) = *(char*)(r + 0x9b);
        memcpy((char*)info + 0x87, r + 0x9e, 0x1e);
        memcpy(info, r + 0x17, 0x78);
    }
    return 1;
}
char CDBManager::OnWriteWebGuildBoard(
    Packet_DB_Load_Request_Web_Guild_Board_Write* req,
    STGuildBoardDBInfo* info)
{
    CDBHandle* h = m_handles[5];    // web db
    char* r = (char*)req;
    h->set_query(0x4f07,
                 "seLect no, charac_no, charac_name, memo, unix_timestamp(create_time), job from guild_memo where no=%u",
                 *(unsigned int*)(r + 0x12));
    if (!h->exec(0x4f07))
    {
        CMyFileLog log(__FUNCTION__, 0x2329);
        log("./log/DBQueryErr", "OnWriteWebGuildBoard Query Error");
        return 0;
    }
    if (!h->fetch())
        return 0;
    if (!h->get_uint(0, *(unsigned int*)((char*)info + 0x7c)))
        return 0;
    if (!h->get_uint(1, *(unsigned int*)((char*)info + 0x80)))
        return 0;
    if (!h->get_binary(2, (char*)info + 0x87, 0x1e))
        return 0;
    if (!h->get_binary(3, info, 0x78))
        return 0;
    if (!h->get_uint(4, *(unsigned int*)((char*)info + 0x78)))
        return 0;
    if (!h->get_byte(5, *(char*)((char*)info + 0x84)))
        return 0;
    return 1;
}
char CDBManager::OnDeleteGuildBoard(unsigned int no)
{
    bool ret;
    CDBHandle* h = m_handles[5];    // web db
    h->set_query(0x4f09, "deLete from guild_memo where no=%u", no);
    ret = h->exec(0x4f09);
    if (!ret)
    {
        CMyFileLog log(__FUNCTION__, 0x235b);
        log("./log/DBQueryErr", "OnDeleteGuildBoard Query Error");
        return 0;
    }
    return 1;
}
bool CDBManager::OnLoadGuildAgit(Packet_DB_Load_Guild_Agit* req,
                                 Packet_Guild_Load_Guild_Agit& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    h->set_query(0x4eb5,
                 "seLect upgrade from guild_agit where guild_id=%d",
                 req->m_guildId);
    if (!h->exec(0x4eb5))
    {
        CMyFileLog log(__FUNCTION__, 0x19c3);
        log("./log/DBQueryErr", "OnLoadGuildAgit Query Error\n");
        return 0;
    }
    if (!h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0x19cb);
        log("./log/DBQueryErr", "OnLoadGuildAgit Fetch Error\n");
        return 0;
    }
    if (!h->get_ubyte(0, *(unsigned char*)((char*)&reply + 0xe)))
    {
        CMyFileLog log(__FUNCTION__, 0x19d3);
        log("./log/DBQueryErr",
            "OnLoadGuildAgit get_ubyte(0, reply.m_stGuildAgitInfo.m_ucUpgrade) Error\n");
        return 0;
    }
    return 1;
}
char CDBManager::OnLoadGuildCargo(unsigned int guildId,
                                  Packet_Guild_Load_Guild_Cargo& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4ed7,
                      "seLect cargo_capacity,cargo from guild_agit where guild_id=%d",
                      guildId))
    {
        CMyFileLog log(__FUNCTION__, 0x1b35);
        log("./log/DBQueryErr", "OnLoadGuildCargo Query Error");
        return 0;
    }
    if (!h->exec(0x4ed7))
        return 0;
    if (!h->fetch())
        return 0;
    int col = 0;
    if (!h->get_uint(col++, *(unsigned int*)((char*)&reply + 0x18e6)))
        return 0;
    if (!h->get_str(col++, (char*)&reply + 0xe, 0x18d8))
        return 0;
    return 1;
}
char CDBManager::OnCreateGuildAgit(Packet_DB_Create_Guild_Agit* req,
                                   Packet_DB_Create_Guild_Agit_Reply& reply)
{
    bool ret;
    CDBHandle* h = m_handles[8];    // guild db
    h->set_query(0x4eae,
                 "inSert into guild_agit set guild_id=%d, upgrade=1, cargo_capacity=8",
                 req->m_guildId);
    ret = h->exec(0x4eae);
    if (!ret)
    {
        reply.m_field12 = 2;
        CMyFileLog(__FUNCTION__, 0x1975)("./log/DBQueryErr",
            "inSert_into_guild_Agit Query Error\n");
        return 0;
    }
    h->set_query(0x4eb4,
                 "upDate guild_info set guild_agit_flag=1 where guild_id=%d",
                 req->m_guildId);
    ret = h->exec(0x4eb4);
    if (!ret)
    {
        reply.m_field12 = 2;
        CMyFileLog(__FUNCTION__, 0x1984)("./log/DBQueryErr",
            "upDate_into_guild_info_guild_agit_flag Query Error\n");
        return 0;
    }
    reply.m_field12 = 0;
    return 1;
}
char CDBManager::OnInsertGuildCargoHistory(
    Packet_DB_Insert_Guild_Cargo_History* req)
{
    CDBHandle* h = m_handles[8];    // guild db
    char* r = (char*)req;
    char table[0x100] = {0};
    sprintf(table, "guild_cargo_history_%d", *(unsigned int*)(r + 0xb) % 10);
    h->set_query(
        0x4ed9,
        "inSert into %s(occ_time,guild_id,server_id,charac_no,charac_name,slot_no,moveto_slot_no,behavior,seal_flag,item_id,add_info,endurance,extend_info,upgrade,seal_cnt,amplify_option,amplify_value,random_option,separate) values(unix_timestamp(now()),%d,%d,%d,'%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d)",
        table, *(unsigned int*)(r + 0xb),
        *(unsigned char*)(r + 0xa), *(unsigned int*)(r + 0xf),
        r + 0x13, *(unsigned int*)(r + 0x29),
        *(unsigned int*)(r + 0x2d), *(signed char*)(r + 0x28),
        *(unsigned char*)(r + 0x31), *(unsigned int*)(r + 0x32),
        *(unsigned int*)(r + 0x37), *(unsigned short*)(r + 0x3b),
        *(unsigned int*)(r + 0x3d), *(unsigned int*)(r + 0x36) & 0x1f,
        (*(unsigned int*)(r + 0x36) >> 5) & 1,
        *(unsigned char*)(r + 0x41), *(unsigned short*)(r + 0x42),
        h->blob_to_str(0, r + 0x4e, 0xe),
        ((UpgradeSeparateInfo*)(r + 0x5c))->GetUpgradeSeparate());
    if (!h->exec(0x4ed9))
    {
        CMyFileLog log(__FUNCTION__, 0x1bd3);
        log("./log/DBQueryErr", "OnInsertGuildCargoHistory Query Error");
        return 0;
    }
    return 1;
}
bool CDBManager::DeleteToBlackListOnly(unsigned int m_id, char* name)
{
    bool ret;
    CDBHandle* h = m_handles[3];    // game db
    h->set_query(0x4e40,
                 "deLete from charac_black_list where m_id = %u and charac_name = '%s'",
                 m_id, name);
    ret = h->exec(0x4e40);
    if (!ret)
        return 0;
    return 1;
}
bool CDBManager::QueryBlackList(unsigned int m_id, STBlackUserDBType* list)
{
    CDBHandle* h = m_handles[3];    // game db
    if (!h->set_query(0x4e44,
                      "seLect charac_no, charac_name, unix_timestamp(occ_time) from  charac_black_list where m_id = %s limit %d",
                      NumberToString(m_id, 0), 0xa))
    {
        CMyFileLog log(__FUNCTION__, 0xaac);
        log("./log/DBQueryErr",
            "CDBManager::QueryCharacNoByName() seLect charac_no, charac_name, occ_time from  charac_black_list where m_id = %s",
            NumberToString(m_id, 0));
        return 0;
    }
    if (!h->exec(0x4e44))
        return 0;
    int n = h->get_n_rows();
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
        if (!h->get_uint(0, *(unsigned int*)((char*)list + i * 0x28)))
            return 0;
        if (!h->get_str(1, (char*)list + i * 0x28 + 0x4, 0x1e))
            return 0;
        if (!h->get_uint(2, *(unsigned int*)((char*)list + i * 0x28 + 0x24)))
            return 0;
    }
    return 1;
}
char CDBManager::SaveGuildSkill(unsigned char serverGroup,
                                unsigned int guildId,
                                STGuildDBInfoOnly& info)
{
    CDBHandle* h = m_handles[8];    // guild db
    h->set_query(0x4e55,
                 "upDate guild_skill set remain_sp = %d, used_sp = %d, skill_slot = '%s' where guild_id = %d",
                 *(unsigned short*)((char*)&info + 0x42),
                 *(unsigned char*)((char*)&info + 0x44),
                 h->blob_to_str(0, (char*)&info + 0x45,
                                *(unsigned char*)((char*)&info + 0x44) * 5),
                 guildId);
    if (h->exec(0x4e55) != 1 || h->getAffectedRowCount() == 0)
    {
        h->set_query(0x4e59,
                     "inSert into guild_skill set guild_id= %d, remain_sp = %d, used_sp = %d, skill_slot = '%s'",
                     guildId, *(unsigned short*)((char*)&info + 0x42),
                     *(unsigned char*)((char*)&info + 0x44),
                     h->blob_to_str(0, (char*)&info + 0x45,
                                    *(unsigned char*)((char*)&info + 0x44) * 5));
        h->exec(0x4e59);
    }
    return 1;
}
char CDBManager::SaveGuildMember(unsigned char serverGroup,
                                 unsigned int guildId,
                                 STGuildMemerDBInfo& info,
                                 unsigned int flag, unsigned char type)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (type <= 2)
    {
        h->set_query(0x4e30,
                     "upDate guild_member set member_point=%d, last_play_time =  now() where guild_id = %d and server_id = %d and charac_no = %d",
                     *(int*)((char*)&info + 0x16), guildId, serverGroup, flag);
        CMyFileLog log(__FUNCTION__, 0x2ba);
        log("./log/GuildModify",
            "CDBManager::SaveGuildMember(SAVE_LOGOUT flag(%d), grade(%d), guildMemPoint(%d), g(%d), s(%d), c(%d))",
            type, *(unsigned char*)((char*)&info + 0x15),
            *(int*)((char*)&info + 0x16), guildId, serverGroup, flag);
    }
    else if (type == 3)
    {
        h->set_query(0x4e30,
                     "upDate guild_member set member_point=%d, grade=%d where guild_id = %d and server_id = %d and charac_no = %d",
                     *(int*)((char*)&info + 0x16),
                     *(unsigned char*)((char*)&info + 0x15), guildId,
                     serverGroup, flag);
        CMyFileLog log(__FUNCTION__, 0x2c5);
        log("./log/GuildModify",
            "CDBManager::SaveGuildMember(SAVE_LOGOUT flag(%d), grade(%d), guildMemPoint(%d), g(%d), s(%d), c(%d))",
            type, *(unsigned char*)((char*)&info + 0x15),
            *(int*)((char*)&info + 0x16), guildId, serverGroup, flag);
    }
    else
    {
        CMyFileLog log(__FUNCTION__, 0x2c9);
        log("./log/GuildModify",
            "CDBManager::SaveGuildMember ERR(save_flag err(%d))", type);
    }
    if (!h->exec(0x4e30))
        return 0;
    return 1;
}
bool CDBManager::SaveGuildWarPointList(int serverId,
                                       unsigned int* guildIds,
                                       unsigned int* points)
{
    if (guildIds == 0 && points == 0)
        return 0;
    CDBHandle* h = m_handles[8];    // guild db
    for (int i = 0; i <= 9; i++)
    {
        if (guildIds[i] == 0)
            continue;
        if (!h->set_query(0x4e3c,
                          "upDate guild_info set guild_war_point = %d where server_id = %d and expire_flag = 0 and guild_id = %d",
                          points[i], serverId, guildIds[i]))
        {
            CMyFileLog log(__FUNCTION__, 0x9b7);
            log("./log/DBQueryErr",
                "CDBManager::SaveGuildWarPointList() update guild_info set guild_war_point = %d where server_id = %d and expire_flag = 0 and guild_id = %d",
                points[i], serverId, guildIds[i]);
            return 0;
        }
        if (!h->exec(0x4e3c))
            return 0;
    }
    return 1;
}
char CDBManager::OnSavePowerWarBonusPoint(
    Packet_DB_Save_Power_War_Bonus_Point* packet)
{
    CDBHandle* h = m_handles[3];    // game db
    char* p = (char*)packet;
    time_t now = time(0);
    struct tm* lt = localtime(&now);
    lt->tm_hour += 1;
    lt->tm_min = 0;
    lt->tm_sec = 0;
    long occTime = mktime(lt);
    std::string name("\xbc\xbc\xb7\xc2\xc0\xfc \xc6\xf7\xc0\xce\xc6\xae");
    int itemId = 0x4df;
    for (int i = 0; i < *(int*)(p + 0xa); i++)
    {
        if (!h->set_query(0x4ef7,
                          "inSert into postal (occ_time, send_charac_no, receive_charac_no, seal_flag, item_id, add_info, endurance, upgrade, gold, send_charac_name) values (from_unixtime(%d), %d, %d, %d, %d, %d, %d, %d, %d,'%s')",
                          occTime, 0, *(int*)(p + 0xe + i * 8), 0, itemId,
                          *(int*)(p + 0x12 + i * 8), 0, 0, 0, name.c_str()))
        {
            CMyFileLog log(__FUNCTION__, 0x2172);
            log("./log/DBQueryErr",
                "CDBManager::OnSavePowerWarBonusPoint() : insert into postal (occ_time, send_charac_no, receive_charac_no, seal_flag, item_id, add_info, endurance, upgrade, gold, send_charac_name ) values ( from_unixtime( now() ), %d, %d, %d, %d, %d, %d, %d, %d,'%s')\n",
                0, *(int*)(p + 0xe + i * 8), 0, itemId,
                *(int*)(p + 0x12 + i * 8), 0, 0, 0, name.c_str());
            return 0;
        }
        if (!h->exec(0x4ef7))
            return 0;
    }
    return 1;
}
char CDBManager::SavePowerWarPoint(Packet_DB_Save_Power_War_Point* packet)
{
    CDBHandle* h = m_handles[8];    // guild db
    char* p = (char*)packet;
    if (!h->set_query(0x4e81,
                      "upDate power_war set a_side_point=%d, b_side_point=%d, winner_side=%d where server_id = %d",
                      *(unsigned int*)(p + 0xc), *(unsigned int*)(p + 0x10),
                      *(signed char*)(p + 0xb), *(unsigned char*)(p + 0xa)))
    {
        CMyFileLog log(__FUNCTION__, 0x12ce);
        log("./log/DBQueryErr",
            "CDBManager::SavePowerWarPoint() : upDate power_war set a_side_point=%d, b_side_point=%d, winner_side=%d where server_id = %d",
            *(unsigned int*)(p + 0xc), *(unsigned int*)(p + 0x10),
            *(signed char*)(p + 0xb), *(unsigned char*)(p + 0xa));
        return 0;
    }
    if (h->exec(0x4e81) != 1 || h->getAffectedRowCount() == 0)
    {
        if (!h->set_query(0x4e82,
                          "inSert into power_war set a_side_point=%d, b_side_point=%d, winner_side=%d ,server_id = %d",
                          *(unsigned int*)(p + 0xc),
                          *(unsigned int*)(p + 0x10),
                          *(signed char*)(p + 0xb),
                          *(unsigned char*)(p + 0xa)))
            return 0;
        if (!h->exec(0x4e82))
            return 0;
    }
    return 1;
}
char CDBManager::OnSavePowerWarStatueRanker(
    Packet_DB_Save_Power_War_Statue_Ranker* packet)
{
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[6];   // sso db
    char* p = (char*)packet;
    unsigned char serverId = *(unsigned char*)(p + 0xa);
    h2->set_query(0x4ecc,
                  "deLete from event_server_message where server_info = %d and message_index in (1, 2, 3)",
                  serverId);
    if (h2->exec(0x4ecc) != 1)
    {
        CMyFileLog log(__FUNCTION__, 0x1943);
        log("./log/DBQueryErr", "deLete_power_war_statue_message Query Error\n");
    }
    h->set_query(0x4ead,
                 "upDate power_war_statue_ranker set first_ranker=%d, second_ranker=%d, third_ranker=%d where server_id=%d",
                 *(unsigned int*)(p + 0xb), *(unsigned int*)(p + 0xf),
                 *(unsigned int*)(p + 0x13), serverId);
    if (h->exec(0x4ead) != 1 || h->getAffectedRowCount() == 0)
    {
        if (!h->set_query(0x4eac,
                          "inSert into power_war_statue_ranker set first_ranker=%d, second_ranker=%d, third_ranker=%d, server_id=%d",
                          *(unsigned int*)(p + 0xb),
                          *(unsigned int*)(p + 0xf),
                          *(unsigned int*)(p + 0x13), serverId))
            return 0;
        if (!h->exec(0x4eac))
        {
            CMyFileLog log(__FUNCTION__, 0x195a);
            log("./log/DBQueryErr",
                "inSert_into_power_war_statue_ranker Query Error\n");
            return 0;
        }
    }
    return 1;
}
bool CDBManager::OnSavePowerWarPointReward(
    Packet_DB_Save_Power_War_Point_Reward* packet)
{
    CDBHandle* h = m_handles[8];    // guild db
    unsigned char serverId = *(unsigned char*)((char*)packet + 0xa);
    int count = *(int*)((char*)packet + 0xb);
    for (int i = 0; i < count; i++)
    {
        int p1 = *(int*)((char*)packet + 0xf + i * 8);
        int p2 = *(int*)((char*)packet + 0x13 + i * 8);
        h->set_query(0x4eab,
                     "upDate guild_info set power_war_point=power_war_point+%d where guild_id=%d and server_id=%d and expire_flag=0",
                     p2, p1, serverId);
        bool ret = h->exec(0x4eab);
        if (!ret)
        {
            CMyFileLog log(__FUNCTION__, 0x192b);
            log("./log/DBQueryErr",
                "upDate_into_guild_info_power_war_point Query Error\n");
            return 0;
        }
    }
    return 1;
}
bool CDBManager::InsertMail(unsigned int characNo, char* subject,
                            char* content, unsigned int hE,
                            unsigned int h12, int h16, int h17)
{
    time_t now = time(0);
    struct tm* lt = localtime(&now);
    lt->tm_hour += 1;
    lt->tm_min = 0;
    lt->tm_sec = 0;
    long occTime = mktime(lt);
    int letterNo = 0;
    if (!InsertLetter(characNo, (unsigned int)subject, content, 0, letterNo,
                      occTime))
    {
        CMyFileLog log(__FUNCTION__, 0x1d9e);
        log("./log/Postal", "InsertLetter Err, %s(%s)", content, subject);
        return 0;
    }
    if (!InsertPostal(characNo, (unsigned int)subject, 0, 0, h12, h16, 0,
                      content, occTime, letterNo))
    {
        CMyFileLog log(__FUNCTION__, 0x1da4);
        log("./log/Postal", "InsertPostal Err, %s(%s)", content, subject);
        return 0;
    }
    return 1;
}
char CDBManager::OnLoadPeriodicMessage(
    Packet_Load_Periodic_Message* req,
    Packet_Result_Loading_Periodic_Message* reply)
{
    CDBHandle* h = m_handles[1];    // account db
    h->set_query(0x4f04,
                 "seLect message, start_h, end_h from dnf_game_message where occ_date=cast(now() as date) and display_type=1");
    if (!h->exec(0x4f04))
    {
        CMyFileLog log(__FUNCTION__, 0x2247);
        log("./log/DBQueryErr", "CDBManager::OnLoadPeriodicMessage() seLect Error");
        return 0;
    }
    if (h->get_n_rows() == 0)
    {
        memset((char*)reply + 0xa, 0, 0x200);
        *(int*)((char*)reply + 0x20a) = 0;
        *(int*)((char*)reply + 0x20e) = 0;
        return 1;
    }
    if (!h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0x2256);
        log("./log/DBQueryErr", "CDBManager::OnLoadPeriodicMessage() fetch Error");
        return 0;
    }
    if (!h->get_str(0, (char*)reply + 0xa, 0x200))
    {
        CMyFileLog log(__FUNCTION__, 0x226c);
        log("./log/DBQueryErr",
            "CDBManager::OnLoadPeriodicMessage() get_str Error");
        return 0;
    }
    if (!h->get_int(1, *(int*)((char*)reply + 0x20a)))
    {
        CMyFileLog log(__FUNCTION__, 0x2273);
        log("./log/DBQueryErr",
            "CDBManager::OnLoadPeriodicMessage() get_int for start_h Error");
        return 0;
    }
    if (!h->get_int(2, *(int*)((char*)reply + 0x20e)))
    {
        CMyFileLog log(__FUNCTION__, 0x2279);
        log("./log/DBQueryErr",
            "CDBManager::OnLoadPeriodicMessage() get_int for end_h Error");
        return 0;
    }
    return 1;
}
bool CDBManager::QueryGuildMember(unsigned char serverId,
                                  unsigned int guildId,
                                  Packet_DB_Reply_Query_Guild_Member& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e24,
                      "seLect guild_id, memo, grade, member_point from guild_member where charac_no = %d and server_id = %d and member_flag = 1",
                      guildId, serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x180);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuildMember() Exception Break\n");
        *(char*)((char*)&reply + 0xa) = 0;
        return 0;
    }
    if (!h->exec(0x4e24))
    {
        *(char*)((char*)&reply + 0xa) = 0;
        return 0;
    }
    if (!h->fetch())
    {
        *(char*)((char*)&reply + 0xa) = 2;
        return 0;
    }
    if (!h->get_uint(0, *(unsigned int*)((char*)&reply + 0xb)))
    {
        *(char*)((char*)&reply + 0xa) = 3;
        return 0;
    }
    if (!h->get_str(1, (char*)&reply + 0x13, 0x15))
    {
        *(char*)((char*)&reply + 0xa) = 3;
        return 0;
    }
    if (!h->get_ubyte(2, *(unsigned char*)((char*)&reply + 0x28)))
    {
        *(char*)((char*)&reply + 0xa) = 3;
        return 0;
    }
    if (!h->get_uint(3, *(unsigned int*)((char*)&reply + 0x29)))
    {
        *(char*)((char*)&reply + 0xa) = 3;
        return 0;
    }
    *(char*)((char*)&reply + 0xa) = 1;
    return 1;
}
char CDBManager::OnSavePowerWarUserRank(
    Packet_DB_Save_Power_War_User_Rank* packet)
{
    CDBHandle* h = m_handles[8];    // guild db
    char* p = (char*)packet;
    unsigned char serverId = *(unsigned char*)(p + 0xb);
    if (*(unsigned char*)(p + 0xa) != 0 &&
        *(unsigned char*)(p + 0xc) == 1)
    {
        if (!h->set_query(0x4ea7,
                          "deLete from power_war_user_rank where server_id=%d",
                          serverId))
        {
            CMyFileLog log(__FUNCTION__, 0x18bf);
            log("./log/DBQueryErr",
                "deLete_from_power_war_user_rank Query Error\n");
            return 0;
        }
        if (!h->exec(0x4ea7))
            return 0;
    }
    int count = *(int*)(p + 0x11);
    int startIdx = *(int*)(p + 0xd);
    for (int i = 0; i < count; i++)
    {
        int p1 = *(int*)(p + 0x15 + i * 8);
        int p2 = *(int*)(p + 0x19 + i * 8);
        if (!h->set_query(0x4ea8,
                          "inSert into power_war_user_rank set server_id=%d, rank=%d, charac_no=%d, power_war_point=%d, power_side=%d",
                          serverId, startIdx + i, p1, p2,
                          *(unsigned char*)(p + 0xc)))
        {
            CMyFileLog log(__FUNCTION__, 0x18d8);
            log("./log/DBQueryErr",
                "inSert_into_power_war_user_rank Query Error\n");
            return 0;
        }
        if (!h->exec(0x4ea8))
            return 0;
    }
    return 1;
}
char CDBManager::OnSavePowerWarGuildRank(
    Packet_DB_Save_Power_War_Guild_Rank* packet)
{
    CDBHandle* h = m_handles[8];    // guild db
    char* p = (char*)packet;
    unsigned char serverId = *(unsigned char*)(p + 0xa);
    if (*(unsigned char*)(p + 0xb) == 1)
    {
        if (!h->set_query(0x4ea9,
                          "deLete from power_war_guild_rank where server_id=%d",
                          serverId))
        {
            CMyFileLog log(__FUNCTION__, 0x18f4);
            log("./log/DBQueryErr",
                "deLete_from_power_war_guild_rank Query Error\n");
            return 0;
        }
        if (!h->exec(0x4ea9))
            return 0;
    }
    int count = *(int*)(p + 0xc);
    for (int i = 0; i < count; i++)
    {
        int g1 = *(int*)(p + 0x10 + i * 8);
        int g2 = *(int*)(p + 0x14 + i * 8);
        if (!h->set_query(0x4eaa,
                          "inSert into power_war_guild_rank set server_id=%d, rank=%d, guild_id=%d, power_war_point=%d, power_side=%d",
                          serverId, i, g1, g2,
                          *(unsigned char*)(p + 0xb)))
        {
            CMyFileLog log(__FUNCTION__, 0x190c);
            log("./log/DBQueryErr",
                "inSert_into_power_war_guild_rank Query Error\n");
            return 0;
        }
        if (!h->exec(0x4eaa))
            return 0;
    }
    return 1;
}
bool CDBManager::InsertUdpCharacteristic(Packet_Udp_Characteristic* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    char* p = (char*)packet;
    if (!h->set_query(
            0x4e92,
            "insert into p2pnetwork_statistic (occ_time,server_group,success_party_try,total_party_try,dungeon_bad_ping,dungeon_total,pvp_bad_ping,pvp_total,fair_pvp_bad_ping,fair_pvp_total,success_dungeon_clear,total_dungeon_clear)  values(now(),%hhd,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
            *(signed char*)(p + 0xa), *(int*)(p + 0xb), *(int*)(p + 0xf),
            *(int*)(p + 0x13), *(int*)(p + 0x17), *(int*)(p + 0x1b),
            *(int*)(p + 0x1f), *(int*)(p + 0x23), *(int*)(p + 0x27),
            *(int*)(p + 0x2b), *(int*)(p + 0x2f)))
        return 0;
    if (!h->exec(0x4e92))
        return 0;
    return 1;
}
bool CDBManager::DelBuddy(unsigned int m_id, unsigned int characNo)
{
    bool ret;
    CDBHandle* h = m_handles[2];    // game db
    if (!h->set_query(0x4e53,
                      "deLete from charac_friends where charac_no = %d and friend_no = %d",
                      m_id, characNo))
    {
        CMyFileLog log(__FUNCTION__, 0xc0a);
        log("./log/DBQueryErr",
            "deLete from charac_friends where charac_no = %d and friend_no = %d",
            m_id, characNo);
        return 0;
    }
    ret = h->exec(0x4e53);
    if (!ret)
        return 0;
    return 1;
}
bool CDBManager::insertServerGameEvent(
    Packet_StartGameEventFromServer* packet)
{
    CDBHandle* h = m_handles[1];    // account db
    if (!h)
        return 0;
    char* p = (char*)packet;
    if (!h->set_query(
            0x4f5d,
            " inSert into dnf_event_log (occ_time, event_type, event_flag, parameter1, parameter2,  server_id, start_time, end_time, m_id, expl, etc)  values (unix_timestamp(now()), %d, %d, %d, %d, %d, unix_timestamp(now()), 0, 1, 'event from monitor server', '6th birthday') ",
            *(int*)(p + 0xa), *(int*)(p + 0xe),
            *(unsigned short*)(p + 0x16), *(unsigned short*)(p + 0x18),
            *(int*)(p + 0x12)))
    {
        CMyFileLog log(__FUNCTION__, 0x2e96);
        log("./log/DBQueryErr", h->get_quest_str());
        return 0;
    }
    if (!h->exec(0x4f5d))
    {
        CMyFileLog log(__FUNCTION__, 0x2e9d);
        log("./log/DBQueryErr", "insertServerGameEvent Query(exec) Error");
        return 0;
    }
    return 1;
}
bool CDBManager::updateServerGameEvent(Packet_StopGameEventFromServer* packet)
{
    CDBHandle* h = m_handles[1];    // account db
    if (!h)
        return 0;
    char* p = (char*)packet;
    if (!h->set_query(0x4f5e,
                      " upDate dnf_event_log set end_time = %u  where server_id = %d and event_type = %d and end_time = 0 ",
                      *(unsigned int*)(p + 0x12), *(int*)(p + 0xe),
                      *(int*)(p + 0xa)))
    {
        CMyFileLog log(__FUNCTION__, 0x2eb2);
        log("./log/DBQueryErr", h->get_quest_str());
        return 0;
    }
    if (!h->exec(0x4f5e))
    {
        CMyFileLog log(__FUNCTION__, 0x2eb9);
        log("./log/DBQueryErr", "updateServerGameEvent Query(exec) Error");
        return 0;
    }
    return 1;
}
char CDBManager::UpdateGuildRank(int serverId, CGuildManager* gm)
{
    if (!gm)
        return 0;
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e33,
                      "upDate guild_info set guild_rank = 0 where server_id = %d and expire_flag = 0",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x6cf);
        log("./log/DBQueryErr",
            "CDBManager::UpdateGuildRank() update guild_info set guild_rank = 0 where server_id = %d and expire_flag = 0\n",
            serverId);
        return 0;
    }
    if (!h->exec(0x4e33))
        return 0;
    std::vector<std::pair<unsigned int, STGuildRankInfo*> >* rankList =
        gm->GetVtGuildRankInfo();
    for (std::vector<std::pair<unsigned int, STGuildRankInfo*> >::iterator it =
             rankList->begin();
         it != rankList->end(); ++it)
    {
        STGuildRankInfo* info = it->second;
        if (!info)
            continue;
        if (*(int*)((char*)info + 8) == 0)
            continue;
        if (!h->set_query(0x4e34,
                          "upDate guild_info set guild_rank = %d where guild_id = %d and server_id = %d and expire_flag = 0",
                          *(int*)((char*)info + 8),
                          *(int*)((char*)info + 0), serverId))
        {
            CMyFileLog log(__FUNCTION__, 0x6e6);
            log("./log/DBQueryErr",
                "CDBManager::UpdateGuildRank() Fatal Error Break : update guild_info set guild_rank = %d where guild_id = %d and server_id = %d and expire_flag = 0\n",
                *(int*)((char*)info + 8), *(int*)((char*)info + 0), serverId);
            return 0;
        }
        if (!h->exec(0x4e34))
            return 0;
    }
    return 1;
}
char CDBManager::QueryGuildPointList(int serverId, CGuildManager* gm)
{
    if (!gm)
        return 0;
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e32,
                      "seLect guild_id, guild_point from guild_info where server_id = %d and expire_flag = 0",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x692);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuild() select guild_id, guild_point from guild_info where server_id = %d and expire_flag = 0\n",
            serverId);
        return 0;
    }
    if (!h->exec(0x4e32))
        return 0;
    std::vector<std::pair<unsigned int, STGuildRankInfo*> >* rankList =
        gm->GetVtGuildRankInfo();
    int n = h->get_n_rows();
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
        STGuildRankInfo* info = new (std::nothrow) STGuildRankInfo;
        if (!info)
            return 0;
        if (!h->get_uint(0, *(unsigned int*)((char*)info + 0)))
            return 0;
        if (!h->get_uint(1, *(unsigned int*)((char*)info + 4)))
            return 0;
        rankList->push_back(
            std::make_pair(*(unsigned int*)((char*)info + 4), info));
    }
    return 1;
}
bool CDBManager::QueryP2PStatistics(Packet_P2P_Statistics* packet)
{
    if (!packet)
        return 0;
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    char* p = (char*)packet;
    if (!h->set_query(
            0x4f26,
            "inSert into p2p_statistics ( occ_time, server_group, p2p_user, p2p_min_ping, p2p_max_ping, p2p_avg_ping, p2p_over_ping_100, p2p_over_ping_200, p2p_over_ping_300, p2p_over_ping_400, relay_user, relay_min_ping, relay_max_ping, relay_avg_ping, relay_over_ping_100, relay_over_ping_200, relay_over_ping_300, relay_over_ping_400) values (now(), %d, %d, %d, %d, %d, %u, %u, %u, %u, %d, %d, %d, %d, %u, %u, %u, %u)",
            *(signed char*)(p + 0x12), *(int*)(p + 0xa),
            *(signed short*)(p + 0x13), *(signed short*)(p + 0x15),
            *(signed short*)(p + 0x17), *(int*)(p + 0x19),
            *(int*)(p + 0x1d), *(int*)(p + 0x21), *(int*)(p + 0x25),
            *(int*)(p + 0xe), *(signed short*)(p + 0x29),
            *(signed short*)(p + 0x2b), *(signed short*)(p + 0x2d),
            *(int*)(p + 0x2f), *(int*)(p + 0x33), *(int*)(p + 0x37),
            *(int*)(p + 0x3b)))
    {
        CMyFileLog log(__FUNCTION__, 0x295c);
        log("./log/DBQueryErr", "set_query(insert_p2p_statistics)");
        return 0;
    }
    if (!h->exec(0x4f26))
        return 0;
    return 1;
}
char CDBManager::OnGoldcardEventStatistic(
    Packet_Goldcard_Event_Statistic_STD* packet)
{
    CDBHandle* h = m_handles[4];    // log db
    char* p = (char*)packet;
    for (int i = 0; i <= 0x62; i++)
    {
        if (*(int*)(p + i * 9 + 0xb) != 0 ||
            *(int*)(p + i * 9 + 0xf) != 0)
        {
            h->set_query(0x4f03,
                         "upDate log_goldcard_event set create_cnt=create_cnt+%d,open_cnt=open_cnt+%d where occ_date=cast(now() as date) and level=%d",
                         *(int*)(p + i * 9 + 0xb),
                         *(int*)(p + i * 9 + 0xf), i);
            if (h->exec(0x4f03) != 1)
            {
                CMyFileLog log(__FUNCTION__, 0x222b);
                log("./log/DBQueryErr",
                    "CDBManager::OnGoldcardEventStatistic() upDate Error");
            }
            if (h->getAffectedRowCount() == 0)
            {
                h->set_query(0x4f02,
                             "inSert into log_goldcard_event(occ_date,level,create_cnt,open_cnt) values(cast(now() as date), %d, %d, %d)",
                             i, *(int*)(p + i * 9 + 0xb),
                             *(int*)(p + i * 9 + 0xf));
                if (h->exec(0x4f02) != 1)
                {
                    CMyFileLog log(__FUNCTION__, 0x2236);
                    log("./log/DBQueryErr",
                        "CDBManager::OnGoldcardEventStatistic() inSert Error");
                }
            }
        }
    }
    return 1;
}
bool CDBManager::QueryUpdateChannelOccNum(Packet_User_Count_Statistic* packet)
{
    if (!packet)
        return 0;
    CDBHandle* h = m_handles[2];    // game db
    if (!h)
        return 0;
    char* p = (char*)packet;
    h->set_query(0x4eed,
                 "upDate game_channel set gc_now=%d,gc_up_time=now() where gc_no=%d",
                 *(int*)(p + 0xe), *(int*)(p + 0xa));
    if (!h->exec(0x4eed))
    {
        CMyFileLog log(__FUNCTION__, 0x27dc);
        log("./log/DBQueryErr",
            "upDate game_channel Error : channel_no(%d), user_count(%d)",
            *(int*)(p + 0xe), *(int*)(p + 0xa));
    }
    for (int i = 0; i <= 0x63; i++)
    {
        h->set_query(0x4f29,
                     "upDate channel_occ_info set occ_num=%d where gc_no=%d and age=%d",
                     *(signed short*)(p + 0x12 + i * 2),
                     *(int*)(p + 0xa), i + 1);
        if (!h->exec(0x4f29))
        {
            CMyFileLog log(__FUNCTION__, 0x27e4);
            log("./log/DBQueryErr",
                "upDate channel_occ_info Error : channel_no(%d), user_count(%d)",
                *(signed short*)(p + 0x12 + i * 2), *(int*)(p + 0xa));
        }
    }
    return 1;
}
char CDBManager::OnMemberDeleteAsCharDelete(unsigned int characNo)
{
    char r;
    CDBHandle* h = m_handles[2];    // game db
    CDBHandle* h2 = m_handles[3];   // game2nd db
    h->set_query(0x4e2d,
                 "deLete from charac_members where charac_no=%d", characNo);
    r = h->exec(0x4e2d);
    h->set_query(0x4e2e,
                 "upDate charac_members set master_no = 0 where master_no=%d",
                 characNo);
    r = h->exec(0x4e2e);
    h2->set_query(0x4e7e,
                  "deLete from charac_black_list where charac_no=%d",
                  characNo);
    r = h2->exec(0x4e7e);
    h2->set_query(0x4e7f,
                  "deLete from charac_black_info where charac_no=%d",
                  characNo);
    r = h2->exec(0x4e7f);
    h->set_query(0x4ea3,
                 "deLete from charac_friends where charac_no=%d", characNo);
    r = h->exec(0x4ea3);
    h->set_query(0x4ea4,
                 "deLete from charac_friends where friend_no=%d", characNo);
    r = h->exec(0x4ea4);
    return 1;
}
bool CDBManager::UpdateMemberKeyInCharacInfo(unsigned char serverId,
                                             unsigned int guildId)
{
    bool ret;
    CDBHandle* h = m_handles[2];    // game db
    h->set_query(0x4e26,
                 "upDate charac_info set member_flag = %d where charac_no = %d",
                 serverId, guildId);
    ret = h->exec(0x4e26);
    if (!ret)
        return 0;
    return 1;
}
char CDBManager::QueryGuildBooting(
    Packet_DB_Query_Reply_On_Guild_Booting& reply, int serverId)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e80,
                      "seLect a_side_point, b_side_point, winner_side from power_war where server_id = %d",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x1297);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuildBooting() : seLect a_side_point, b_side_point, winner_side from power_war where server_id = %d",
            serverId);
        return 0;
    }
    if (!h->exec(0x4e80))
        return 0;
    if (h->get_n_rows() != 0)
    {
        if (!h->fetch())
            return 0;
        if (!h->get_uint(0, *(unsigned int*)((char*)&reply + 0xa)))
            return 0;
        if (!h->get_uint(1, *(unsigned int*)((char*)&reply + 0xe)))
            return 0;
        if (!h->get_ubyte(2, *(unsigned char*)((char*)&reply + 0x12)))
            return 0;
    }
    return 1;
}
char CDBManager::QueryHellPartyStatisticItemCreate(
    Packet_DBMW_HellParty_Statistic_Item* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    char* p = (char*)packet;
    int count = *(int*)(p + 0xa);
    CMyFileLog log(__FUNCTION__, 0x1848);
    log("./log/statistic",
        "Packet_DBMW_HellParty_Statistic_Item : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xbc\xf6\xbd\xc5\n",
        count);
    for (int i = 0; i < count; i++)
    {
        char* e = p + i * 0x24;
        if (!h->set_query(
                0x4ec0,
                "inSert into log_hellparty_value (occ_time, hellparty_type, dungeon_index, dungeon_diff, party_count, hellparty_diff, update_count, uncommon_count, rare_count, uniq_count, epic_count) values (now(), %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                *(unsigned char*)(e + 0xe), *(int*)(e + 0xf),
                *(signed char*)(e + 0x13), *(signed char*)(e + 0x14),
                *(signed char*)(e + 0x15), *(int*)(e + 0x16),
                *(int*)(e + 0x1e), *(int*)(e + 0x22), *(int*)(e + 0x26),
                *(int*)(e + 0x2a)))
        {
            CMyFileLog log2(__FUNCTION__, 0x185c);
            log2("./log/statistic",
                 "\nQueryDeathTowerValueStatisticCreate db error!!\n");
            return 0;
        }
        if (!h->exec(0x4ec0))
            return 0;
    }
    return 1;
}
char CDBManager::OnSavePacketOverflowWrite(
    Packet_DBMW_Packet_Overflow_Statistic* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    char name[0x100];
    if (*(unsigned char*)((char*)packet + 0xa) == 0)
    {
        int idx = *(unsigned short*)((char*)packet + 0xb);
        if (idx >= getNotiPacketNameCount())
            memcpy(name, "???", 4);
        else
            strcpy(name, g_szNotiPacketName[idx]);
    }
    else
    {
        int idx = *(unsigned short*)((char*)packet + 0xb);
        if (idx >= getCmdPacketNameCount())
            memcpy(name, "???", 4);
        else
            strcpy(name, g_szCmdPacketName[idx]);
    }
    char sql[0x400];
    sprintf(sql, "upDate packet_overflow set cnt=cnt+%d where packet_type=%d and packet_kind='%s'",
            *(int*)((char*)packet + 0xd),
            *(unsigned char*)((char*)packet + 0xa), name);
    if (!h->set_query(0x4eba, "%s", sql))
        return 0;
    if (!h->exec(0x4eba))
    {
        memset(sql, 0, 0x400);
        sprintf(sql, "inSert into packet_overflow (packet_type, packet_kind, cnt) values (%d, '%s', %d)",
                *(unsigned char*)((char*)packet + 0xa), name,
                *(int*)((char*)packet + 0xd));
        if (!h->set_query(0x4eb9, "%s", sql))
            return 0;
        if (!h->exec(0x4eb9))
            return 0;
    }
    return 1;
}
char CDBManager::QueryErrorLineStatisticCreate(
    Packet_DBMW_Save_Error_Line_Statistic* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    char* p = (char*)packet;
    int count = *(int*)(p + 0xa);
    char buf[0x200] = {0};
    std::string sql;
    for (int i = 0; i < count; i++)
    {
        char* e = p + i * 0xa;
        if (sql.size() != 0)
            sprintf(buf, ",(from_unixtime(%d),%d,%d,%d)", now,
                    *(unsigned short*)(e + 0x12), *(int*)(e + 0xe),
                    *(int*)(e + 0x14));
        else
            sprintf(buf, "(from_unixtime(%d),%d,%d,%d)", now,
                    *(unsigned short*)(e + 0x12), *(int*)(e + 0xe),
                    *(int*)(e + 0x14));
        sql += buf;
    }
    if (!h->set_query(0x4e88,
                      "inSert into log_packet_dispatcher_error_line(occ_time,channel_no,error_line,cnt) values%s",
                      sql.c_str()))
        return 0;
    if (!h->exec(0x4e88))
        return 0;
    return 1;
}
bool CDBManager::QueryTowerOfDespairStatistic(
    Packet_TowerOfDespair_Statistic_STD* packet)
{
    if (!packet)
        return 0;
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    char* p = (char*)packet;
    for (int i = 1; i <= 0x64; i++)
    {
        if (*(int*)(p + 0x12 + i * 8) == 0)
            continue;
        if (*(int*)(p + 0xe + i * 8) == 0)
            continue;
        h->set_query(0x4f27,
                     "inSert into log_tower_despair_layer_stat(occ_date,server_id,layer,enter,success) values(now(),%d,%d,%d,%d)",
                     *(int*)(p + 0xa), i, *(int*)(p + 0x12 + i * 8),
                     *(int*)(p + 0xe + i * 8));
        if (!h->exec(0x4f27))
        {
            CMyFileLog log(__FUNCTION__, 0x27bc);
            log("./log/DBQueryErr",
                "insert error TOD : group(%d),layer(%d),enter(%d),succ(%d)",
                *(int*)(p + 0xa), i, *(int*)(p + 0x12 + i * 8),
                *(int*)(p + 0xe + i * 8));
        }
    }
    h->set_query(0x4f28,
                 "inSert into log_tower_despair_uv_stat(occ_date,server_id,uv) values(now(),%d,%d)",
                 *(int*)(p + 0xa), *(int*)(p + 0xe));
    if (!h->exec(0x4f28))
    {
        CMyFileLog log(__FUNCTION__, 0x27c8);
        log("./log/DBQueryErr", "insert error TOD : uv(%d)",
            *(int*)(p + 0xe));
    }
    return 1;
}
bool CDBManager::GetVillageAttackedRank(Packet_DB_VillageAttackedRank* packet,
                                        bool& flag, int& a, int& b)
{
    char* p = (char*)packet;
    if (*(unsigned char*)(p + 0xa) == GetMinTimeServerGroup(*(int*)(p + 0xb)) ||
        *(unsigned char*)(p + 0xa) == GetMaxHuntingPointServerGroup(*(int*)(p + 0xf)))
    {
        if (GetCoinEventPerDay(*(unsigned char*)(p + 0xa), 1, a, b))
            flag = true;
    }
    if (*(unsigned char*)(p + 0xa) == GetMinTimeServerGroup(*(int*)(p + 0x13)) ||
        *(unsigned char*)(p + 0xa) == GetMaxHuntingPointServerGroup(*(int*)(p + 0x17)))
    {
        if (GetCoinEventPerDay(*(unsigned char*)(p + 0xa), -1, a, b))
            flag = true;
    }
    return 1;
}
int CDBManager::GetMinTimeServerGroup(int serverId)
{
    CDBHandle* h = m_handles[6];    // sso db
    if (!h->set_query(0x4ee1,
                      "seLect server_info from village_attacked_server_time_rank where occ_date = cast(from_unixtime(%d) as date) order by clear_time asc limit 1",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x1deb);
        log("./log/DBQueryErr", "GetMinTimeServerGroup Error\n");
        return 0;
    }
    if (!h->exec(0x4ee1))
        return 0;
    if (h->get_n_rows() == 0)
        return 0;
    if (!h->fetch())
        return 0;
    int result = 0;
    if (!h->get_int(0, result))
        return 0;
    return result;
}
int CDBManager::GetMaxHuntingPointServerGroup(int serverId)
{
    CDBHandle* h = m_handles[6];    // sso db
    if (!h->set_query(0x4ee0,
                      "seLect server_info from village_attacked_server_point_rank where occ_date = cast(from_unixtime(%d) as date) order by hunting_point desc limit 1",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x1dc9);
        log("./log/DBQueryErr", "GetMaxHuntingPointServerGroup Error\n");
        return 0;
    }
    if (!h->exec(0x4ee0))
        return 0;
    if (h->get_n_rows() == 0)
        return 0;
    if (!h->fetch())
        return 0;
    int result = 0;
    if (!h->get_int(0, result))
        return 0;
    return result;
}
char CDBManager::updateCollectItems(unsigned char a, int b, unsigned int c,
                                    unsigned char d)
{
    CDBHandle* h = m_handles[9];    // event db
    if (!h)
        return 0;
    bool setQueryOk = false;
    if (d == 0)
    {
        if (b <= 0)
            return 1;
        if (c == 0)
            setQueryOk = h->set_query(
                0x4f4d,
                "upDate collect_items set cur_count = cur_count + %d where server_info = %d",
                b, a);
        else
            setQueryOk = h->set_query(
                0x4f4d,
                "upDate collect_items set cur_count = cur_count + %d, full_time = from_unixtime(%d) where server_info = %d",
                b, c, a);
    }
    else
    {
        setQueryOk = h->set_query(
            0x4f4d,
            "upDate collect_items set change_flag = %d where server_info = %d",
            0, a);
    }
    if (!setQueryOk)
    {
        CMyFileLog log(__FUNCTION__, 0x29d6);
        log("./log/DBQueryErr", "upDate collect_items set Error");
        return 0;
    }
    if (!h->exec(0x4f4d))
    {
        CMyFileLog log(__FUNCTION__, 0x29df);
        log("./log/DBQueryErr", "updateCollectItems Query(exec) Error");
        return 0;
    }
    return 1;
}
char CDBManager::updateCollectItemsGm(unsigned char a, int b, int c,
                                      unsigned int d)
{
    CDBHandle* h = m_handles[9];    // event db
    if (!h)
        return 0;
    if (!h->set_query(0x4f4d,
                      "upDate collect_items set cur_count=%u, total_count=%u, change_flag = 1, full_time=from_unixtime(%d) where server_info = %d",
                      b, c, d, a))
    {
        CMyFileLog log(__FUNCTION__, 0x29f6);
        log("./log/DBQueryErr", "upDate collect_items set Error");
        return 0;
    }
    bool ret = h->exec(0x4f4d);
    if (!ret)
    {
        CMyFileLog log(__FUNCTION__, 0x29ff);
        log("./log/DBQueryErr", "updateCollectItems Query(exec) Error");
        return 0;
    }
    return 1;
}
bool CDBManager::insertHolePunchingResult(
    Packet_GameServer2Statisctics2DBServer* packet)
{
    if (!packet)
        return 0;
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    if (!h->set_query(0x4f60,
                      "inSert into p2p_connect_success_rate  (server_group, connected_type, required_time, check_time, nation_code, peer_address, occ_date) values (%d, %d, %d, %d, '%s', '%s', now())",
                      *(unsigned short*)((char*)packet + 0xa),
                      *(signed char*)((char*)packet + 0xc),
                      *(int*)((char*)packet + 0xd),
                      *(int*)((char*)packet + 0x11),
                      (char*)packet + 0x15, (char*)packet + 0x25))
    {
        CMyFileLog log(__FUNCTION__, 0x2edf);
        log("./log/DBQueryErr",
            "set_query(inSert_hole_punching_success_rate_stat)");
        return 0;
    }
    if (!h->exec(0x4f60))
        return 0;
    return 1;
}
bool CDBManager::UpdateRandomboxStatistic(
    Packet_Randombox_statistic_DB* packet)
{
    CDBHandle* h = m_handles[4];    // log db
    char boxKind[0x20] = {0};
    if (!h)
        return 0;
    char* p = (char*)packet;
    for (int i = 0; i <= 4; i++)
    {
        if (i == 0)
            memcpy(boxKind, "randombox", 10);
        else if (i == 2)
            memcpy(boxKind, "emeraldbox", 11);
        if (*(int*)(p + i * 4 + 0xa) == 0)
            continue;
        if (*(int*)(p + i * 4 + 0x1e) == 0)
            continue;
        h->set_query(0x4eea,
                     "inSert into log_randombox(occ_date, box_kind, create_count, open_count) values(CURDATE(), '%s', %d, %d)",
                     boxKind, *(int*)(p + i * 4 + 0xa),
                     *(int*)(p + i * 4 + 0x1e));
        if (!h->exec(0x4eea))
        {
            CMyFileLog log(__FUNCTION__, 0x207b);
            log("./log/statistic", "UpdateRandomboxStatistic db error!!\n");
            return 0;
        }
    }
    return 1;
}
char CDBManager::SaveMemberExp(unsigned int characNo, unsigned int exp,
                               unsigned int lev)
{
    bool ret;
    CDBHandle* h = m_handles[2];    // game db
    h->set_query(0x4e4d,
                 "upDate charac_members set exp=%d where charac_no = %d and master_no = %d",
                 lev, characNo, exp);
    ret = h->exec(0x4e4d);
    if (!ret)
    {
        CMyFileLog log(__FUNCTION__, 0x4f1);
        log("./log/MemberModify",
            "ERROR  CDBManager::SaveMemberExp   upDate charac_members set exp=%d where charac_no = %d and master_no = %d",
            lev, characNo, exp);
        return 0;
    }
    return 1;
}
char CDBManager::UpdatePowerSecedeTime(unsigned char serverId,
                                       unsigned int secedeTime)
{
    bool ret;
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4ec6,
                      "upDate guild_info set power_secede_time = now() where guild_id = %d and server_id = %d",
                      secedeTime, serverId))
    {
        register unsigned int sv = serverId;
        CMyFileLog log(__FUNCTION__, 0x12fa);
        log("./log/DBQueryErr",
            "CDBManager::SavePowerWarPoint() : upDate guild_info set power_secede_time = now() where guild_id = %d and server_id = %d",
            secedeTime, sv);
        return 0;
    }
    ret = h->exec(0x4ec6);
    return 1;
}
bool CDBManager::QueryMsg(Packet_DBMW_Query_Msg* packet)
{
    bool ret;
    CDBHandle* h = m_handles[((QueryMsgView*)packet)->m_fieldE];
    h->set_query(((QueryMsgView*)packet)->m_fieldA,
                 ((QueryMsgView*)packet)->m_data);
    ret = h->exec(((QueryMsgView*)packet)->m_fieldA);
    if (!ret)
    {
        register char* str = ((QueryMsgView*)packet)->m_data;
        CMyFileLog log(__FUNCTION__, 0x1db6);
        log("./log/DBQueryErr", "GetDBMWQueryMsg Query(%s) Error\n",
            str);
        return 0;
    }
    return 1;
}
bool CDBManager::GetDBMWStatistic(Packet_DBMW_Query_String* packet)
{
    bool ret;
    CDBHandle* h = m_handles[4];    // log db
    h->set_query(((QueryStringView*)packet)->m_queryId,
                 ((QueryStringView*)packet)->m_data);
    ret = h->exec(((QueryStringView*)packet)->m_queryId);
    if (!ret)
    {
        register char* str = ((QueryStringView*)packet)->m_data;
        CMyFileLog log(__FUNCTION__, 0x1cb8);
        log("./log/DBQueryErr", "GetDBMWStatistic Query(%s) Error\n",
            str);
        return 0;
    }
    return 1;
}
bool CDBManager::UpdateCreateEmblemStatistic(
    Packet_Emblem_Create_Statistic_DB* packet)
{
    CDBHandle* h = m_handles[4];    // log db
    char* p = (char*)packet;
    if (!h->set_query(0x4ee9,
                      "inSert into log_emblem_create(cur_date, grade0, grade1, grade2, grade3, grade4, grade5, grade6) values(CURDATE(), %d, %d, %d, %d, %d, %d, %d)",
                      *(int*)(p + 0xa), *(int*)(p + 0xe), *(int*)(p + 0x12),
                      *(int*)(p + 0x16), *(int*)(p + 0x1a),
                      *(int*)(p + 0x1e), *(int*)(p + 0x22)))
    {
        CMyFileLog log(__FUNCTION__, 0x1f04);
        log("./log/statistic", "UpdateCreateEmblemStatistic db error!!\n");
        return 0;
    }
    if (!h->exec(0x4ee9))
        return 0;
    return 1;
}
char CDBManager::OnWriteGuildMemberMemo(
    Packet_DB_Write_Guild_Member_Memo* packet)
{
    CDBHandle* h = m_handles[8];    // guild db
    char* p = (char*)packet;
    char buf[0x6002];
    memset(buf, 0, 0x6002);
    h->escape_string(buf, p + 0x12);
    if (!h->set_query(0x4ebb,
                      "upDate guild_member set memo='%s' where guild_id = %d and charac_no = %d",
                      buf, *(int*)(p + 0xa), *(int*)(p + 0xe)))
    {
        CMyFileLog log(__FUNCTION__, 0x1a8e);
        log("./log/DBQueryErr",
            "CDBManager::OnWriteGuildMemo() upDate guild_member set memo='%s' where guild_id = %d and charac_no = %d",
            buf, *(int*)(p + 0xa), *(int*)(p + 0xe));
        return 0;
    }
    if (!h->exec(0x4ebb))
    {
        CMyFileLog log(__FUNCTION__, 0x1a96);
        log("./log/DBQueryErr",
            "upDate_into_guild_member_memo Query Error\n");
        return 0;
    }
    return 1;
}
char CDBManager::OnServerMatchData(Packet_Server_Match_data_DBMW* packet)
{
    CDBHandle* h = m_handles[9];    // event db
    char* p = (char*)packet;
    h->set_query(0x4ef8,
                 "upDate pvp_score set win_count=win_count+%d,lose_count=lose_count+%d where server_id = %d and occ_date = cast(now() as date)",
                 *(int*)(p + 0xb), *(int*)(p + 0xf),
                 *(signed char*)(p + 0xa));
    if (h->exec(0x4ef8) != 1)
    {
        CMyFileLog log(__FUNCTION__, 0x219d);
        log("./log/Except", "OnServerMatchData Error db ");
        return 0;
    }
    if (h->getAffectedRowCount() == 0)
    {
        h->set_query(0x4ef9,
                     "inSert into pvp_score(server_id,occ_date,win_count,lose_count) values(%d,cast(now() as date),%d,%d)",
                     *(signed char*)(p + 0xa), *(int*)(p + 0xb),
                     *(int*)(p + 0xf));
        h->exec(0x4ef9);
    }
    return 1;
}
char CDBManager::OnManagerEventTriggerAck(
    Packet_Manager_Event_Trigger_Ack* packet)
{
    CDBHandle* h = m_handles[1];    // account db
    char* p = (char*)packet;
    int kind = *(int*)(p + 0xe);
    if (kind == 2)
    {
        h->set_query(0x4eff,
                     "upDate dnf_event_log set event_flag=%d where event_type=%d and server_id=%d and ( end_time > unix_timestamp(now()) or end_time=0)",
                     kind, *(int*)(p + 0xa), *(int*)(p + 0x12));
    }
    else if (kind == 4)
    {
        h->set_query(0x4eff,
                     "upDate dnf_event_log set event_flag=%d where event_type=%d and server_id=%d and end_time <>0",
                     kind, *(int*)(p + 0xa), *(int*)(p + 0x12));
    }
    else
    {
        CMyFileLog log(__FUNCTION__, 0x2211);
        log("./log/DBQueryErr",
            "CDBManager::OnManagerEventTriggerAck() Unvalid Kind(%d)", kind);
    }
    if (!h->exec(0x4eff))
    {
        CMyFileLog log(__FUNCTION__, 0x2215);
        log("./log/DBQueryErr",
            "CDBManager::OnManagerEventTriggerAck() upDate Error");
    }
    return 1;
}
char CDBManager::OnSaveLoadingTimeReport(
    Packet_DBMW_Loading_Time_Report* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    for (int i = 0; i <= 8; i++)
    {
        char buf[0x400];
        memset(buf, 0, 0x400);
        snprintf(buf, 0x400,
                 "inSert into loading_time (occ_time, server_id, type, load_sec) values (now(), %d, %d, %d)",
                 *(unsigned char*)((char*)packet + 0xa + i), i,
                 *(int*)((char*)packet + 0x13 + i * 4));
        h->set_query(0x4ec4, "%s", buf);
        h->exec(0x4ec4);
        CMyFileLog log(__FUNCTION__, 0x1ae8);
        log("./log/Statistic", "[LoadingTime] %s", buf);
    }
    return 1;
}
char CDBManager::OnSaveFatigueBattery(
    Packet_DBMW_Fatigue_Battery_Money_Statistic* packet)
{
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    for (int i = 0; i <= 0x64; i++)
    {
        if (((FatigueBatteryView*)packet)->m_entries[i].m_fieldA != 0)
        {
            h->set_query(0x4ec5,
                         "inSert into log_fatigue_battery set occ_date = now(), server_id = %d, money = %d, buff = %d",
                         i, ((FatigueBatteryView*)packet)->m_entries[i].m_fieldA,
                         ((FatigueBatteryView*)packet)->m_entries[i].m_fieldE);
            h->exec(0x4ec5);
            CMyFileLog log(__FUNCTION__, 0x1b23);
            log("./log/Statistic",
                "[Fatigue Battery] inSert into log_fatigue_battery set occ_time = now(), server_id = %d, money = %d, buff = %d",
                i, ((FatigueBatteryView*)packet)->m_entries[i].m_fieldA,
                ((FatigueBatteryView*)packet)->m_entries[i].m_fieldE);
        }
    }
    return 1;
}
void RandomOptionSeed::reset()
{
    m_data[0] = 0;
}
void RandomOption::reset()
{
    m_field0.reset();
    m_field1.reset();
    m_field2.reset();
    m_seed0.reset();
    m_field3.reset();
    m_seed1.reset();
}
void RandomOptionField::reset()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
}
UpgradeSeparateInfo::UpgradeSeparateInfo()
{
    reset();
}
void UpgradeSeparateInfo::reset()
{
    (*(UpgradeSeparateBits*)m_data).b0 &= ~0x1f;
    (*(UpgradeSeparateBits*)m_data).b1 &= ~0x1;
    (*(UpgradeSeparateBits*)m_data).b2 = 0;
}
unsigned char UpgradeSeparateInfo::GetUpgradeSeparate() const
{
    return (unsigned char)(m_data[0] & 0x1f);
}
ReservedCapacity::ReservedCapacity()
{
    reset();
}
void ReservedCapacity::reset()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}
NpcBuyLimitItem::NpcBuyLimitItem()
{
    clear();
}
void NpcBuyLimitItem::clear()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}
STGuildAgitDBInfo::STGuildAgitDBInfo()
{
    *(char*)((char*)this + 0) = 0;
}
DnfItemInfo::DnfItemInfo()
{
    reset();
}
void DnfItemInfo::reset()
{
    m_field0 = 0;
    m_field1 = 0;
    m_field5 = 0;
    m_field6 = 0;
    m_fieldA = 0;
    m_fieldC = 0;
    m_field10 = 0;
    m_field11 = 0;
    m_option.reset();
    m_upg.reset();
    m_resv.reset();
}
STGuildMemberCharacData::STGuildMemberCharacData()
{
    m_field0 = 0xff;
    m_field1 = 0xff;
    m_field2 = 0;
    memset(m_name, 0, 0x1e);
}
STGuildCargoLog::STGuildCargoLog()
{
    memset(this, 0, 0x30);
}
STGuildCargoLog::~STGuildCargoLog()
{
}
STBlackUserDBType::STBlackUserDBType()
{
    m_field0 = 0;
    m_field24 = 0;
    memset(m_pad4, 0, 0x1e);
}
STGuildBoardDBInfo::STGuildBoardDBInfo()
    : m_member()
{
    m_field78 = 0;
    m_field7c = 0;
    m_field80 = 0;
    // (char*)this 强制转换改变 GCC -O0 的实参求值顺序（先求值 arg1），与 ORIG 一致
    memset((char*)this, 0, 0x78);
}
STGuildCargoDBInfo::STGuildCargoDBInfo()
    : m_items()
{
    m_tail = 0;
}
STGuildMemerDBInfo::STGuildMemerDBInfo()
{
    m_field15 = 0;
    m_field16 = 0;
    memset(m_pad, 0, 0x15);
}
STTodayGuildMember::~STTodayGuildMember() {}
st_ip_counter_list::~st_ip_counter_list() {}
st_full_ip_counter_list::~st_full_ip_counter_list() {}
stTowerRank_t::~stTowerRank_t() {}
st_ars_info_list::~st_ars_info_list() {}
void st_ars_info_list::CopyStruct(const st_ars_info_list& other)
{
    m_field0 = other.m_field0;
    m_field2 = other.m_field2;
    m_field4 = other.m_field4;
    m_field6 = other.m_field6;
    m_field8 = other.m_field8;
    m_field9 = other.m_field9;
    m_fieldA = other.m_fieldA;
}
void st_ip_counter_list::CopyStruct(const st_ip_counter_list& other)
{
    m_field0 = other.m_field0;
    m_field2 = other.m_field2;
    memset(m_data, 0, 0xc);
    strncpy(m_data, other.m_data, 0xc);
    m_field10 = other.m_field10;
}
void st_full_ip_counter_list::CopyStruct(const st_full_ip_counter_list& other)
{
    m_field0 = other.m_field0;
    m_field2 = other.m_field2;
    memset(m_data, 0, 0x10);
    strncpy(m_data, other.m_data, 0x10);
    m_field14 = other.m_field14;
}
Packet_Server_Queue_Load_Statistic::Packet_Server_Queue_Load_Statistic()
    : PacketHeader(0x9d2, 0xe)
{
    m_fieldA = 0;
    m_fieldB = 0;
    m_fieldC = 0;
}
Packet_DB_Query_Reply_On_Guild_Booting::Packet_DB_Query_Reply_On_Guild_Booting()
    : PacketHeader(0x677, 0x13)
{
}
Packet_Reply_Load_Tower_Full_Rank::Packet_Reply_Load_Tower_Full_Rank()
    : PacketHeader(0x4cd, 0x17bf)
{
}
Packet_Set_ARS_Info::Packet_Set_ARS_Info()
    : PacketHeader(0xb61, 0x4bf)
{
}
Packet_Result_Ontime_Event_Idx_Update::Packet_Result_Ontime_Event_Idx_Update()
    : PacketHeader(0x2348, 0x16)
{
    m_fieldA = 0;
}
Packet_CollectItemsResult::Packet_CollectItemsResult()
    : PacketHeader(0x27e7, 0x16), m_fieldA(0), m_fieldE(0), m_field12(0) {}
Packet_DB_Reply_Query_Guild::Packet_DB_Reply_Query_Guild()
    : PacketHeader(0x405, 0x135), m_fieldA(0), m_fieldB(0), m_fieldF(0), m_guildInfo()
{
    memset((char*)this + 0xd0, 0, 0x65);
}
Packet_DB_Reply_Guild_Secede::Packet_DB_Reply_Guild_Secede()
    : PacketHeader(0x43a, 0x41)
{
    memset((char*)this + 0x1f, 0, 0x1e);
}
Packet_Guild_Load_Guild_Agit::Packet_Guild_Load_Guild_Agit()
    : PacketHeader(0x6e1, 0xf), m_fieldA(0), m_info() {}
Packet_Notify_New_Group_Mail::Packet_Notify_New_Group_Mail()
    : PacketHeader(0x515, 0x4be)
{
    memset((char*)this + 0xe, 0, 0x4b0);
}
Packet_Guild_Load_Guild_Cargo::Packet_Guild_Load_Guild_Cargo()
    : PacketHeader(0x708, 0x18ea), m_cargo()
{
    memset((char*)this + 0xe, 0, 0x18dc);
}
Packet_Response_IPCounterList::Packet_Response_IPCounterList()
    : PacketHeader(0x1039, 0xbc4), m_fieldA(0), m_fieldB(0) {}
Packet_Reply_Today_Guild_Member::Packet_Reply_Today_Guild_Member()
    : PacketHeader(0x1bc0, 0x35), m_fieldA(0)
{
    memset((char*)this + 0xe, 0, 0x27);
}
Packet_Response_D_IPCounterList::Packet_Response_D_IPCounterList()
    : PacketHeader(0x103a, 0xe1c), m_fieldA(0), m_fieldB(0) {}
Packet_Result_Ontime_Event_Item::Packet_Result_Ontime_Event_Item()
    : PacketHeader(0x2346, 0x14), m_fieldA(0), m_fieldE(0), m_field12(0) {}
Packet_DB_Create_Guild_Agit_Reply::Packet_DB_Create_Guild_Agit_Reply()
    : PacketHeader(0x6dd, 0x16), m_fieldA(0), m_fieldE(0), m_field12(0) {}
Packet_DB_Delete_Guild_Agit_Reply::Packet_DB_Delete_Guild_Agit_Reply()
    : PacketHeader(0x6df, 0x16), m_fieldA(0), m_fieldE(0), m_field12(0) {}
Packet_DB_Reply_Query_Guild_Member::Packet_DB_Reply_Query_Guild_Member()
    : PacketHeader(0x403, 0x2d), m_flag(0), m_fieldB(0), m_fieldF(0), m_info() {}
Packet_DB_Upgrade_Guild_Agit_Reply::Packet_DB_Upgrade_Guild_Agit_Reply()
    : PacketHeader(0x6e4, 0x16), m_fieldA(0), m_fieldE(0), m_result(0) {}
Packet_DB_Load_Reply_Guild_Board_Open::Packet_DB_Load_Reply_Guild_Board_Open()
    : PacketHeader(0x232c, 0x688)
{
    m_fieldA = 0;
    m_fieldC = 0;
    m_fieldD = 0;
    m_field11 = 0;
    m_field15 = 0;
    memset((char*)this + 0x16, 0, 0x672);
}
Packet_DB_Reply_Guild_Master_Delegate::Packet_DB_Reply_Guild_Master_Delegate()
    : PacketHeader(0x43c, 0x38)
{
    memset((char*)this + 0x16, 0, 0x1e);
}
Packet_DB_Response_Approve_Join_Guild::Packet_DB_Response_Approve_Join_Guild()
    : PacketHeader(0x1bc5, 0x56), m_fieldA(0), m_fieldE(0), m_field12(0), m_field16(0)
{
    memset(m_data, 0, 0x3c);
}
Packet_Guild_Load_Guild_Cargo_History::Packet_Guild_Load_Guild_Cargo_History()
    : PacketHeader(0x709, 0x972), m_fieldA(0), m_fieldE(0)
{
    memset((char*)this + 0x12, 0, 0x960);
}
Packet_DB_Load_Reply_Guild_Board_Write::Packet_DB_Load_Reply_Guild_Board_Write()
    : PacketHeader(0x2330, 0xb9), m_board()
{
    m_fieldA = 0;
    m_fieldC = 0;
    m_field10 = 0;
}
Packet_Result_Loading_Periodic_Message::Packet_Result_Loading_Periodic_Message()
    : PacketHeader(0x1f49, 0x212)
{
    memset((char*)this + 0xa, 0, 0x200);
    m_field20A = 0;
    m_field20E = 0;
}
Packet_DB_Load_Reply_Guild_Board_Delete::Packet_DB_Load_Reply_Guild_Board_Delete()
    : PacketHeader(0x2334, 0x18)
{
    m_fieldA = 0;
    m_field14 = 0;
    m_fieldC = 0;
    m_field10 = 0;
}
Packet_DB_Load_Reply_Web_Guild_Board_Write::Packet_DB_Load_Reply_Web_Guild_Board_Write()
    : PacketHeader(0x233f, 0xb9), m_board()
{
    m_fieldA = 0;
    m_fieldC = 0;
    m_field10 = 0;
}
Packet_DB_Monitor_Change_Unconnected_GuildMember_Grade::
    Packet_DB_Monitor_Change_Unconnected_GuildMember_Grade()
    : PacketHeader(0x42b, 0x36), m_fieldA(0), m_fieldE(0), m_field30(0xff), m_field31(0), m_field32(0)
{
    memset((char*)this + 0x12, 0, 0x1e);
}
Packet_DB_Reply_Query_Member::Packet_DB_Reply_Query_Member()
    : PacketHeader(0x4b3, 0x1c5), m_flag(0), m_fieldB(0), m_master() {}
Packet_DB_Reply_Unconn_Guild_Member::Packet_DB_Reply_Unconn_Guild_Member()
    : PacketHeader(0x427, 0x53), m_fieldA(0), m_fieldE(0), m_proxy() {}
Packet_DB_Reply_Guild_All_Members::Packet_DB_Reply_Guild_All_Members()
    : PacketHeader(0x426, 0x17b1), m_fieldA(0), m_fieldE(0), m_flag(0), m_count(0) {}
int CDBManager::FindCharProxyInArray(ST_MemberProxy* proxies, unsigned int characNo,
                                     unsigned char maxIdx)
{
    for (int i = 0; i < maxIdx; i++)
    {
        if (proxies[i].m_no != 0)
        {
            if ((unsigned int)proxies[i].m_no == characNo)
                return i;
        }
        else
        {
            __asm__ __volatile__("nop");
        }
    }
    return -1;
}
bool CDBManager::QueryMember(unsigned int characNo, Packet_DB_Reply_Query_Member& reply)
{
    CDBHandle* h = m_handles[2];    // game db
    char* mbase = (char*)&reply + 0x17;  // m_master（STMemberDBInfo，紧打包）
    if (!h->set_query(
            0x4e29,
            "seLect 1 as type, master_no, exp, unix_timestamp(create_time), unix_timestamp(delete_time) as charac from charac_members where charac_no = %d union all select 2, charac_no, exp, unix_timestamp(create_time), unix_timestamp(delete_time) from charac_members where master_no = %d",
            characNo, characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x52d);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuildMember() select 1 as type, master_no as charac from charac_members where charac_no = %d union all select 2, charac_no from charac_members where master_no = %d\n",
            characNo, characNo);
        reply.m_flag = 0;
        return 0;
    }
    if (!h->exec(0x4e29))
    {
        reply.m_flag = 0;
        CMyFileLog log(__FUNCTION__, 0x537);
        log("./log/DBQueryErr",
            "CDBManager::QueryMember() db->exec(select_from_charac_members_for_lower_member_query, Query ID : %d\n",
            characNo);
        return 0;
    }
    char str[0x100] = {0};
    str[0] = '(';
    int n = h->get_n_rows();
    if (n > 0xb)
        n = 0xb;
    if (n == 0)
    {
        *(char*)(mbase + 0x27) = 0;
        reply.m_flag = 1;
        return 1;
    }
    if (!h->fetch())
    {
        reply.m_flag = 0;
        CMyFileLog log(__FUNCTION__, 0x54f);
        log("./log/DBQueryErr",
            "CDBManager::QueryMember() First db->fetch(), Lower, Query ID : %d, n_data : %d\n",
            characNo, n);
        return 0;
    }
    int type = 0;
    if (!h->get_int(0, type))
    {
        reply.m_flag = 3;
        return 0;
    }
    ST_MemberProxy* proxies = (ST_MemberProxy*)(mbase + 0x28);
    unsigned int maxExp = 0;
    unsigned int maxIdx = 0;
    if (type == 1)
    {
        unsigned int masterNo = 0;
        if (!h->get_uint(1, masterNo))
        {
            reply.m_flag = 3;
            return 0;
        }
        if (masterNo != 0)
        {
            *(int*)(mbase + 0) = masterNo;
            if (!h->get_uint(2, *(unsigned int*)(mbase + 0x23)))
            {
                reply.m_flag = 3;
                return 0;
            }
        }
        unsigned int t = 0;
        if (!h->get_uint(3, t))
        {
            reply.m_flag = 3;
            return 0;
        }
        if (t > maxExp)
            maxExp = t;
        if (!h->get_uint(4, t))
        {
            reply.m_flag = 3;
            return 0;
        }
        if (t > maxIdx)
            maxIdx = t;
        *(char*)(mbase + 0x27) = (char)(n - 1);
        sprintf(str, "%s%d,", str, *(int*)(mbase + 0));
        for (int i = 1; i < n; i++)
        {
            if (!h->fetch())
            {
                reply.m_flag = 0;
                CMyFileLog log(__FUNCTION__, 0x58e);
                log("./log/DBQueryErr",
                    "CDBManager::QueryMember() 1 == type and find lower db->fetch() loop : %d, Lower, Query ID : %d\n",
                    i, characNo);
                return 0;
            }
            if (!h->get_uint(1, (unsigned int&)proxies[i - 1].m_no))
            {
                reply.m_flag = 3;
                return 0;
            }
            sprintf(str, "%s%d,", str, proxies[i - 1].m_no);
            if (!h->get_uint(2, (unsigned int&)proxies[i - 1].m_field23))
            {
                reply.m_flag = 3;
                return 0;
            }
        }
    }
    else if (type == 2)
    {
        if (!h->get_uint(1, (unsigned int&)proxies[0].m_no))
        {
            reply.m_flag = 3;
            return 0;
        }
        if (!h->get_uint(2, (unsigned int&)proxies[0].m_field23))
        {
            reply.m_flag = 3;
            return 0;
        }
        *(char*)(mbase + 0x27) = (char)n;
        sprintf(str, "%s%d,", str, proxies[0].m_no);
        for (int i = 1; i < n; i++)
        {
            if (!h->fetch())
            {
                reply.m_flag = 0;
                CMyFileLog log(__FUNCTION__, 0x5cb);
                log("./log/DBQueryErr",
                    "CDBManager::QueryMember() 1 != type and find lower db->fetch() loop, Lower Query ID : %d\n",
                    i, characNo);
                return 0;
            }
            if (!h->get_uint(1, (unsigned int&)proxies[i].m_no))
            {
                reply.m_flag = 3;
                return 0;
            }
            sprintf(str, "%s%d,", str, proxies[i].m_no);
            if (!h->get_uint(2, (unsigned int&)proxies[i - 1].m_field23))
            {
                reply.m_flag = 3;
                return 0;
            }
        }
    }
    else
    {
        reply.m_flag = 0;
        CMyFileLog log(__FUNCTION__, 0x5e3);
        log("./log/DBQueryErr",
            "CDBManager::QueryMember() 1 != type and 2 != type, type(%d)\n", type);
        return 0;
    }
    int len = strlen(str);
    str[len - 1] = 0;
    sprintf(str, "%s)", str);
    reply.m_fieldF = (int)maxExp;
    reply.m_field13 = (int)maxIdx;
    if (!h->set_query(0x4e2b,
                      "seLect charac_no, lev, charac_name from charac_info where charac_no in %s",
                      str))
    {
        CMyFileLog log(__FUNCTION__, 0x5f5);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuildMember() select lev, charac_name from charac_info where charac_no in %s\n",
            str);
        reply.m_flag = 2;
        return 0;
    }
    if (!h->exec(0x4e2b))
    {
        reply.m_flag = 0;
        return 0;
    }
    n = h->get_n_rows();
    ST_MemberProxy* master = (ST_MemberProxy*)mbase;
    int j = 0;
    for (; j < n; j++)
    {
        if (!h->fetch())
        {
            reply.m_flag = 0;
            return 0;
        }
        int no = 0;
        if (!h->get_uint(0, (unsigned int&)no))
        {
            reply.m_flag = 3;
            return 0;
        }
        if (master->m_no == no)
        {
            if (!h->get_ubyte(1, master->m_lev))
            {
                reply.m_flag = 3;
                return 0;
            }
            if (!h->get_str(2, master->m_name, 0x1d))
            {
                reply.m_flag = 3;
                return 0;
            }
        }
        else
        {
            int found = FindCharProxyInArray(proxies, (unsigned int)no, 0xa);
            if (found == -1)
                CDNFException("CDBManager::QueryMember(), Not Coresponding Database!");
            if (!h->get_ubyte(1, proxies[found].m_lev))
            {
                reply.m_flag = 3;
                return 0;
            }
            if (!h->get_str(2, proxies[found].m_name, 0x1d))
            {
                reply.m_flag = 3;
                return 0;
            }
        }
    }
    reply.m_flag = 1;
    return 1;
}
bool CDBManager::QueryGuildMemberProxy(unsigned int guildId, unsigned int characNo,
                                       STGuildMemberProxy& proxy)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e54,
                      "seLect charac_no, charac_name, job, lev, grow_type, sex from guild_member where guild_id = %d and charac_no = %d and member_flag = 1",
                      guildId, characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x25d);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuildMemberProxy() seLect charac_no, charac_name, job, lev, grow_type, sex from guild_member where guild_id = %d and charac_no = %d and member_flag = 1",
            guildId, characNo);
        return 0;
    }
    if (!h->exec(0x4e54))
        return 0;
    if (!h->fetch())
        return 0;
    if (!h->get_uint(0, (unsigned int&)proxy.m_no))
        return 0;
    if (!h->get_str(1, proxy.m_name, 0x1d))
        return 0;
    if (!h->get_ubyte(2, proxy.m_field22))
        return 0;
    if (!h->get_short(3, (short&)proxy.m_field24))
        return 0;
    if (!h->get_ubyte(4, proxy.m_field23))
        return 0;
    if (!h->get_ubyte(5, proxy.m_field26))
        return 0;
    return 1;
}
bool CDBManager::QueryGuildAllMembersProxy(unsigned int guildId,
                                           STGuildMemberProxy* proxies,
                                           unsigned short& count)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e23,
                      "seLect charac_no, charac_name, job, lev, grow_type, sex, grade, unix_timestamp(last_play_time), memo from guild_member where guild_id = %d and member_flag = 1 limit %d",
                      guildId, 0x12c))
    {
        CMyFileLog log(__FUNCTION__, 0x1d7);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuildAllMembersProxy() seLect charac_no, charac_name, job, lev, grow_type, sex, grade, unix_timestamp(last_play_time) from guild_member where guild_id = %d and member_flag = 1 limit %d",
            guildId, 0x12c);
        return 0;
    }
    if (!h->exec(0x4e23))
        return 0;
    count = (unsigned short)h->get_n_rows();
    if (count > 0x12c)
        count = 0x12c;
    for (int i = 0; i < count; i++)
    {
        if (!h->fetch())
            return 0;
        if (!h->get_uint(0, (unsigned int&)proxies[i].m_no))
            return 0;
        if (!h->get_str(1, proxies[i].m_name, 0x1d))
            return 0;
        if (!h->get_ubyte(2, proxies[i].m_field22))
            return 0;
        if (!h->get_short(3, (short&)proxies[i].m_field24))
            return 0;
        if (!h->get_ubyte(4, proxies[i].m_field23))
            return 0;
        if (!h->get_ubyte(5, proxies[i].m_field26))
            return 0;
        if (!h->get_ubyte(6, proxies[i].m_field27))
            return 0;
        if (!h->get_uint(7, (unsigned int&)proxies[i].m_field28))
            return 0;
        if (!h->get_str(8, proxies[i].m_data2c, 0x15))
            return 0;
    }
    return 1;
}
bool CDBManager::GuildJoin(STGuildJoinInfo* info, unsigned int& result)
{
    result = 2;
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[2];   // game db
    if (info->m_characName[0] == 0)
    {
        result = 0x27;
        CMyFileLog log(__FUNCTION__, 0xe76);
        log("./log/TraceGuildErr",
            "CDBManager::GuildJoin guild(%d), server_group(%d), charac_no(%d)\n",
            info->m_guildId, info->m_serverId, info->m_characNo);
        return 0;
    }
    if (!h->set_query(0x4e60,
                      "seLect member_flag, unix_timestamp(secede_time) from guild_member where charac_no = %d and  server_id= %d",
                      info->m_characNo, info->m_serverId))
    {
        CMyFileLog log(__FUNCTION__, 0xe80);
        log("./log/DBQueryErr",
            "CDBManager::GuildJoin()select_secede_time_from_guild_member_for_guildjoin Exception Break\n");
        return 0;
    }
    if (!h->exec(0x4e60))
        return 0;
    if (h->fetch())
    {
        int memberFlag = 0;
        if (!h->get_uint(0, (unsigned int&)memberFlag))
            return 0;
        if (memberFlag == 1)
        {
            result = 0x27;
            return 0;
        }
        if (memberFlag == 2)
        {
            unsigned int secedeTime = 0;
            if (!h->get_uint(1, secedeTime))
                return 0;
            if (!isDayTimeOver(secedeTime, 3))
            {
                result = 0x68;
                return 0;
            }
        }
    }
    if (!h->set_query(0x4e83,
                      "seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
                      info->m_guildId))
    {
        CMyFileLog log(__FUNCTION__, 0xeb9);
        log("./log/DBQueryErr",
            "CDBManager::GuildJoin() seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
            info->m_guildId);
        return 0;
    }
    if (!h->exec(0x4e83))
    {
        CMyFileLog log(__FUNCTION__, 0xebe);
        log("./log/DBQueryErr",
            "CDBManager::GuildJoin() seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
            info->m_guildId);
        return 0;
    }
    if (!h->fetch())
        return 0;
    int memberCount = 0;
    if (!h->get_int(0, memberCount))
        return 0;
    if (memberCount + 1 > 0x12c)
    {
        result = 0x26;
        return 0;
    }
    if (!h->set_query(0x4e61,
                      "upDate guild_member set guild_id=%d, member_flag=1, member_time= now(), grade = 0,last_visit_time = 0, secede_type = 0, secede_time = 0, member_point = 0, member_point_prev = 0, last_play_time = 0  where charac_no = %d and server_id= %d",
                      info->m_guildId, info->m_characNo, info->m_serverId))
    {
        CMyFileLog log(__FUNCTION__, 0xedb);
        log("./log/DBQueryErr",
            "CDBManager::GuildJoin() upDate guild_member set guild_id=%d, member_flag=1 where charac_no = %d and server_id= %d",
            info->m_guildId, info->m_characNo, info->m_serverId);
        return 0;
    }
    if (!h->exec(0x4e61) || h->getAffectedRowCount() == 0)
    {
        if (!h->set_query(0x4e5e,
                          "inSert into guild_member set guild_id=%d,m_id=%s,server_id=%d,charac_no=%d,charac_name='%s',job=%d,grow_type=%d,lev=%d,born_year='%s',sex=%d,member_flag=1,member_time= now()",
                          info->m_guildId, NumberToString(info->m_id, 0),
                          info->m_serverId, info->m_characNo, info->m_characName,
                          info->m_job, info->m_growType, info->m_lev,
                          info->m_bornYear, info->m_sex))
        {
            CMyFileLog log(__FUNCTION__, 0xefd);
            log("./log/DBQueryErr",
                "CDBManager::GuildJoin() Exception Break\n");
            return 0;
        }
        if (!h->exec(0x4e5e))
            return 0;
    }
    if (memberCount != 0)
    {
        if (!h->set_query(0x4e5f,
                          "upDate guild_info set member_count = %d where guild_id = %d",
                          memberCount + 1, info->m_guildId))
        {
            CMyFileLog log(__FUNCTION__, 0xf0d);
            log("./log/DBQueryErr",
                "CDBManager::GuildJoin() upDate guild_info set member_count = %d where guild_id = %d joined(%d)",
                memberCount + 1, info->m_guildId, info->m_characNo);
            return 0;
        }
        if (!h->exec(0x4e5f))
        {
            CMyFileLog log(__FUNCTION__, 0xf12);
            log("./log/DBQueryErr",
                "CDBManager::GuildJoin() upDate guild_info set member_count = %d where guild_id = %d joined(%d)",
                memberCount + 1, info->m_guildId, info->m_characNo);
            return 0;
        }
    }
    if (!h2->set_query(0x4e65,
                       "upDate charac_info set guild_id=%d where charac_no = %d",
                       info->m_guildId, info->m_characNo))
    {
        CMyFileLog log(__FUNCTION__, 0xf1d);
        log("./log/DBQueryErr",
            "CDBManager::GuildJoin() upDate charac_info set guild_id=%d where charac_no = %d",
            info->m_guildId, info->m_characNo);
        return 0;
    }
    if (!h2->exec(0x4e65))
    {
        CMyFileLog log(__FUNCTION__, 0xf24);
        log("./log/DBQueryErr",
            "CDBManager::GuildJoin() upDate charac_info set guild_id=%d where charac_no = %d",
            info->m_guildId, info->m_characNo);
        return 0;
    }
    result = 0;
    return 1;
}
char CDBManager::SaveMemberInsert(unsigned int masterNo, unsigned int characNo,
                                  unsigned char type)
{
    CDBHandle* h = m_handles[2];    // game db
    if (type == 2)
    {
        if (!h->set_query(0x4e45,
                          "upDate charac_members set master_no = %d , create_time = now() where charac_no = %d",
                          masterNo, characNo))
        {
            CMyFileLog log(__FUNCTION__, 0x455);
            log("./log/DBQueryErr",
                "CDBManager::SaveMemberInsert() upDate charac_members set master_no = %d , create_time = now() where charac_no = %d",
                masterNo, characNo);
            return 0;
        }
        if (!h->exec(0x4e45) || h->getAffectedRowCount() == 0)
        {
            if (!h->set_query(0x4e46,
                              "inSert into charac_members set charac_no=%d, master_no=%d, exp = 0, create_time = now()",
                              characNo, masterNo))
            {
                CMyFileLog log(__FUNCTION__, 0x466);
                log("./log/DBQueryErr",
                    "CDBManager::SaveMemberInsert() inSert into charac_members set charac_no=%d, master_no=%d, exp = 0, create_time = now()",
                    characNo, masterNo);
                return 0;
            }
            if (!h->exec(0x4e46))
                return 0;
        }
        if (!h->set_query(0x4e4b,
                          "upDate charac_members set charac_no = %d, create_time = now() where charac_no = %d",
                          masterNo, masterNo))
        {
            CMyFileLog log(__FUNCTION__, 0x47b);
            log("./log/DBQueryErr",
                "CDBManager::SaveMemberInsert() upDate charac_members set charac_no = %d where charac_no = %d",
                masterNo, masterNo);
            return 0;
        }
        if (!h->exec(0x4e4b) || h->getAffectedRowCount() == 0)
        {
            if (!h->set_query(0x4e47,
                              "inSert into charac_members set charac_no=%d, master_no=0, exp = 0, create_time = now()",
                              masterNo))
            {
                CMyFileLog log(__FUNCTION__, 0x488);
                log("./log/DBQueryErr",
                    "CDBManager::SaveMemberInsert() inSert into charac_members set charac_no=%d, master_no=0, exp = 0",
                    masterNo);
                return 0;
            }
            if (!h->exec(0x4e47))
                return 0;
        }
        return 1;
    }
    if (type != 1)
        return 1;
    if (!h->set_query(0x4e48,
                      "upDate charac_members set master_no = %d, create_time = now() where charac_no = %d",
                      masterNo, characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x49d);
        log("./log/DBQueryErr",
            "CDBManager::SaveMemberInsert() upDate charac_members set master_no = %d where charac_no = %d",
            masterNo, characNo);
        return 0;
    }
    if (!h->exec(0x4e48) || h->getAffectedRowCount() == 0)
    {
        if (!h->set_query(0x4e49,
                          "inSert into charac_members set charac_no=%d, master_no=%d, exp = 0",
                          characNo, masterNo))
        {
            CMyFileLog log(__FUNCTION__, 0x4ae);
            log("./log/DBQueryErr",
                "CDBManager::SaveMemberInsert() inSert into charac_members set charac_no=%d, master_no=%d, exp = 0",
                characNo, masterNo);
            return 0;
        }
        if (!h->exec(0x4e49))
            return 0;
    }
    if (!h->set_query(0x4e4a,
                      "upDate charac_members set create_time = now() where charac_no = %d",
                      masterNo))
    {
        CMyFileLog log(__FUNCTION__, 0x4c2);
        log("./log/DBQueryErr",
            "CDBManager::SaveMemberInsert() upDate charac_members set create_time = now() where charac_no = %d",
            masterNo);
        return 0;
    }
    if (!h->exec(0x4e4a) || h->getAffectedRowCount() == 0)
    {
        if (!h->set_query(0x4e46,
                          "inSert into charac_members set charac_no=%d, master_no=0, exp = 0, create_time=now()",
                          masterNo))
        {
            CMyFileLog log(__FUNCTION__, 0x4cf);
            log("./log/DBQueryErr",
                "CDBManager::SaveMemberInsert() inSert into charac_members set charac_no=%d, master_no=0, exp = 0, create_time=now()",
                masterNo);
            return 0;
        }
        if (!h->exec(0x4e46))
            return 0;
    }
    return 1;
}
char CDBManager::SaveMemberDelete(unsigned int characNo, unsigned int masterNo,
                                  unsigned char type)
{
    CDBHandle* h = m_handles[2];    // game db
    if (type == 1)
    {
        h->set_query(0x4e4e,
                     "upDate charac_members set master_no = 0, exp = 0 where charac_no=%d",
                     masterNo);
        bool r1 = h->exec(0x4e4e);
        if (!r1)
            return 0;
        h->set_query(0x4e4c,
                     "upDate charac_members set  delete_time = now() where charac_no=%d",
                     characNo);
        bool r2 = h->exec(0x4e4c);
    }
    if (type == 2)
    {
        h->set_query(0x4e4f,
                     "upDate charac_members set  master_no = 0 , exp = 0, delete_time = now() where charac_no=%d",
                     masterNo);
        bool r3 = h->exec(0x4e4f);
        if (!r3)
            return 0;
    }
    return 1;
}
bool CDBManager::OnGuildJoinByListApprove(unsigned int guildId,
                                          char serverId,
                                          unsigned int m_id,
                                          unsigned int characNo,
                                          STGuildJoinInfo& joinInfo,
                                          unsigned int& result)
{
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[2];   // game db
    memset(&joinInfo, 0, 0x3c);
    joinInfo.m_serverId = serverId;
    joinInfo.m_guildId = guildId;
    joinInfo.m_fieldC = m_id;
    joinInfo.m_characNo = characNo;
    if (!h->set_query(0x4f0a,
                      "seLect m_id,born_year from guild_join_list where guild_id=%d and charac_no=%d",
                      guildId, characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x23d1);
        log("./log/DBQueryErr", "set_query(seLect_from_guild_join_list) Query Error");
        return 0;
    }
    if (!h->exec(0x4f0a) || !h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0x23d7);
        log("./log/DBQueryErr", "exec(seLect_from_guild_join_list) or fetch() Query Error");
        return 0;
    }
    if (!h->get_uint(0, joinInfo.m_id))
    {
        CMyFileLog log(__FUNCTION__, 0x23dd);
        log("./log/DBQueryErr", "get_uint(0, join_info.m_uAccId) Query Error");
        return 0;
    }
    if (!h->get_str(1, joinInfo.m_bornYear, 3))
    {
        CMyFileLog log(__FUNCTION__, 0x23e3);
        log("./log/DBQueryErr",
            "get_str(1, join_info.m_bornYear, sizeof(join_info.m_bornYear)) Query Error");
        return 0;
    }
    if (!h2->set_query(0x4f0b,
                       "seLect charac_name,job,grow_type,lev,sex from charac_info where charac_no=%d",
                       characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x23ec);
        log("./log/DBQueryErr",
            "set_query(seLect_from_charac_info_with_guild_join_list Query Error");
        return 0;
    }
    if (!h2->exec(0x4f0b) || !h2->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0x23f2);
        log("./log/DBQueryErr",
            "exec(seLect_from_charac_info_with_guild_join_list) or fetch() Query Error");
        return 0;
    }
    if (!h2->get_str(0, joinInfo.m_characName, 0x1d))
    {
        CMyFileLog log(__FUNCTION__, 0x240a);
        log("./log/DBQueryErr", "get_str(0, join_info.m_szJoinCharName) Query Error");
        return 0;
    }
    if (!h2->get_byte(1, (char&)joinInfo.m_lev))
    {
        CMyFileLog log(__FUNCTION__, 0x2411);
        log("./log/DBQueryErr", "get_byte(1, join_info.m_JoinJob) Query Error");
        return 0;
    }
    if (!h2->get_byte(2, (char&)joinInfo.m_growType))
    {
        CMyFileLog log(__FUNCTION__, 0x2417);
        log("./log/DBQueryErr", "get_byte(2, join_info.m_JoinGrowType) Query Error");
        return 0;
    }
    if (!h2->get_byte(3, (char&)joinInfo.m_job))
    {
        CMyFileLog log(__FUNCTION__, 0x241d);
        log("./log/DBQueryErr", "get_byte(3, join_info.m_JoinLevel) Query Error");
        return 0;
    }
    if (!h2->get_byte(4, (char&)joinInfo.m_sex))
    {
        CMyFileLog log(__FUNCTION__, 0x2423);
        log("./log/DBQueryErr", "get_byte(4, join_info.m_JoinSex) Query Error");
        return 0;
    }
    if (!GuildJoin(&joinInfo, result))
        return 0;
    if (!h->set_query(0x4f0c,
                      "deLete from guild_join_list where guild_id=%d and charac_no=%d",
                      guildId, characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x242f);
        log("./log/DBQueryErr", "set_query(deLete_from_guild_join_list) Query Error");
        return 0;
    }
    if (!h->exec(0x4f0c))
    {
        CMyFileLog log(__FUNCTION__, 0x2435);
        log("./log/DBQueryErr", "guild_db->exec(deLete_from_guild_join_list) Query Error");
        return 0;
    }
    return 1;
}
char CDBManager::QueryPartyStatisticCreate(
    Packet_DBMW_Dungeon_Statistic_Party* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    int count = packet->m_count;
    CMyFileLog log(__FUNCTION__, 0x15b3);
    log("./log/statistic",
        "Packet_DBMW_Dungeon_Statistic_Party : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xbc\xf6\xbd\xc5\n",
        count);
    char buf[0x200] = {0};
    std::string sql;
    for (int i = 0; i < count; i++)
    {
#define PM(i) ((STPartyMemberStat*)((char*)packet + 0x10 + (i) * 0x3c))
        if (sql.size())
            sprintf(buf,
                    ",(now(),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                    PM(i)->m_fieldE, PM(i)->m_field10, PM(i)->m_field14,
                    PM(i)->m_field15, PM(i)->m_field16, PM(i)->m_field17,
                    PM(i)->m_field18, PM(i)->m_field19, PM(i)->m_field1A,
                    PM(i)->m_field1E, PM(i)->m_field22, PM(i)->m_field26,
                    PM(i)->m_field2A, PM(i)->m_field2E, PM(i)->m_field32,
                    PM(i)->m_field36, PM(i)->m_field46, PM(i)->m_field42,
                    PM(i)->m_field3E, PM(i)->m_field3A);
        else
            sprintf(buf,
                    "(now(),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                    PM(i)->m_fieldE, PM(i)->m_field10, PM(i)->m_field14,
                    PM(i)->m_field15, PM(i)->m_field16, PM(i)->m_field17,
                    PM(i)->m_field18, PM(i)->m_field19, PM(i)->m_field1A,
                    PM(i)->m_field1E, PM(i)->m_field22, PM(i)->m_field26,
                    PM(i)->m_field2A, PM(i)->m_field2E, PM(i)->m_field32,
                    PM(i)->m_field36, PM(i)->m_field46, PM(i)->m_field42,
                    PM(i)->m_field3E, PM(i)->m_field3A);
#undef PM
        if (sql.length() + 0x800 > 0x6000)
        {
            h->set_query(0x4e99,
                         "inSert into log_dungeon_party_job(last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, success, party_user_count, charac_job, charac_grow, job_count, rank) values%s",
                         sql.c_str());
            if (!h->exec(0x4e99))
            {
                CMyFileLog log2(__FUNCTION__, 0x15e1);
                log2("./log/statistic",
                     "\nQueryPartyJobStatisticCreate db error!!\n");
                return 0;
            }
            sql.clear();
            i--;
        }
        else
        {
            sql += buf;
        }
    }
    h->set_query(0x4e97,
                 "inSert into log_dungeon_party(last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, success, party_user_count, clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, update_count, level, fatigue_consume, exp_add) values%s",
                 sql.c_str());
    if (!h->exec(0x4e97))
    {
        CMyFileLog log2(__FUNCTION__, 0x15f0);
        log2("./log/statistic", "\nQueryPartyStatisticCreate db error!!\n");
        return 0;
    }
    return 1;
}
char CDBManager::QueryPartyJobStatisticCreate(
    Packet_DBMW_Dungeon_Statistic_Party_Job* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    int count = packet->m_count;
    CMyFileLog log(__FUNCTION__, 0x1645);
    log("./log/statistic",
        "Packet_DBMW_Dungeon_Statistic_Party_Job : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xbc\xf6\xbd\xc5\n",
        count);
    char buf[0x200] = {0};
    std::string sql;
    for (int i = 0; i < count; i++)
    {
#define PM(i) ((STPartyJobMemberStat*)((char*)packet + 0x10 + (i) * 0x19))
        if (sql.size())
            sprintf(buf,
                    ",(now(),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                    PM(i)->m_fieldE, PM(i)->m_field10, PM(i)->m_field14,
                    PM(i)->m_field15, PM(i)->m_field16, PM(i)->m_field17,
                    PM(i)->m_field18, PM(i)->m_field19, PM(i)->m_field1A,
                    PM(i)->m_field1E, PM(i)->m_field1F, PM(i)->m_field23);
        else
            sprintf(buf,
                    "(now(),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                    PM(i)->m_fieldE, PM(i)->m_field10, PM(i)->m_field14,
                    PM(i)->m_field15, PM(i)->m_field16, PM(i)->m_field17,
                    PM(i)->m_field18, PM(i)->m_field19, PM(i)->m_field1A,
                    PM(i)->m_field1E, PM(i)->m_field1F, PM(i)->m_field23);
#undef PM
        if (sql.length() + 0x800 > 0x6000)
        {
            h->set_query(0x4e99,
                         "inSert into log_dungeon_party_job(last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, success, party_user_count, charac_job, charac_grow, job_count, rank) values%s",
                         sql.c_str());
            if (!h->exec(0x4e99))
            {
                CMyFileLog log2(__FUNCTION__, 0x166a);
                log2("./log/statistic",
                     "\nQueryPartyJobStatisticCreate db error!!\n");
                return 0;
            }
            sql.clear();
            i--;
        }
        else
        {
            sql += buf;
        }
    }
    h->set_query(0x4e99,
                 "inSert into log_dungeon_party_job(last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, abuse_party, balkun_party, success, party_user_count, charac_job, charac_grow, job_count, rank) values%s",
                 sql.c_str());
    if (!h->exec(0x4e99))
    {
        CMyFileLog log2(__FUNCTION__, 0x1678);
        log2("./log/statistic", "\nQueryPartyJobStatisticCreate db error!!\n");
        return 0;
    }
    return 1;
}
char CDBManager::QueryPartyCharacStatisticCreate(
    Packet_DBMW_Dungeon_Statistic_Party_Charac* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    int count = packet->m_count;
    CMyFileLog log(__FUNCTION__, 0x16ce);
    log("./log/statistic",
        "Packet_DBMW_Dungeon_Statistic_Party_Charac : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xbc\xf6\xbd\xc5\n",
        count);
    char buf[0x200] = {0};
    std::string sql;
    for (int i = 0; i < count; i++)
    {
#define PM(i) ((STPartyCharacMemberStat*)((char*)packet + 0x10 + (i) * 0x43))
        if (sql.size())
            sprintf(buf,
                    ",(now(),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                    PM(i)->m_fieldE, PM(i)->m_field10, PM(i)->m_field14,
                    PM(i)->m_field15, PM(i)->m_field16, PM(i)->m_field17,
                    PM(i)->m_field1B, PM(i)->m_field1D, PM(i)->m_field21,
                    PM(i)->m_field25, PM(i)->m_field29, PM(i)->m_field2D,
                    PM(i)->m_field31, PM(i)->m_field35, PM(i)->m_field39,
                    PM(i)->m_field49, PM(i)->m_field45, PM(i)->m_field41,
                    PM(i)->m_field3D, PM(i)->m_field1C, PM(i)->m_field4D);
        else
            sprintf(buf,
                    "(now(),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
                    PM(i)->m_fieldE, PM(i)->m_field10, PM(i)->m_field14,
                    PM(i)->m_field15, PM(i)->m_field16, PM(i)->m_field17,
                    PM(i)->m_field1B, PM(i)->m_field1D, PM(i)->m_field21,
                    PM(i)->m_field25, PM(i)->m_field29, PM(i)->m_field2D,
                    PM(i)->m_field31, PM(i)->m_field35, PM(i)->m_field39,
                    PM(i)->m_field49, PM(i)->m_field45, PM(i)->m_field41,
                    PM(i)->m_field3D, PM(i)->m_field1C, PM(i)->m_field4D);
#undef PM
        if (sql.length() + 0x800 > 0x6000)
        {
            h->set_query(0x4e9b,
                         "inSert into log_dungeon_charac(last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, success, charac_job, charac_grow , clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, update_count, level, fatigue_consume, exp_avg, party_user_count, rank) values%s",
                         sql.c_str());
            if (!h->exec(0x4e9b))
            {
                CMyFileLog log2(__FUNCTION__, 0x16ff);
                log2("./log/statistic",
                     "\nQueryPartyCharacStatisticCreate db error!!\n");
                return 0;
            }
            sql.clear();
            i--;
        }
        else
        {
            sql += buf;
        }
    }
    h->set_query(0x4e9b,
                 "inSert into log_dungeon_charac(last_time, channel_no, dungeon_index, dungeon_diff, dungeon_standard_level, success, charac_job, charac_grow , clear_time, die_count, hp_consume, mp_consume, hit_count, hit_per_avg_damage, hp_recovery, mp_recovery, update_count, level, fatigue_consume, exp_avg, party_user_count, rank) values%s",
                 sql.c_str());
    if (!h->exec(0x4e9b))
    {
        CMyFileLog log2(__FUNCTION__, 0x170c);
        log2("./log/statistic", "\nQueryPartyCharacStatisticCreate db error!!\n");
        return 0;
    }
    return 1;
}
bool CDBManager::SaveGuildInfo(unsigned char serverGroup, unsigned int guildId,
                               STGuildDBInfoOnly& info)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (info.m_guildName[0] == 0)
    {
        register unsigned char sg = serverGroup;
        CMyFileLog log(__FUNCTION__, 0x2e2);
        log("./log/TraceGuildErr",
            "CDBManager::SaveGuildInfo server_group(%d), guild_id(%d) GuildName NULL\n",
            sg, guildId);
    }
    h->set_query(0x4e25,
                 "upDate guild_info set lev=%d, ability=%d, guild_point=%d, guild_exp = %d, guild_name = '%s', power_side=%d, power_war_point=%d, guild_agit_flag=%d, power_join_count=%d, guild_fund = %d where guild_id = %d",
                 info.m_lev, info.m_ability, info.m_guildPoint,
                 info.m_guildExp, info.m_guildName, info.m_powerSide,
                 info.m_powerWarPoint, info.m_guildAgitFlag,
                 info.m_powerJoinCount, info.m_guildFund, guildId);
    if (!h->exec(0x4e25))
        return 0;
    return 1;
}
CDBManager::CDBManager()
{
    m_app = 0;
    for (int i = 0; i <= 0x10; i++)
        m_handles[i] = 0;
}
CDBHandle* CDBManager::GetDBHandle(ENUM_DB_HANDLE_IDX idx)
{
    return m_handles[idx];
}
CDBManager::~CDBManager()
{
    Close();
    for (int i = 0; i <= 0x10; i++)
    {
        if (m_handles[i])
        {
            delete m_handles[i];
            m_handles[i] = 0;
        }
    }
}
char CDBManager::Open(ENUM_DB_HANDLE_IDX idx, const char* host, unsigned int port,
                      const char* user, const char* pass, const char* db)
{
    return ((CMySql*)m_handles[idx])->open(host, port, user, pass, db);
}
void CDBManager::Close()
{
    for (int i = 0; i <= 0x10; i++)
    {
        if (m_handles[i])
            m_handles[i]->close();
    }
}
bool CDBManager::UpdateQueryCount(unsigned int idx, int count, int time)
{
    bool ret;
    CDBHandle* h = m_handles[4];
    if (count <= 0)
        return 0;
    h->set_query(0x4e2c,
                 "inSert into log_query_stat(occ_time,q_id,total,response_time) values(now(),%d,%d,%d)",
                 idx, count, time);
    ret = h->exec(0x4e2c);
    if (!ret)
        return 0;
    return 1;
}
char CDBManager::SelectTest()
{
    int i = 0;
    int j = 0;
    CDBHandle* h = m_handles[2];
    if (!h->set_query(0x4e21,
                      "seLect m_id, charac_no from charac_info where m_id = 1001024"))
    {
        puts("select login_status, m_channel_no from login_account");
        return 0;
    }
    if (!h->exec(0x4e21))
        return 0;
    if (!h->fetch())
        return 0;
    if (!h->get_int(0, j))
        return 0;
    if (!h->get_int(1, i))
        return 0;
    return 1;
}
void CDBManager::Init(ENUM_DB_KIND kind, CApplication* app)
{
    m_app = app;
    if (kind == 1)
    {
        for (int i = 0; i <= 0x10; i++)
        {
            m_handles[i] = new (std::nothrow) CMySql;
            if (!m_handles[i])
                throw CDNFException("CDBManager::Init() DB Handle New Error\n");
            if (!m_handles[i]->init())
                throw CDNFException("CDBManager::Init() DB Handle Error\n");
        }
    }
    else
        throw CDNFException("CDBManager::Init() DB Handle Create Error\n");
}
unsigned int CDBManager::GetIdentity(CDBHandle* h)
{
    h->set_query(0x4e5d, "seLect @@identity");
    if (!h->exec(0x4e5d))
        return 0;
    if (!h->fetch())
        return 0;
    unsigned int id;
    if (!h->get_uint(0, id))
        return 0;
    return id;
}
bool CDBManager::QueryInsertUpdate(PacketInsertUpdate* packet)
{
    CDBHandle* h = m_handles[packet->m_handleIdx];
    h->set_query(packet->m_insertQueryId, packet->m_insertSql);
    if (h->exec(packet->m_insertQueryId))
    {
        if (h->getAffectedRowCount() == 0)
        {
            h->set_query(packet->m_updateQueryId, packet->m_updateSql);
            h->exec(packet->m_updateQueryId);
        }
        return 1;
    }
    CMyFileLog log(__FUNCTION__, 0x27f5);
    log("./log/DBQueryErr", "QueryInsertUpdate Query(%s) Error\n",
        packet->m_insertSql);
    return 0;
}
char CDBManager::InsertDailyBadSpecStatistics(
    Packet_Frame_Lag_Statistic_Write_Daily_Bad_Spec* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    h->set_query(0x4e8b,
                 "insert into daily_bad_spec(occ_date,spec_id,server_group) values(curdate(),%d,%hhd",
                 packet->m_specId, packet->m_serverGroup);
    bool ret = h->exec(0x4e8b);
    if (!ret)
        return 0;
    return 1;
}
char CDBManager::RegisterQueryIdTable(int queryId, const char* query)
{
    CDBHandle* h = m_handles[6];    // sso db
    char buf[0x6002] = {0};
    h->escape_string(buf, query);
    h->set_query(0x4f61,
                 "inSert into log_query_dbmw_ref(q_id,query,query_hash) values(%d,'%s',password('%s'))",
                 queryId, buf, buf);
    h->exec(0x4f61);
    return 1;
}
char CDBManager::LoadQueryIdTable()
{
    CDBHandle* h = m_handles[6];    // sso db
    if (!h->set_query(0x4f61, "seLect q_id from log_query_dbmw_ref"))
    {
        CMyFileLog log(__FUNCTION__, 0xcb6);
        log("./log/DBQueryErr", "seLect q_id from log_query_dbmw_ref");
        return 0;
    }
    if (!h->exec(0x4f61))
        return 0;
    int n = h->get_n_rows();
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            break;
        int queryId = 0;
        if (!h->get_int(0, queryId))
            return 0;
        CQueryCounterInstance()->LoadQueryIdTable(queryId);
    }
    return 1;
}
int CDBManager::FindCharIdInArray(unsigned int* arr, unsigned int characNo,
                                  unsigned char maxIdx)
{
    int i = 0;
    while (i < (int)maxIdx)
    {
        if (arr[i] != 0)
        {
            if (arr[i] == characNo)
                return i;
        }
        else
        {
            __asm__ __volatile__("nop");
        }
        i++;
    }
    return -1;
}
char CDBManager::OnSaveTingUserAccount(
    Packet_DBMW_Ting_User_TimeCheck_Write_Query* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    int count = packet->m_count;
    for (int i = 0; i < count; i++)
    {
        char buf[0x400];
        memset(buf, 0, 0x400);
        snprintf(buf, 0x400,
                 "inSert into ting_user_account (occ_time, m_id, minute) values (now(), %s, %d)",
                 NumberToString(packet->m_entries[i].m_id, 0),
                 packet->m_entries[i].m_minute);
        h->set_query(0x4ebf, "%s", buf);
        h->exec(0x4ebf);
    }
    return 1;
}
char CDBManager::OnSavePowerwarLagReport(
    Packet_DBMW_Powerwar_Lag_Report* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    int count = packet->m_count;
    for (int i = 0; i < count; i++)
    {
        h->set_query(0x4eca, "%s", packet->m_sql[i]);
        h->exec(0x4eca);
        register char* q = packet->m_sql[i];
        CMyFileLog log(__FUNCTION__, 0x1b0f);
        log("./log/Statistics", "[PowerWar Lag] %s", q);
    }
    return 1;
}
char CDBManager::OnSaveUsedMemoryWriteQuery(
    Packet_Frame_Lag_Used_Memory_Write_Query* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    h->set_query(0x4e91, "%s", packet->m_query);
    bool ret = h->exec(0x4e91);
    if (!ret)
        return 0;
    return 1;
}
char CDBManager::OnReasonCrashDownQueryWrite(
    Packet_DBMW_Reason_Crash_Down_Query* packet)
{
    bool ok;
    CDBHandle* h = m_handles[4];    // log db
    h->set_query(0x4edd, packet->m_query);
    ok = h->exec(0x4edd);
    if (!ok)
    {
        register char* q = packet->m_query;
        CMyFileLog log(__FUNCTION__, 0x1d53);
        log("./log/StatisticsErr", "Query Error : %s",
            q);
        return 0;
    }
    return 1;
}
char CDBManager::OnSavePowerwarLoadingReport(
    Packet_DBMW_Powerwar_Loading_Time_Report* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    int count = packet->m_count;
    for (int i = 0; i < count; i++)
    {
        h->set_query(0x4ec9, "%s", packet->m_sql[i]);
        h->exec(0x4ec9);
        register char* q = packet->m_sql[i];
        CMyFileLog log(__FUNCTION__, 0x1afc);
        log("./log/Statistics", "[PowerWar LoadingTime] %s", q);
    }
    return 1;
}
char CDBManager::OnSaveUserTingTimeCheckWrite(
    Packet_DBMW_User_Ting_TimeCheck_Write_Query* packet)
{
    char buf[0x400];
    bool r;
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    int count = packet->m_count;
    for (int i = 0; i < count; i++)
    {
        memset(buf, 0, 0x400);
        sprintf(buf,
                "inSert into user_ting_timecheck (occ_time, minute, cnt) values (now(),%d,%d)",
                packet->m_entries[i].m_minute,
                packet->m_entries[i].m_count);
        h->set_query(0x4ebc, "%s", buf);
        r = h->exec(0x4ebc);
    }
    return 1;
}
char CDBManager::OnTechnicalReportCommonQuery(
    Packet_DBMW_TechnicalReport_Common_Query* packet)
{
    bool ret;
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    h->set_query(0x4ef2, "%s", packet->m_query);
    ret = h->exec(0x4ef2);
    if (!ret)
    {
        register char* q = packet->m_query;
        CMyFileLog log(__FUNCTION__, 0x2188);
        log("./log/TechnicalReport", "OnTechnicalReportCommonQuery Error (%s)", q);
    }
    return 1;
}
char CDBManager::SunAhWriteQuery(
    Packet_Frame_Lag_Statistic_Write_Query* packet)
{
    CDBHandle* h = m_handles[0xf];  // frame_lag db
    if (!h)
        return 0;
    h->set_query(0x4e90, "%s", packet->m_query);
    bool ret = h->exec(0x4e90);
    if (!ret)
        return 0;
    return 1;
}
char CDBManager::Open(ENUM_DB_HANDLE_IDX idx, const char* host,
                      const char* user, const char* pass, const char* db)
{
    return m_handles[idx]->open(host, user, pass, db);
}
char CDBManager::OnDeleteGuildAgit(Packet_DB_Delete_Guild_Agit* req,
                                   Packet_DB_Delete_Guild_Agit_Reply& reply)
{
    bool ret;
    CDBHandle* h = m_handles[8];    // guild db
    h->set_query(0x4eaf,
                 "deLete from guild_agit where guild_id=%d",
                 req->m_guildId);
    ret = h->exec(0x4eaf);
    if (!ret)
    {
        reply.m_field12 = 2;
        CMyFileLog(__FUNCTION__, 0x199d)("./log/DBQueryErr",
            "deLete_from_guild_Agit Query Error\n");
        return 0;
    }
    h->set_query(0x4eb4,
                 "upDate guild_info set guild_agit_flag=0 where guild_id=%d",
                 req->m_guildId);
    ret = h->exec(0x4eb4);
    if (!ret)
    {
        reply.m_field12 = 2;
        CMyFileLog(__FUNCTION__, 0x19ac)("./log/DBQueryErr",
            "upDate_into_guild_info_guild_agit_flag Query Error\n");
        return 0;
    }
    reply.m_field12 = 0;
    return 1;
}
char CDBManager::OnUpgradeGuildAgit(Packet_DB_Upgrade_Guild_Agit* req,
                                    Packet_DB_Upgrade_Guild_Agit_Reply& reply)
{
    bool ret;
    CDBHandle* h = m_handles[8];    // guild db
    h->set_query(0x4eb6,
                 "UpDate guild_agit set upgrade = upgrade + 1 where guild_id = %d",
                 req->m_guildId);
    ret = h->exec(0x4eb6);
    if (!ret)
    {
        reply.m_result = 2;
        CMyFileLog log(__FUNCTION__, 0x19ea);
        log("./log/DBQueryErr", "OnUpgradeGuildAgit Query Error\n");
        return 0;
    }
    reply.m_result = 0;
    return 1;
}
char CDBManager::QueryGuildWarPointList(int guildWarPoint, CGuildManager* gm)
{
    if (!gm)
        return 0;
    CDBHandle* h = m_handles[8];
    if (!h->set_query(
            0x4e3b,
            "seLect guild_id, guild_war_point, guild_name, guild_point_prev from guild_info where server_id = %d and expire_flag = 0 and guild_rank <= %d and guild_rank != 0",
            guildWarPoint, 0xa))
    {
        CMyFileLog log(__FUNCTION__, 0x953);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuildWarPointList() select guild_id, guild_war_point from guild_info where server_id = %d and expire_flag = 0 and guild_rank <= %d and guild_rank != 0\n",
            guildWarPoint, 0xa);
        return 0;
    }
    if (!h->exec(0x4e3b))
        return 0;
    std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >* ranks =
        gm->GetVtGuildWarRankInfo();
    int n = h->get_n_rows();
    if (n > 0xa)
    {
        CMyFileLog log(__FUNCTION__, 0x963);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuildWarPointList() : Server Group( %d )\tMAX_GUILD_WAR_ENTERABLE_RANK( %d ) <-> select n_data( %d )\n",
            guildWarPoint, 0xa, n);
    }
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
        STGuildWarRankInfo* info = new (std::nothrow) STGuildWarRankInfo;
        if (!info)
            return 0;
        if (!h->get_uint(0, info->m_field0))
            return 0;
        if (!h->get_uint(1, info->m_field4))
            return 0;
        if (!h->get_str(2, info->m_name, 0x17))
            return 0;
        if (!h->get_uint(3, info->m_field24))
            return 0;
        ranks->push_back(std::make_pair(info->m_field4, info));
    }
    return 1;
}
char CDBManager::AwardGuildTitleByMail(int guildId, unsigned int characNo,
                                       unsigned int itemId, char* guildName,
                                       unsigned int item)
{
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[3];   // game_db_2nd（postal 表）
    if (!h->set_query(0x4e39,
                      "seLect charac_no from guild_member where guild_id = %d and server_id = %d and member_flag = 1",
                      characNo, guildId))
    {
        CMyFileLog log(__FUNCTION__, 0x82b);
        log("./log/DBQueryErr",
            "CDBManager::AwardGuildTitleByMail() select charac_no from guild_member where server_id = %d and guild_id = %d and member_flag = 1\n",
            guildId, characNo);
        return 0;
    }
    if (!h->exec(0x4e39))
        return 0;
    int n = h->get_n_rows();
    time_t t = time(0);
    struct tm* now = localtime(&t);
    now->tm_hour += 1;
    now->tm_min = 0;
    now->tm_sec = 0;
    time_t awardTime = mktime(now);
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
        unsigned int titleNo = 0;
        if (!h->get_uint(0, titleNo))
            return 0;
        int rand = (int)DNFFLib::get_rand_int(0x3e8);
        if (!h2->set_query(
                0x4e3a,
                "inSert into postal (occ_time, send_charac_no, receive_charac_no, seal_flag, item_id, add_info, endurance, upgrade, gold, send_charac_name ) values ( from_unixtime( %d ), %d, %d, %d, %d, %d, %d, %d, %d,'%s')",
                awardTime, 0, titleNo, 0, itemId, rand, item, 0, 0, guildName))
        {
            CMyFileLog log(__FUNCTION__, 0x87d);
            log("./log/DBQueryErr",
                "CDBManager::AwardGuildTitleByMail() Fatal Error Break : insert into postal (occ_time, send_charac_no, receive_charac_no, seal_flag, item_id, add_info, endurance, upgrade, gold, send_charac_name ) values ( from_unixtime( %d ), %d, %d, %d, %d, %d, %d, %d, %d,'%s')\n",
                awardTime, 0, titleNo, 0, itemId, rand, 0, 0, 0, guildName);
            if (!h2->exec(0x4e3a))
                return 0;
        }
    }
    return 1;
}
bool CDBManager::RegisterToBlackList(unsigned int m_id, unsigned int characNo,
                                     char* characName)
{
    CDBHandle* h = m_handles[3];    // game db
    if (!h->set_query(0x4e3f,
                      "inSert into charac_black_list( m_id, charac_no, charac_name,  occ_time ) values( %s, %d, '%s', now() )",
                      NumberToString(m_id, 0), characNo, characName))
    {
        CMyFileLog log(__FUNCTION__, 0x9fd);
        log("./log/DBQueryErr",
            "CDBManager::RegisterToBlackList() inSert into charac_black_list( m_id, charac_no, charac_name,  occ_time ) values( %s, %d, '%s', now() )",
            NumberToString(m_id, 0), characNo, characName);
        return 0;
    }
    if (!h->exec(0x4e3f))
        return 0;
    if (!h->set_query(0x4e41,
                      "upDate charac_black_info set black_point = black_point + 1 where charac_no = %d",
                      characNo))
    {
        CMyFileLog log(__FUNCTION__, 0xa0e);
        log("./log/DBQueryErr",
            "CDBManager::RegisterToBlackList() upDate charac_black_info set black_point = black_point + 1 where charac_no = %d",
            characNo);
        return 0;
    }
    h->exec(0x4e41);
    if (h->getAffectedRowCount() == 0)
    {
        if (!h->set_query(0x4e43,
                          "inSert into charac_black_info( charac_no, black_point,  offset_point ) values( %d, 1, 0 )",
                          characNo))
        {
            CMyFileLog log(__FUNCTION__, 0xa17);
            log("./log/DBQueryErr",
                "CDBManager::RegisterToBlackList() inSert into charac_black_info( charac_no, black_point,  offset_point ) values( %d, 1, 0 )",
                characNo);
            return 0;
        }
        if (!h->exec(0x4e43))
            return 0;
    }
    if (!h->set_query(0x4ed4,
                      "seLect black_point,offset_point,unix_timestamp(problem_child_time) from charac_black_info where charac_no=%d",
                      characNo))
    {
        CMyFileLog log(__FUNCTION__, 0xa24);
        log("./log/DBQueryErr",
            "CDBManager::RegisterToBlackList() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    if (!h->exec(0x4ed4))
        return 0;
    int n = h->get_n_rows();
    if (n > 1)
    {
        CMyFileLog log(__FUNCTION__, 0xa2c);
        log("./log/BlackListModify",
            "CDBManager::RegisterToBlackList() idata > 1 seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
    }
    if (!h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0xa32);
        log("./log/DBQueryErr",
            "CDBManager::RegisterToBlackList() !db->fetch() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    int blackPoint = 0;    // -0x68
    int offsetPoint = 0;   // -0x6c
    unsigned int problemTime = 0;  // -0x70
    if (!h->get_int(0, blackPoint))
    {
        CMyFileLog log(__FUNCTION__, 0xa3b);
        log("./log/DBQueryErr",
            "CDBManager::RegisterToBlackList() !db->fetch() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    if (!h->get_int(1, offsetPoint))
    {
        CMyFileLog log(__FUNCTION__, 0xa40);
        log("./log/DBQueryErr",
            "CDBManager::RegisterToBlackList() !db->fetch() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    if (!h->get_uint(2, problemTime))
    {
        CMyFileLog log(__FUNCTION__, 0xa45);
        log("./log/DBQueryErr",
            "CDBManager::RegisterToBlackList() !db->fetch() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    if (problemTime == 0 && blackPoint - offsetPoint > 0x63)
    {
        if (!h->set_query(0x4ed5,
                          "upDate charac_black_info set problem_child_time = now() where charac_no = %d",
                          characNo))
        {
            CMyFileLog log(__FUNCTION__, 0xa4e);
            log("./log/DBQueryErr",
                "CDBManager::RegisterToBlackList() upDate charac_black_info set problem_child_time = now() where charac_no = %d",
                characNo);
            return 0;
        }
        if (!h->exec(0x4ed5))
            return 0;
    }
    return 1;
}
bool CDBManager::GuildSecede(Packet_DB_Request_Guild_Secede* req,
                             unsigned int& characNo, unsigned int& m_id,
                             unsigned int& result)
{
    result = 2;
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[2];   // game db
    char grade = 0;
    if (req->m_secedeType)
    {
        if (!h->set_query(0x4e66,
                          "seLect charac_no,grade from guild_member where guild_id = %d and charac_name = '%s' and member_flag = 1",
                          req->m_guildId, req->m_characName))
        {
            CMyFileLog log(__FUNCTION__, 0xfaf);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede()seLect charac_no from guild_member where guild_id = %d and charac_name = '%s' and member_flag = 1",
                req->m_guildId, req->m_characName);
            return 0;
        }
        if (!h->exec(0x4e66))
        {
            CMyFileLog log(__FUNCTION__, 0xfb6);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede() db->exec() seLect charac_no from guild_member where guild_id = %d and charac_name = '%s' and member_flag = 1",
                req->m_guildId, req->m_characName);
            return 0;
        }
        if (!h->fetch())
        {
            result = 0x22;
            return 0;
        }
        if (!h->get_uint(0, characNo))
        {
            CMyFileLog log(__FUNCTION__, 0xfc4);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede() db->get_uint() seLect charac_no from guild_member where guild_id = %d and charac_name = '%s' and member_flag = 1",
                req->m_guildId, req->m_characName);
            return 0;
        }
        if (!h->get_ubyte(1, (unsigned char&)grade))
        {
            CMyFileLog log(__FUNCTION__, 0xfcc);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede() db->get_uint() seLect grade from guild_member where guild_id = %d and charac_name = '%s' and member_flag = 1",
                req->m_guildId, req->m_characName);
            return 0;
        }
    }
    int memberCount = 0;
    if (!h->set_query(0x4e83,
                      "seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
                      req->m_guildId))
        return 0;
    if (!h->exec(0x4e83))
        return 0;
    if (!h->fetch())
        return 0;
    if (!h->get_int(0, memberCount))
        return 0;
    if (req->m_grade == 2 && req->m_masterCharacNo == (int)characNo)
    {
        result = 0x57;
        return 0;
    }
    if (req->m_grade == 2 && req->m_masterCharacNo != req->m_characNo && grade == 2)
    {
        result = 0x18;
        return 0;
    }
    char isMaster = 0;
    if (req->m_grade == 1 && req->m_masterCharacNo == (int)characNo)
    {
        if (memberCount == 1)
            isMaster = 1;
        else
        {
            result = 4;
            return 0;
        }
    }
    if (!h2->set_query(0x4e68,
                       "upDate charac_info set guild_id = 0 where charac_no = %d",
                       characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x100f);
        log("./log/DBQueryErr",
            "CDBManager::GuildSecede() upDate charac_info set guild_id = 0 where charac_no = %d",
            characNo);
        return 0;
    }
    if (!h2->exec(0x4e68))
    {
        CMyFileLog log(__FUNCTION__, 0x1016);
        log("./log/DBQueryErr",
            "CDBManager::GuildSecede() upDate charac_info set guild_id = 0 where charac_no = %d",
            characNo);
        return 0;
    }
    if (!h->set_query(0x4e67,
                      "upDate guild_member set member_flag = 2, secede_time = now(), secede_type = %d where guild_id = %d and charac_no = %d",
                      req->m_grade - 1, req->m_guildId, characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x1023);
        log("./log/DBQueryErr",
            "CDBManager::GuildSecede()upDate guild_member set member_flag = 2 where guild_id = %d and charac_no = %d and member_flag = 1",
            characNo, req->m_guildId);
        return 0;
    }
    if (!h->exec(0x4e67))
    {
        CMyFileLog log(__FUNCTION__, 0x102b);
        log("./log/DBQueryErr",
            "CDBManager::GuildSecede()upDate guild_member set member_flag = 2 where guild_id = %d and charac_no = %d and member_flag = 1",
            characNo, req->m_guildId);
        return 0;
    }
    if (!h->set_query(0x4e83,
                      "seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
                      req->m_guildId))
    {
        CMyFileLog log(__FUNCTION__, 0x1034);
        log("./log/DBQueryErr",
            "CDBManager::GuildSecede() seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
            req->m_guildId);
        return 0;
    }
    if (!h->exec(0x4e83))
    {
        CMyFileLog log(__FUNCTION__, 0x1039);
        log("./log/DBQueryErr",
            "CDBManager::GuildSecede() seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
            req->m_guildId);
        return 0;
    }
    if (!h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0x103f);
        log("./log/DBQueryErr",
            "CDBManager::GuildSecede() db->fetch() seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
            req->m_guildId);
        return 0;
    }
    if (!h->get_int(0, memberCount))
    {
        CMyFileLog log(__FUNCTION__, 0x1046);
        log("./log/DBQueryErr",
            "CDBManager::GuildSecede() db->get_int() seLect count(*) from guild_member where guild_id = %d and member_flag = 1",
            req->m_guildId);
        return 0;
    }
    if (memberCount != 0)
    {
        if (!h->set_query(0x4e74,
                          "upDate guild_info set member_count = %d where guild_id = %d",
                          memberCount, req->m_guildId))
        {
            CMyFileLog log(__FUNCTION__, 0x104e);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede() upDate guild_info set member_count = %d where guild_id = %d seceded(%d)",
                characNo, req->m_guildId, memberCount);
            return 0;
        }
        if (!h->exec(0x4e74))
        {
            CMyFileLog log(__FUNCTION__, 0x1053);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede() upDate guild_info set member_count = %d where guild_id = %d seceded(%d)",
                characNo, req->m_guildId, memberCount);
            return 0;
        }
    }
    if (req->m_secedeType && characNo)
    {
        if (!h2->set_query(0x4f01,
                           "seLect m_id from charac_info where charac_no = %u",
                           characNo))
        {
            CMyFileLog log(__FUNCTION__, 0x105f);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede() seLect m_id from charac_info where charac_no = %u",
                characNo);
            return 0;
        }
        if (!h2->exec(0x4f01))
        {
            CMyFileLog log(__FUNCTION__, 0x1067);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede() seLect m_id from charac_info where charac_no = %u",
                characNo);
            return 0;
        }
        if (!h2->fetch())
        {
            result = 0x22;
            return 0;
        }
        if (!h2->get_uint(0, m_id))
        {
            CMyFileLog log(__FUNCTION__, 0x1076);
            log("./log/DBQueryErr",
                "CDBManager::GuildSecede()  db->get_uint() seLect m_id from charac_info where charac_no = %u",
                characNo);
            return 0;
        }
    }
    if (isMaster)
    {
        if (!h->set_query(0x4f0d,
                          "upDate guild_info set expire_flag=1, expire_time=now() where guild_id=%d",
                          req->m_guildId))
            return 0;
        if (!h->exec(0x4f0d))
            return 0;
        if (!h->set_query(0x4f0e, "deLete from guild_introduce where guild_id=%d",
                          req->m_guildId))
            return 0;
        if (!h->exec(0x4f0e))
            return 0;
        if (!h->set_query(0x4f10, "deLete from guild_member_introduce where guild_id=%d",
                          req->m_guildId))
            return 0;
        if (!h->exec(0x4f10))
            return 0;
        if (!h->set_query(0x4f0f, "deLete from guild_member where guild_id=%d",
                          req->m_guildId))
            return 0;
        if (!h->exec(0x4f0f))
            return 0;
        if (!h->set_query(0x4f11, "deLete from guild_visit where guild_id=%d",
                          req->m_guildId))
            return 0;
        if (!h->exec(0x4f11))
            return 0;
        if (!h->set_query(0x4f12, "deLete from guild_notice where guild_id=%d",
                          req->m_guildId))
            return 0;
        if (!h->exec(0x4f12))
            return 0;
        if (!h->set_query(0x4f13, "deLete from guild_skill where guild_id=%d",
                          req->m_guildId))
            return 0;
        if (!h->exec(0x4f13))
            return 0;
        if (!h->set_query(0x4f13, "deLete from guild_join_list where guild_id=%d",
                          req->m_guildId))
            return 0;
        if (!h->exec(0x4f13))
            return 0;
        result = 1;
        return 1;
    }
    result = 0;
    return 1;
}
bool CDBManager::QueryGuildCreate(Packet_DBMW_Request_Guild_Create* req,
                                  unsigned int& guildId, unsigned int& result)
{
    result = 0;
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[2];   // game db
    if (!h->set_query(0x4e6b,
                      "seLect member_flag, unix_timestamp(secede_time) from guild_member where charac_no = %d and server_id = %d",
                      req->m_characNo, req->m_serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x110a);
        log("./log/DBQueryErr",
            "seLect member_flag from guild_member where server_id = %d and charac_no = %d",
            req->m_serverId, req->m_characNo);
        result = 2;
        return 0;
    }
    if (!h->exec(0x4e6b))
    {
        result = 2;
        return 0;
    }
    if (h->get_n_rows() != 0)
    {
        if (!h->fetch())
        {
            result = 0x22;
            return 0;
        }
        int memberFlag = 0;
        if (!h->get_uint(0, (unsigned int&)memberFlag))
        {
            result = 2;
            return 0;
        }
        if (memberFlag == 1)
        {
            result = 0x20;
            return 0;
        }
        if (memberFlag == 2)
        {
            unsigned int secedeTime = 0;
            if (!h->get_uint(1, secedeTime))
                return 0;
            if (!isDayTimeOver(secedeTime, 3))
            {
                result = 0x68;
                return 0;
            }
        }
    }
    if (req->m_characName[0] == 0 || req->m_guildName[0] == 0)
    {
        result = 2;
        if (req->m_characName[0] == 0)
        {
            CMyFileLog log(__FUNCTION__, 0x114f);
            log("./log/TraceGuildErr",
                "CDBManager::QueryGuildCreate server_group(%d), charac_no(%d) CharacName NULL\n",
                req->m_serverId, req->m_characNo);
            return 0;
        }
        else
        {
            CMyFileLog log(__FUNCTION__, 0x1151);
            log("./log/TraceGuildErr",
                "CDBManager::QueryGuildCreate server_group(%d), charac_no(%d) GuildName NULL\n",
                req->m_serverId, req->m_characNo);
        }
    }
    if (!h->set_query(0x4e6c,
                      "inSert into guild_info set server_id=%d,guild_name='%s',master_id=%s,master_no=%d,master_name='%s',guild_url='%s',create_time=now(),member_count=1",
                      req->m_serverId, req->m_guildName,
                      NumberToString(req->m_id, 0), req->m_characNo,
                      req->m_characName, req->m_guildUrl))
    {
        result = 2;
        return 0;
    }
    if (!h->exec(0x4e6c))
    {
        result = 2;
        return 0;
    }
    guildId = GetIdentity(h);
    std::allocator<char> alloc;
    StackBuffer_char buf = sformat("%s%d", "url", guildId);
    std::string url((char*)buf, alloc);
    h->set_query(0x4f5f,
                 "upDate guild_info set guild_url='%s' where guild_id=%d",
                 url.c_str(), guildId);
    if (!h->exec(0x4f5f) || h->getAffectedRowCount() == 0)
        result = 2;
    h->set_query(0x4e6d,
                 "upDate guild_member set guild_id=%d,m_id=%s,charac_name='%s',grade=1,job=%d,grow_type=%d,lev=%d,born_year='%s',sex=%d,apply_time=now(),member_time=now(),member_flag=1 where charac_no=%d and server_id=%d",
                 guildId, NumberToString(req->m_id, 0), req->m_characName,
                 req->m_job, req->m_growType, req->m_lev, req->m_bornYear,
                 req->m_sex, req->m_characNo, req->m_serverId);
    if (h->exec(0x4e6d) && h->getAffectedRowCount() != 0)
    {
    }
    else
    {
        h->set_query(0x4e6e,
                     "inSert into guild_member set guild_id=%d,charac_no=%d,m_id=%s,server_id=%d,charac_name='%s',grade=1,job=%d,grow_type=%d,lev=%d,born_year='%s',sex=%d,apply_time=now(),member_time=now(),member_flag=1",
                     guildId, req->m_characNo, NumberToString(req->m_id, 0),
                     req->m_serverId, req->m_characName, req->m_job,
                     req->m_growType, req->m_lev, req->m_bornYear, req->m_sex);
        if (!h->exec(0x4e6e))
            result = 2;
    }
    if (!h->set_query(0x4e6f,
                      "inSert into guild_introduce set guild_id=%d,server_id=%d",
                      guildId, req->m_serverId))
    {
        result = 2;
        return 0;
    }
    if (!h->exec(0x4e6f))
        result = 2;
    if (!h->set_query(0x4e70,
                      "inSert into guild_member_introduce set guild_id=%d,charac_no=%d",
                      guildId, req->m_characNo))
    {
        result = 2;
        return 0;
    }
    if (!h->exec(0x4e70))
        result = 2;
    if (!h->set_query(0x4e71,
                      "inSert into guild_visit set guild_id=%d,server_id=%d, total_visit=0, today_visit=0",
                      guildId, req->m_serverId))
    {
        result = 2;
        return 0;
    }
    if (!h->exec(0x4e71))
        result = 2;
    if (!h->set_query(0x4e72, "inSert into guild_skill set guild_id=%d", guildId))
    {
        result = 2;
        return 0;
    }
    if (!h->exec(0x4e72))
        result = 2;
    if (!h2->set_query(0x4e73,
                       "upDate charac_info set guild_id=%d where m_id=%s and charac_no=%d",
                       guildId, NumberToString(req->m_id, 0), req->m_characNo))
    {
        result = 2;
        return 0;
    }
    if (!h2->exec(0x4e73))
        result = 2;
    return 1;
}
ST_MemberProxy::ST_MemberProxy()
{
    m_no = 0;
    m_lev = 0;
    m_field23 = 0;
    memset(m_name, 0, 0x1e);
}
STMemberDBInfo::STMemberDBInfo() : m_count(0) {}
STGuildSkill::STGuildSkill()
{
    m_field0 = 0xffffffff;
    m_field4 = 0xff;
}
STGuildDBInfoOnly::STGuildDBInfoOnly()
    : m_masterNo(0), m_lev(0), m_field1E(0), m_memberCount(0), m_guildPoint(0),
      m_guildRank(0), m_guildExp(0), m_field2D(0), m_field42(0), m_field44(0)
{
    m_powerSide = 0;
    m_powerSecedeTime = 0;
    m_powerWarPoint = 0;
    m_guildAgitFlag = 0;
    m_powerJoinCount = 0;
    m_guildFund = 0;
    m_fieldB9 = 0;
    memset(m_pad2E, 0, 0x14);
    memset((char*)this, 0, 0x17);
    b0 = b0 | 0x1;
    b0 = b0 & ~0x2;
    memset((char*)this + 0x45, 0, 0x50);
    memset((char*)this + 0xa4, 0, 0x15);
}
Packet_Item_Limit_Edition_Load_Data_Rpy::Packet_Item_Limit_Edition_Load_Data_Rpy()
    : PacketHeader(0x1008, 0x7ef)
{
    m_fieldB = 0;
}
char* getList2inQuery(unsigned int count, const unsigned int* list, char* out)
{
    out[0] = 0;
    memcpy(out, "in (", 5);
    unsigned int i;
    for (i = 0; i < count - 1; i++)
        sprintf(out, "%s%d,", out, list[i]);
    sprintf(out, "%s%d)", out, list[i]);
    return out;
}
bool CDBManager::onItemLimitEditionLoadData(
    const Packet_Item_Limit_Edition_Load_Data_Req* req,
    Packet_Item_Limit_Edition_Load_Data_Rpy* rpy)
{
    CDBHandle* h = m_handles[1];    // neople db
    if (!h)
        return 0;
    time_t now = time(0);
    char buf[0x400] = {0};
    if (req->m_fieldF != 0 || req->m_fieldA != 1)
    {
        memcpy(buf, "and ipg_no ", 0xc);
        getList2inQuery(req->m_fieldF, (const unsigned int*)((char*)req + 0x13),
                        buf + 0xb);
    }
    if (!h->set_query(0x4ec7,
                      "seLect ipg_no,item_no,item_cnt,cera_price,gold_price,avatar_period_type,total_cnt,sell_cnt,restrict_no,start_time,end_time,npc_idx,cond_charac_job,cond_lev_begin,cond_lev_end,cond_acc_create_time_begin,cond_acc_create_time_end,cond_cha_create_time_begin,cond_cha_create_time_end from limited_shop_manager where server_id=%d %s and (start_time<%d and end_time>%d) and status_flag=0 limit %d",
                      req->m_fieldB, buf, now, now, 0x1c))
        return 0;
    if (!h->exec(0x4ec7))
        return 0;
    rpy->m_fieldA = req->m_fieldA;
    rpy->m_fieldB = h->get_n_rows();
    int n = rpy->m_fieldB;
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
#define IT(i) ((STItemLimitItem*)((char*)rpy + 0xf + (i) * 0x48))
        int col = 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x0f)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x13)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x17)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x1f)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x23)))
            return 0;
        if (!h->get_ubyte(col++, *(unsigned char*)((char*)IT(i) + 0x1b)))
            return 0;
        if (!h->get_int(col++, *(int*)((char*)IT(i) + 0x2b)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x27)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x2f)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x33)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x37)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x3b)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x3f)))
            return 0;
        if (!h->get_short(col++, *(short*)((char*)IT(i) + 0x43)))
            return 0;
        if (!h->get_short(col++, *(short*)((char*)IT(i) + 0x45)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x47)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x4b)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x4f)))
            return 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)IT(i) + 0x53)))
            return 0;
#undef IT
    }
    return 1;
}
bool CDBManager::onItemLimitEditionUpdateData(
    const Packet_Item_Limit_Edition_Update* packet)
{
    CDBHandle* h = m_handles[1];    // neople db
    if (!h)
        return 0;
    time_t now = time(0);
    for (int i = 0; i < packet->m_count; i++)
    {
        if (((char*)packet)[i * 9 + 0x1a] != 0)
        {
            if (!h->set_query(0x4ec8,
                              "upDate limited_shop_manager set sell_cnt=%d,real_end_time=%d where ipg_no=%d and server_id=%d",
                              *(int*)((char*)packet + i * 9 + 0x16), now,
                              *(int*)((char*)packet + i * 9 + 0x12),
                              packet->m_serverId))
                return 0;
        }
        else
        {
            if (!h->set_query(0x4ec8,
                              "upDate limited_shop_manager set sell_cnt=%d where ipg_no=%d and server_id=%d",
                              *(int*)((char*)packet + i * 9 + 0x16),
                              *(int*)((char*)packet + i * 9 + 0x12),
                              packet->m_serverId))
                return 0;
        }
        h->exec(0x4ec8);
    }
    return 1;
}
bool CDBManager::QueryGuild(unsigned char serverGroup, unsigned int guildId,
                            Packet_DB_Reply_Query_Guild& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e22,
                      "seLect guild_name, master_no, lev, ability, member_count, guild_rank, guild_point, guild_exp, power_side, unix_timestamp(power_secede_time), power_war_point, guild_agit_flag, power_join_count, guild_fund,master_name from guild_info where guild_id = %d and server_id = %d and expire_flag = 0",
                      guildId, serverGroup))
    {
        CMyFileLog log(__FUNCTION__, 0x97);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuild() select guild_name, master_no, lev, ability, member_count, guild_rank, guild_point, guild_exp from guild_info where guild_id = %d\n",
            guildId);
        *(char*)((char*)&reply + 0xa) = 0;
        return 0;
    }
    if (!h->exec(0x4e22))
    {
        *(char*)((char*)&reply + 0xa) = 0;
        return 0;
    }
    if (!h->fetch())
    {
        *(char*)((char*)&reply + 0xa) = 2;
        return 0;
    }
    char* info = (char*)&reply + 0x13;
#define QG_FAIL() \
    do { \
        *(char*)((char*)&reply + 0xa) = 3; \
        return 0; \
    } while (0)
    if (!h->get_str(0, info, 0x17))
        QG_FAIL();
    if (!h->get_uint(1, *(unsigned int*)(info + 0x17)))
        QG_FAIL();
    if (!h->get_ubyte(2, *(unsigned char*)(info + 0x1b)))
        QG_FAIL();
    if (!h->get_ushort(3, *(unsigned short*)(info + 0x1c)))
        QG_FAIL();
    if (!h->get_ushort(4, *(unsigned short*)(info + 0x22)))
        QG_FAIL();
    unsigned int guildRank = 0;
    if (!h->get_uint(5, guildRank))
        QG_FAIL();
    if (guildRank > 0x64)
        *(char*)(info + 0x28) = 0;
    else
        *(char*)(info + 0x28) = (char)guildRank;
    if (!h->get_uint(6, *(unsigned int*)(info + 0x24)))
        QG_FAIL();
    if (!h->get_uint(7, *(unsigned int*)(info + 0x29)))
        QG_FAIL();
    if (!h->get_ubyte(8, *(unsigned char*)(info + 0x95)))
        QG_FAIL();
    if (!h->get_uint(9, *(unsigned int*)(info + 0x96)))
        QG_FAIL();
    if (!h->get_uint(10, *(unsigned int*)(info + 0x9a)))
        QG_FAIL();
    if (!h->get_ubyte(11, *(unsigned char*)(info + 0x9e)))
        QG_FAIL();
    if (!h->get_ubyte(12, *(unsigned char*)(info + 0x9f)))
        QG_FAIL();
    if (!h->get_uint(13, *(unsigned int*)(info + 0xa0)))
        QG_FAIL();
    if (!h->get_str(14, info + 0xa4, 0x15))
        QG_FAIL();
#undef QG_FAIL
    *(char*)((char*)&reply + 0xa) = 1;
    return 1;
}
char CDBManager::SaveServerQueueLoadStatistic(unsigned char type, int kind,
                                              int qCnt)
{
    bool ret;
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    h->set_query(0x4ecd,
                 "inSert into log_otherserver_load_stat set occ_time=now(), server_type=%d, kind=%d, q_cnt=%d",
                 type, kind, qCnt);
    ret = h->exec(0x4ecd);
    if (!ret)
    {
        CMyFileLog log(__FUNCTION__, 0x1c5f);
        log("./log/DBQueryErr", "SaveServerQueueLoadStatistic Query Error");
    }
    return 1;
}
char CDBManager::UpdateGuildWarPointList(int serverId, int rank)
{
    bool ret;
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e3d,
                      "upDate guild_info set guild_war_point = 1000 where server_id = %d and expire_flag = 0 and guild_rank <= %d",
                      serverId, 0xa))
    {
        CMyFileLog log(__FUNCTION__, 0x9d2);
        log("./log/DBQueryErr",
            "CDBManager::UpdateGuildWarPointList() update guild_info set guild_war_point = 1000 where server_id = %d and expire_flag = 0 and guild_rank <= %d",
            serverId, 0xa);
        return 0;
    }
    ret = h->exec(0x4e3d);
    if (!ret)
        return 0;
    return 1;
}
char CDBManager::UpdateResetGuildPoint(int serverId)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e36,
                      "upDate guild_info set guild_point = 0 , guild_war_point = 0 where server_id = %d and expire_flag = 0",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x72b);
        log("./log/DBQueryErr",
            "CDBManager::UpdateResetGuildPoint() Fatal Error Break : update guild_info set guild_point = 0 where server_id = %d and expire_flag = 0\n",
            serverId);
    }
    if (!h->exec(0x4e36))
        return 0;
    if (!h->set_query(0x4e38,
                      "upDate guild_member set member_point = 0 where server_id = %d",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x737);
        log("./log/DBQueryErr",
            "CDBManager::UpdateResetGuildPoint() Fatal Error Break : update guild_member set member_point = 0 where server_id = %d\n",
            serverId);
    }
    if (!h->exec(0x4e38))
        return 0;
    return 1;
}
char CDBManager::UpdateAccumulateGuildPoint(int serverId)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e35,
                      "upDate guild_info set guild_point_acc = guild_point_acc + guild_point, guild_point_prev = guild_point where server_id = %d and expire_flag = 0",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x707);
        log("./log/DBQueryErr",
            "CDBManager::UpdateGuildRank() Fatal Error Break : update guild_info set guild_point_acc = guild_point_acc + guild_point, guild_point_prev = guild_point where server_id = %d and expire_flag = 0\n",
            serverId);
    }
    if (!h->exec(0x4e35))
        return 0;
    if (!h->set_query(0x4e37,
                      "upDate guild_member set member_point_prev = member_point where server_id = %d",
                      serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x712);
        log("./log/DBQueryErr",
            "CDBManager::UpdateGuildRank() Fatal Error Break : update guild_member set member_point_prev = member_point where server_id = %d\n",
            serverId);
    }
    if (!h->exec(0x4e37))
        return 0;
    return 1;
}
char CDBManager::ChangeCharName(Packet_DBMW_Change_Char_Name* packet)
{
    CDBHandle* h = m_handles[3];    // game db
    if (!h->set_query(0x4e85,
                      "upDate charac_black_list set charac_name='%s' where charac_no=%d",
                      packet->m_name, packet->m_characNo))
    {
        unsigned int characNo = packet->m_characNo;
        char* name = packet->m_name;
        CMyFileLog log(__FUNCTION__, 0x1392);
        log("./log/DBQueryErr",
            "CDBManager::ChangeCharName() : upDate charac_black_list set charac_name='%s' where charac_no=%d",
            name, characNo);
        return 0;
    }
    bool ret = h->exec(0x4e85);
    if (!ret)
        return 0;
    return 1;
}
char CDBManager::ChangePvPBuddyName(Packet_DBMW_Change_Char_Name* packet)
{
    CDBHandle* h = m_handles[9];    // +0x24
    if (!h->set_query(0x4efa,
                      "upDate pvp_buddy set buddy_charac_name='%s' where buddy_server_id=%d and buddy_charac_no=%d",
                      packet->m_name, packet->m_serverId, packet->m_characNo))
    {
        CMyFileLog log(__FUNCTION__, 0x13b0);
        log("./log/DBQueryErr",
            "CDBManager::ChangeCharacName() : upDate pvp_buddy set charac_name='%s' where server_id=%d and charac_no=%d",
            packet->m_name, packet->m_serverId, packet->m_characNo);
        return 0;
    }
    if (!h->exec(0x4efa))
        return 0;
    return 1;
}
bool CDBManager::DeleteJoinListByInvite(unsigned int guildId,
                                        unsigned int characNo)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4f0c,
                      "deLete from guild_join_list where guild_id=%d and charac_no=%d",
                      guildId, characNo))
    {
        CMyFileLog(__FUNCTION__, 0xe54)("./log/DBQueryErr",
            "set_query(deLete_from_guild_join_list) Query Error");
        return 0;
    }
    if (!h->exec(0x4f0c))
    {
        CMyFileLog(__FUNCTION__, 0xe5a)("./log/DBQueryErr",
            "guild_db->exec(deLete_from_guild_join_list) Query Error");
        return 0;
    }
    return 1;
}
char CDBManager::OnUpgradeGuildCargo(Packet_DB_Guild_Cargo_Upgrade* packet)
{
    bool ret;
    CDBHandle* h = m_handles[8];    // guild db
    h->set_query(0x4edc,
                 "upDate guild_agit set cargo_capacity=%d where guild_id=%d",
                 ((GuildCargoUpgradeView*)packet)->m_field12,
                 ((GuildCargoUpgradeView*)packet)->m_fieldA);
    ret = h->exec(0x4edc);
    if (!ret)
    {
        register unsigned int c =
            ((GuildCargoUpgradeView*)packet)->m_field12;
        register unsigned int u =
            ((GuildCargoUpgradeView*)packet)->m_fieldE;
        register unsigned int g =
            ((GuildCargoUpgradeView*)packet)->m_fieldA;
        CMyFileLog log(__FUNCTION__, 0x1bec);
        log("./log/DBQueryErr",
            "OnUpgradeGuildCargo Query Error(G:%d,U:%d,Capa:%d)",
            g, u, c);
        return 0;
    }
    return 1;
}
char CDBManager::OnUpdateGuildCargo(Packet_DB_Update_Guild_Cargo* packet)
{
    CDBHandle* h = m_handles[8];    // guild db
    h->set_query(0x4ecb,
                 "upDate guild_agit set cargo='%s' where guild_id=%d",
                 h->blob_to_str(0, (char*)packet + 0x12, 0x18d8),
                 *(int*)((char*)packet + 0xa));
    bool ret = h->exec(0x4ecb);
    if (!ret)
    {
        CMyFileLog log(__FUNCTION__, 0x1b90);
        log("./log/DBQueryErr", "OnUpdateGuildCargo Query Error");
        return 0;
    }
    return 1;
}
char CDBManager::OnStatisticNumOfOccupations(
    Packet_DBMW_Statistic_Login_Logout* packet)
{
    bool ret;
    CDBHandle* h = m_handles[2];    // game db
    h->set_query(0x4eec,
                 "inSert into log_num_occupations(occ_time,num_occupations_charscreen,num_occupations_seriaroom,num_login_per_min,num_logout_per_min) values (now(),%d,%d,%d,%d)",
                 packet->m_field608, packet->m_field60c,
                 packet->m_field610, packet->m_field614);
    ret = h->exec(0x4eeb);
    if (!ret)
    {
        CMyFileLog log(__FUNCTION__, 0x20b6);
        log("./log/Statistics", "OnStatisticNumOfOccupations db insert error");
    }
    return 1;
}
Packet_Result_OnTimeEvent_Idx::Packet_Result_OnTimeEvent_Idx()
    : PacketHeader(0x2341, 0xf)
{
    m_fieldA = 0;
    m_fieldE = 0;
}
char CDBManager::OnStatisticLoginLogout(
    Packet_DBMW_Statistic_Login_Logout* packet)
{
    CDBHandle* h = m_handles[4];    // log db
    time_t now = time(0);
    for (int i = 0; i < *(int*)((char*)packet + 0xa); i++)
    {
        h->set_query(0x4eeb,
                     "inSert into log_login_logout(occ_time,channel_no,event_type,count) values (from_unixtime(%d),%d,%d,%d)",
                     now, *(unsigned char*)((char*)packet + i * 6 + 0xe),
                     *(unsigned char*)((char*)packet + i * 6 + 0xf),
                     *(int*)((char*)packet + i * 6 + 0x10));
        if (!h->exec(0x4eeb))
        {
            CMyFileLog log(__FUNCTION__, 0x2099);
            log("./log/Statistics", "OnStatisticLoginLogout db insert error");
        }
    }
    return 1;
}
char CDBManager::QueryOnTimeEventIdx(Packet_Result_OnTimeEvent_Idx& rpy)
{
    CDBHandle* h = m_handles[0xd];    // se_event db
    rpy.m_fieldA = 0;
    if (!h->set_query(0x4f14,
                      "seLect ifnull(max(no), 1) from event_1112_ontime_info"))
    {
        CMyFileLog log(__FUNCTION__, 0x244e);
        log("./log/DBQueryErr", "set_query(seLect_from_event_ontime_idx) Query Error");
        return 0;
    }
    if (!h->exec(0x4f14) || !h->fetch())
        return 0;
    if (!h->get_uint(0, *(unsigned int*)((char*)&rpy + 0xa)))
        return 0;
    return 1;
}
Packet_Frame_Lag_Collect_Interval_Check::Packet_Frame_Lag_Collect_Interval_Check()
    : PacketHeader(0xc2f, 0xc)
{
}
Packet_Frame_Lag_Statistic_Result_Reload_Spec::
    Packet_Frame_Lag_Statistic_Result_Reload_Spec()
    : PacketHeader(0xc2a, 0xe5)
{
}
Packet_Frame_Lag_Statistic_Result_Load_Spec::Packet_Frame_Lag_Statistic_Result_Load_Spec()
    : PacketHeader(0xc28, 0xe5)
{
}
Packet_Frame_Lag_Spec_Delete_Notify::Packet_Frame_Lag_Spec_Delete_Notify()
    : PacketHeader(0xc2e, 0xe)
{
}
char CDBManager::QueryReloadSpecDb(Packet_Frame_Lag_Statistic_Reload_Spec* req,
                                   CStatisticsServer* stats)
{
    CDBHandle* h = m_handles[0xf];    // frame_lag db
    if (!h)
        return 0;
    h->set_query(0x4e8f,
                 "seLect value from collect_interval where start_time <= now() and now() <= end_time order by start_time limit 1");
    if (!h->exec(0x4e8f))
        return 0;
    Packet_Frame_Lag_Collect_Interval_Check pkt;
    if (h->get_n_rows() != 0)
    {
        if (!h->fetch())
            return 0;
        if (!h->get_short(0, *(short*)((char*)&pkt + 0xa)))
            return 0;
    }
    else
    {
        pkt.m_fieldA = 0;
    }
    stats->SendToServer((char*)&pkt, pkt.packetSize);
    h->set_query(0x4e8a,
                 "seLect unique_id,unix_timestamp(modify_time),spec_id,cpu_vendor,cpu_processor_num,above_cpu_clock,below_cpu_clock,ram,videocard_vendor,videocard_device,videocard_texture_mem,os_version from monitoring_spec where unix_timestamp(modify_time)>%d",
                 *(int*)((char*)req + 0xb));
    if (!h->exec(0x4e8a))
        return 0;
    int n_rows = h->get_n_rows();
    if (n_rows <= 0)
        return 1;
    Packet_Frame_Lag_Statistic_Result_Reload_Spec rp;
    rp.m_fieldA = req->m_fieldA;
    int count = n_rows / 6;
    if (n_rows % 6 != 0)
        count++;
    *(int*)((char*)&rp + 0xf) = count;
    *(int*)((char*)&rp + 0xb) = 1;
    int i = 0;
    for (int j = 0; j < n_rows; j++)
    {
        if (!h->fetch())
            return 0;
        if (!h->get_int(0, *(int*)((char*)&rp + (i + 4) * 4 + 3)))
            return 0;
        if (!h->get_uint(1, *(unsigned int*)((char*)&rp + (i + 8) * 4 + 0xb)))
            return 0;
        if (!h->get_int(2, *(int*)((char*)&rp + (i + 0x10) * 4 + 3)))
            return 0;
        if (!h->get_byte(3, *(char*)((char*)&rp + 0x50 + i + 0xb)))
            return 0;
        if (!h->get_byte(4, *(char*)((char*)&rp + 0x60 + i + 1)))
            return 0;
        if (!h->get_int(5, *(int*)((char*)&rp + (i + 0x18) * 4 + 7)))
            return 0;
        if (!h->get_int(6, *(int*)((char*)&rp + (i + 0x1c) * 4 + 0xf)))
            return 0;
        if (!h->get_short(7, *(short*)((char*)&rp + (i + 0x48) * 2 + 7)))
            return 0;
        if (!h->get_int(8, *(int*)((char*)&rp + (i + 0x28) * 4 + 3)))
            return 0;
        if (!h->get_int(9, *(int*)((char*)&rp + (i + 0x2c) * 4 + 0xb)))
            return 0;
        if (!h->get_short(10, *(short*)((char*)&rp + (i + 0x68) * 2 + 3)))
            return 0;
        if (!h->get_byte(11, *(char*)((char*)&rp + 0xd0 + i + 0xf)))
            return 0;
        i++;
        if (i % 6 == 0)
        {
            stats->SendToServer((char*)&rp, rp.packetSize);
            DNFFLib::Sleep_Ext(0, 1);
            stats->SendToServer((char*)&rp, rp.packetSize);
            *(int*)((char*)&rp + 0xb) += 1;
            i = 0;
        }
    }
    if (i != 0)
    {
        if (i > 0 && i <= 5)
            *(int*)((char*)&rp + (i + 4) * 4 + 3) = -1;
        stats->SendToServer((char*)&rp, rp.packetSize);
        DNFFLib::Sleep_Ext(0, 1);
        stats->SendToServer((char*)&rp, rp.packetSize);
        i = 0;
    }
    return 1;
}
char CDBManager::InsertFrameLagStatistics(
    Packet_Frame_Lag_Statistic_Write_Lag_Index* packet, CStatisticsServer* stats)
{
    CDBHandle* h = m_handles[0xf];    // frame_lag db
    if (!h)
        return 0;
    h->set_query(0x4e8c,
                 "inSert into common_index(spec_id,occ_time,server_group,share_rate,crash_village,crash_dungeon,crash_challenge,crash_wararea,crash_fight_village,crash_dead_tower,crash_channel,crash_load,village_to_dungeon_lag,dungeon_to_village_lag) values(%d,from_unixtime(%d),%hhd,%u,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hd,%hd)",
                 *(int*)((char*)packet + 0xb),
                 *(unsigned int*)((char*)packet + 0x177),
                 *(signed char*)((char*)packet + 0xa),
                 *(unsigned int*)((char*)packet + 0xf),
                 *(unsigned short*)((char*)packet + 0x13),
                 *(unsigned short*)((char*)packet + 0x15),
                 *(unsigned short*)((char*)packet + 0x17),
                 *(unsigned short*)((char*)packet + 0x19),
                 *(unsigned short*)((char*)packet + 0x1b),
                 *(unsigned short*)((char*)packet + 0x1d),
                 *(unsigned short*)((char*)packet + 0x1f),
                 *(unsigned short*)((char*)packet + 0x21),
                 *(short*)((char*)packet + 0x23),
                 *(short*)((char*)packet + 0x25));
    if (!h->exec(0x4e8c))
        return 0;
    char buf[0x20];
    for (int kind = 0; kind <= 5; kind++)
    {
        switch (kind)
        {
        case 0:
            strncpy(buf, "village_lag_index", 0x20);
            break;
        case 1:
            strncpy(buf, "dungeon_lag_index", 0x20);
            break;
        case 2:
            strncpy(buf, "challenge_lag_index", 0x20);
            break;
        case 3:
            strncpy(buf, "wararea_lag_index", 0x20);
            break;
        case 4:
            strncpy(buf, "fight_village_lag_index", 0x20);
            break;
        case 5:
            strncpy(buf, "dead_tower_lag_index", 0x20);
            break;
        default:
            memcpy(buf, "___MAX_FRAME_LAG_STATISTISCS_KIND over", 0x20);
            break;
        }
        h->set_query(0x4e8d,
                     "inSert into %s(spec_id,occ_time,server_group,share_rate,win_fps,full_fps,full_win_fps,full_win_nosync_fps,frame1,time1,frame2,time2,frame3,time3,frame4,time4,frame5,time5,frame6,time6) values(%d,from_unixtime(%d),%hhd,%u,%hd,%hd,%hd,%hd,%d,%.3f,%d,%.3f,%d,%.3f,%d,%.3f,%d,%.3f,%d,%.3f)",
                     buf,
                     *(int*)((char*)packet + 0xb),
                     *(unsigned int*)((char*)packet + 0x177),
                     *(signed char*)((char*)packet + 0xa),
                     *(unsigned int*)((char*)packet + 0xf),
                     *(short*)((char*)packet + kind * 0x38 + 0x27),
                     *(short*)((char*)packet + kind * 0x38 + 0x29),
                     *(short*)((char*)packet + kind * 0x38 + 0x2b),
                     *(short*)((char*)packet + kind * 0x38 + 0x2d),
                     *(int*)((char*)packet + kind * 0x38 + 0x2f),
                     *(float*)((char*)packet + kind * 0x38 + 0x33),
                     *(int*)((char*)packet + kind * 0x38 + 0x37),
                     *(float*)((char*)packet + kind * 0x38 + 0x3b),
                     *(int*)((char*)packet + kind * 0x38 + 0x3f),
                     *(float*)((char*)packet + kind * 0x38 + 0x43),
                     *(int*)((char*)packet + kind * 0x38 + 0x47),
                     *(float*)((char*)packet + kind * 0x38 + 0x4b),
                     *(int*)((char*)packet + kind * 0x38 + 0x4f),
                     *(float*)((char*)packet + kind * 0x38 + 0x53),
                     *(int*)((char*)packet + kind * 0x38 + 0x57),
                     *(float*)((char*)packet + kind * 0x38 + 0x5b));
        if (!h->exec(0x4e8d))
            return 0;
    }
    h->set_query(0x4e8e,
                 "select unique_id from monitoring_spec where spec_id = %d",
                 *(int*)((char*)packet + 0xb));
    if (!h->exec(0x4e8e))
        return 0;
    if (h->get_n_rows() != 0)
        return 1;
    Packet_Frame_Lag_Spec_Delete_Notify pkt;
    *(int*)((char*)&pkt + 0xb) = *(int*)((char*)packet + 0xb);
    stats->SendToServer((char*)&pkt, pkt.packetSize);
    return 1;
}
char CDBManager::QueryFirstLoadSpecDb(Packet_Frame_Lag_Statistic_Load_Spec* req,
                                      CStatisticsServer* stats)
{
    CDBHandle* h = m_handles[0xf];    // frame_lag db
    if (!h)
        return 0;
    h->set_query(0x4e89,
                 "seLect unique_id,unix_timestamp(modify_time),spec_id,cpu_vendor,cpu_processor_num,above_cpu_clock,below_cpu_clock,ram,videocard_vendor,videocard_device,videocard_texture_mem,os_version from monitoring_spec");
    if (!h->exec(0x4e89))
        return 0;
    int n_rows = h->get_n_rows();
    if (n_rows <= 0)
        return 1;
    Packet_Frame_Lag_Statistic_Result_Load_Spec rp;
    rp.m_fieldA = req->m_fieldA;
    int count = n_rows / 6;
    if (n_rows % 6 != 0)
        count++;
    *(int*)((char*)&rp + 0xf) = count;
    *(int*)((char*)&rp + 0xb) = 1;
    int i = 0;
    for (int j = 0; j < n_rows; j++)
    {
        if (!h->fetch())
            return 0;
        if (!h->get_int(0, *(int*)((char*)&rp + (i + 4) * 4 + 3)))
            return 0;
        if (!h->get_uint(1, *(unsigned int*)((char*)&rp + (i + 8) * 4 + 0xb)))
            return 0;
        if (!h->get_int(2, *(int*)((char*)&rp + (i + 0x10) * 4 + 3)))
            return 0;
        if (!h->get_byte(3, *(char*)((char*)&rp + 0x50 + i + 0xb)))
            return 0;
        if (!h->get_byte(4, *(char*)((char*)&rp + 0x60 + i + 1)))
            return 0;
        if (!h->get_int(5, *(int*)((char*)&rp + (i + 0x18) * 4 + 7)))
            return 0;
        if (!h->get_int(6, *(int*)((char*)&rp + (i + 0x1c) * 4 + 0xf)))
            return 0;
        if (!h->get_short(7, *(short*)((char*)&rp + (i + 0x48) * 2 + 7)))
            return 0;
        if (!h->get_int(8, *(int*)((char*)&rp + (i + 0x28) * 4 + 3)))
            return 0;
        if (!h->get_int(9, *(int*)((char*)&rp + (i + 0x2c) * 4 + 0xb)))
            return 0;
        if (!h->get_short(10, *(short*)((char*)&rp + (i + 0x68) * 2 + 3)))
            return 0;
        if (!h->get_byte(11, *(char*)((char*)&rp + 0xd0 + i + 0xf)))
            return 0;
        i++;
        if (i % 6 == 0)
        {
            stats->SendToServer((char*)&rp, rp.packetSize);
            DNFFLib::Sleep_Ext(0, 1);
            stats->SendToServer((char*)&rp, rp.packetSize);
            *(int*)((char*)&rp + 0xb) += 1;
            i = 0;
        }
    }
    if (i != 0)
    {
        if (i > 0 && i <= 5)
            *(int*)((char*)&rp + (i + 4) * 4 + 3) = -1;
        stats->SendToServer((char*)&rp, rp.packetSize);
        DNFFLib::Sleep_Ext(0, 1);
        stats->SendToServer((char*)&rp, rp.packetSize);
        i = 0;
    }
    return 1;
}
bool CDBManager::QueryTodayGuildMember(unsigned int guildId,
                                       Packet_Reply_Today_Guild_Member& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    time_t now = time(0);
    localtime(&now);
    *(unsigned int*)((char*)&reply + 0xa) = guildId;
    unsigned int i = 0;
    std::vector<STTodayGuildMember> vec;
    vec.clear();
    if (!h->set_query(0x4f05,
                      "seLect charac_no,charac_name,grade,job,grow_type,sex,lev from guild_member where guild_id = %d and member_flag = 1 and grade != 0",
                      guildId))
        return 0;
    if (!h->exec(0x4f05))
        return 0;
    while (i < (unsigned int)h->get_n_rows())
    {
        STTodayGuildMember member;
        memset(&member, 0, 0x27);
        if (!h->fetch())
            return 0;
        if (!h->get_uint(0, member.m_field0))
            return 0;
        if (!h->get_str(1, member.m_name, 0x1d))
            return 0;
        if (!h->get_byte(2, *(char*)&member.m_field22))
            return 0;
        if (!h->get_byte(3, *(char*)&member.m_field23))
            return 0;
        if (!h->get_byte(4, *(char*)&member.m_field24))
            return 0;
        if (!h->get_byte(5, *(char*)&member.m_field25))
            return 0;
        if (!h->get_byte(6, *(char*)&member.m_field26))
            return 0;
        vec.push_back(member);
        i++;
    }
    if (vec.size() <= 0x13)
        return 1;
    STTodayGuildMember& m = vec[rand() % vec.size()];
    *(STTodayGuildMember*)((char*)&reply + 0xe) = m;
    vec.clear();
    return 1;
}
char CDBManager::QueryHWspecCreate(
    Packet_DBMW_Save_Client_Spec_Statistic* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (*(unsigned char*)((char*)packet + 0xa) == 0)
    {
        int count = *(int*)((char*)packet + 0xb);
        for (int i = 0; i < count; i++)
        {
            char* e = (char*)packet + 0xf + i * 0xe;
            h->set_query(0x4e78,
                         "upDate log_hardware_ting set total=%d where occ_time=from_unixtime(%d) and category1=%d and category2=%d and category3=%d",
                         *(unsigned short*)(e + 0), now,
                         *(unsigned char*)(e + 2), *(int*)(e + 6),
                         *(int*)(e + 0xa));
            if (h->exec(0x4e78) != 1 || h->getAffectedRowCount() == 0)
            {
                h->set_query(0x4e79,
                             "inSert into log_hardware_ting(occ_time, category1, category2, category3, total) values(from_unixtime(%d), %d, %d, %d, %d)",
                             now, *(unsigned char*)(e + 2), *(int*)(e + 6),
                             *(int*)(e + 0xa), *(unsigned short*)(e + 0));
                if (!h->exec(0x4e79))
                    return 0;
            }
        }
    }
    else if (*(unsigned char*)((char*)packet + 0xa) == 1)
    {
        int count = *(int*)((char*)packet + 0xb);
        for (int i = 0; i < count; i++)
        {
            char* e = (char*)packet + 0xf + i * 0xe;
            h->set_query(0x4e7a,
                         "upDate log_hardware_ting set ting=%d where occ_time=from_unixtime(%d) and category1=%d and category2=%d and category3=%d",
                         *(unsigned short*)(e + 0), now,
                         *(unsigned char*)(e + 2), *(int*)(e + 6),
                         *(int*)(e + 0xa));
            if (h->exec(0x4e7a) != 1 || h->getAffectedRowCount() == 0)
            {
                h->set_query(0x4e7b,
                             "inSert into log_hardware_ting(occ_time, category1, category2, category3, ting) values(from_unixtime(%d), %d, %d, %d, %d)",
                             now, *(unsigned char*)(e + 2), *(int*)(e + 6),
                             *(int*)(e + 0xa), *(unsigned short*)(e + 0));
                if (!h->exec(0x4e7b))
                    return 0;
            }
        }
    }
    else
    {
        int count = *(int*)((char*)packet + 0xb);
        for (int i = 0; i < count; i++)
        {
            char* e = (char*)packet + 0xf + i * 0xe;
            h->set_query(0x4e7c,
                         "upDate log_hardware_ting_low set total=%d where occ_time=from_unixtime(%d) and category1=%d and category2=%d and category3=%d",
                         *(unsigned short*)(e + 0), now,
                         *(unsigned char*)(e + 2), *(int*)(e + 6),
                         *(int*)(e + 0xa));
            if (h->exec(0x4e7c) != 1 || h->getAffectedRowCount() == 0)
            {
                h->set_query(0x4e7d,
                             "inSert into log_hardware_ting_low(occ_time, category1, category2, category3, total) values(from_unixtime(%d), %d, %d, %d, %d)",
                             now, *(unsigned char*)(e + 2), *(int*)(e + 6),
                             *(int*)(e + 0xa), *(unsigned short*)(e + 0));
                if (!h->exec(0x4e7d))
                    return 0;
            }
        }
    }
    return 1;
}
char CDBManager::OnLoadGuildCargoHistory(
    unsigned int guildId, Packet_Guild_Load_Guild_Cargo_History& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    char buf[0x100] = {0};
    sprintf(buf, "guild_cargo_history_%d", guildId % 10);
    h->set_query(0x4ed8,
                 "seLect occ_time,behavior,charac_name,item_id,add_info,random_option from %s where guild_id=%d order by occ_time desc limit %d",
                 buf, guildId, 0x32);
    if (!h->exec(0x4ed8))
    {
        CMyFileLog log(__FUNCTION__, 0x1b63);
        log("./log/DBQueryErr", "OnLoadGuildCargoHistory Query Error");
        return 0;
    }
    *(int*)((char*)&reply + 0xe) = h->get_n_rows();
    unsigned int j = 0;
    while (j < (unsigned int)h->get_n_rows())
    {
        if (!h->fetch())
            return 0;
        char* base = (char*)&reply + 0x12 + j * 0x30;
        if (!h->get_int(0, *(int*)(base + 0)))
            return 0;
        if (!h->get_byte(1, *(char*)(base + 4)))
            return 0;
        if (!h->get_str(2, base + 5, 0x15))
            return 0;
        if (!h->get_int(3, *(int*)(base + 0x1a)))
            return 0;
        if (!h->get_int(4, *(int*)(base + 0x1e)))
            return 0;
        if (!h->get_binary(5, base + 0x22, 0xe))
            return 0;
        j++;
    }
    return 1;
}
bool CDBManager::DeleteToBlackList(unsigned int m_id, unsigned int characNo)
{
    CDBHandle* h = m_handles[3];    // game db
    if (!h->set_query(0x4e40,
                      "deLete from charac_black_list where m_id = %s and charac_no = %d",
                      NumberToString(m_id, 0), characNo))
        return 0;
    if (!h->exec(0x4e40))
        return 0;
    if (!h->set_query(0x4ed6,
                      "seLect black_point,offset_point from charac_black_info where charac_no=%d",
                      characNo))
    {
        CMyFileLog log(__FUNCTION__, 0xa6c);
        log("./log/BlackListModify",
            "CDBManager::DeleteToBlackList() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    if (!h->exec(0x4ed6))
        return 0;
    if (h->get_n_rows() > 1)
    {
        CMyFileLog log(__FUNCTION__, 0xa74);
        log("./log/BlackListModify",
            "CDBManager::seLect_black_point_offset_point_from_charac_black_info() idata > 1 seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
    }
    if (!h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0xa7a);
        log("./log/BlackListModify",
            "CDBManager::seLect_black_point_offset_point_from_charac_black_info() !db->fetch() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    int blackPoint = 0;
    int offsetPoint = 0;
    if (!h->get_int(0, blackPoint))
    {
        CMyFileLog log(__FUNCTION__, 0xa82);
        log("./log/BlackListModify",
            "CDBManager::DeleteToBlackList() !db->fetch() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    if (!h->get_int(1, offsetPoint))
    {
        CMyFileLog log(__FUNCTION__, 0xa87);
        log("./log/BlackListModify",
            "CDBManager::DeleteToBlackList() !db->fetch() seLect black_point,offset_point from charac_black_info where charac_no=%d",
            characNo);
        return 0;
    }
    if (blackPoint - offsetPoint > 0)
    {
        if (!h->set_query(0x4e42,
                          "upDate charac_black_info set black_point = black_point - 1 where charac_no = %d",
                          characNo))
        {
            CMyFileLog log(__FUNCTION__, 0xa91);
            log("./log/BlackListModify",
                "CDBManager::DeleteToBlackList() upDate charac_black_info set black_point = black_point - 1 where charac_no = %d",
                characNo);
            return 0;
        }
        if (!h->exec(0x4e42))
            return 0;
    }
    return 1;
}
char CDBManager::OnLoadGuildBoard(int guildId, int& count,
                                  STGuildBoardDBInfo* boards)
{
    CDBHandle* h = m_handles[5];    // sso db
    if (!h->set_query(0x4f07,
                      "seLect no, m_id, charac_no, charac_name, memo, unix_timestamp(create_time), job from guild_memo where guild_id=%d order by no desc limit %d",
                      guildId, 0x32))
    {
        CMyFileLog log(__FUNCTION__, 0x2292);
        log("./log/DBQueryErr", "OnLoadGuildBoard Query Error");
        return 0;
    }
    if (!h->exec(0x4f07))
        return 0;
    count = h->get_n_rows();
    if (count == 0)
        return 0;
    unsigned int i = 0;
    while (i < (unsigned int)count)
    {
        unsigned int m_id = 0;
        if (!h->fetch())
            return 0;
        if (!h->get_uint(0, *(unsigned int*)((char*)boards + i * 0xa5 + 0x7c)))
            return 0;
        if (!h->get_uint(1, m_id))
            return 0;
        if (!h->get_uint(2, *(unsigned int*)((char*)boards + i * 0xa5 + 0x80)))
            return 0;
        if (!h->get_str(3, (char*)boards + i * 0xa5 + 0x87, 0x1e))
            return 0;
        if (!h->get_str(4, (char*)boards + i * 0xa5, 0x78))
            return 0;
        if (!h->get_uint(5, *(unsigned int*)((char*)boards + i * 0xa5 + 0x78)))
            return 0;
        if (!h->get_byte(6, *(char*)((char*)boards + i * 0xa5 + 0x84)))
            return 0;
        if (m_id == 0)
            *(unsigned int*)((char*)boards + i * 0xa5 + 0x80) = 0;
        i++;
    }
    return 1;
}
bool CDBManager::selectCollectItems(unsigned char serverInfo, int& curCount,
                                    int& totalCount, unsigned int& changeFlag,
                                    unsigned char& fullTime)
{
    CDBHandle* h = m_handles[9];    // +0x24
    if (!h)
        return 0;
    if (!h->set_query(0x4f4c,
                      "seLect cur_count, total_count, change_flag, unix_timestamp(full_time) from collect_items where server_info = %d",
                      serverInfo))
    {
        CMyFileLog log(__FUNCTION__, 0x2977);
        log("./log/DBQueryErr",
            "seLect cur_count, total_count from collect_items Error");
        return 0;
    }
    if (!h->exec(0x4f4c))
    {
        CMyFileLog log(__FUNCTION__, 0x2981);
        log("./log/DBQueryErr", "selectCollectItems Query(exec) Error");
        return 0;
    }
    if (h->get_n_rows() == 0)
    {
        CMyFileLog log(__FUNCTION__, 0x2987);
        log("./log/DBQueryErr", "selectCollectItems (Row_Data Not Exist) Error");
        return 0;
    }
    if (!h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0x298e);
        log("./log/DBQueryErr", "selectCollectItems Query(fetch) Error");
        return 0;
    }
    if (!h->get_int(0, curCount))
    {
        CMyFileLog log(__FUNCTION__, 0x2997);
        log("./log/DBQueryErr", "selectCollectItems (get_uint(cur_count_)) Error");
        return 0;
    }
    if (!h->get_int(1, totalCount))
    {
        CMyFileLog log(__FUNCTION__, 0x299e);
        log("./log/DBQueryErr",
            "selectCollectItems (get_uint(total_count_) Error");
        return 0;
    }
    if (!h->get_ubyte(2, fullTime))
    {
        CMyFileLog log(__FUNCTION__, 0x29a5);
        log("./log/DBQueryErr", "selectCollectItems (get_ubyte(change_flag) Error");
        return 0;
    }
    if (!h->get_uint(3, changeFlag))
    {
        CMyFileLog log(__FUNCTION__, 0x29ac);
        log("./log/DBQueryErr",
            "selectCollectItems (get_uint(total_count_) Error");
        return 0;
    }
    return 1;
}
bool CDBManager::updateNexonPinPcRoomPlayTimeEvent(
    unsigned char serverInfo, unsigned int m_id, unsigned int& pinNo,
    char* nexonPin, unsigned int len)
{
    CDBHandle* h = m_handles[9];    // +0x24
    if (!h)
        return 0;
    if (!h->set_query(0x4f4e,
                      "seLect no, nexon_pin from event_pcroom_time_nexon_cash where server_info = %d and m_id = 0 order by no asc limit 1",
                      serverInfo))
    {
        CMyFileLog log(__FUNCTION__, 0x2a16);
        log("./log/DBQueryErr", "seLect NexonPinPcRoomPlayTime set Error");
        return 0;
    }
    if (!h->exec(0x4f4e))
    {
        CMyFileLog log(__FUNCTION__, 0x2a1e);
        log("./log/DBQueryErr",
            "selectNexonPinPcRoomPlayTime Query(exec) Error");
        return 0;
    }
    if (h->get_n_rows() == 0)
    {
        CMyFileLog log(__FUNCTION__, 0x2a24);
        log("./log/DBQueryErr",
            "selectNexonPinPcRoomPlayTime (Row_Data Not Exist) Error");
        return 0;
    }
    if (!h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0x2a2b);
        log("./log/DBQueryErr",
            "selectNexonPinPcRoomPlayTime Query(fetch) Error");
        return 0;
    }
    if (!h->get_uint(0, pinNo))
    {
        CMyFileLog log(__FUNCTION__, 0x2a32);
        log("./log/DBQueryErr",
            "selectNexonPinPcRoomPlayTime (get_uint(pin_num)) Error");
        return 0;
    }
    if (!h->get_str(1, nexonPin, len))
    {
        CMyFileLog log(__FUNCTION__, 0x2a39);
        log("./log/DBQueryErr",
            "selectNexonPinPcRoomPlayTime (get_str(nexon_pin)) Error");
        return 0;
    }
    if (!h->set_query(0x4f4f,
                      "upDate event_pcroom_time_nexon_cash set m_id = %d, occ_date = now() where no = %d",
                      m_id, pinNo))
    {
        CMyFileLog log(__FUNCTION__, 0x2a43);
        log("./log/DBQueryErr", "upDate NexonPinPcRoomPlayTime set Error");
        return 0;
    }
    if (!h->exec(0x4f4f))
    {
        CMyFileLog log(__FUNCTION__, 0x2a4b);
        log("./log/DBQueryErr",
            "upDate updateNexonPinPcRoomPlayTime Query(exec) Error");
        return 0;
    }
    return 1;
}
char CDBManager::OnSaveAssertManagerInfoWrite(
    Packet_DBMW_Assert_Manager_Info_Write_Query* packet)
{
    CDBHandle* h = m_handles[0xf];    // frame_lag db
    if (!h)
        return 0;
    int count = *(int*)((char*)packet + 0xa);
    for (int i = 0; i < count; i++)
    {
        char buf1[0x400];
        char buf2[0x100];
        char buf3[0x100];
        memset(buf1, 0, 0x400);
        memset(buf2, 0, 0x100);
        char* entry = (char*)packet + i * 0x206;
        if (strlen(entry + 0xe) > 0xfe || entry[0xe] == 0)
        {
            CMyFileLog log(__FUNCTION__, 0x1a0e);
            log("./log/Statistics", "Assert Manager Error : %s", entry + 0xe);
            continue;
        }
        h->escape_string(buf2, entry + 0xe);
        memset(buf3, 0, 0x100);
        h->escape_string(buf3, entry + 0x114);
        sprintf(buf1,
                "upDate assert_manager set cnt=cnt+%d where file_name='%s' and file_line=%d and reason='%s'",
                *(int*)(entry + 0x110), buf2,
                *(unsigned short*)(entry + 0x10e), buf3);
        h->set_query(0x4eb8, "%s", buf1);
        if (!h->exec(0x4eb8))
        {
            memset(buf1, 0, 0x400);
            sprintf(buf1,
                    "inSert into assert_manager (file_name, file_line, reason, cnt) values ('%s', %d, '%s', %d)",
                    buf2, *(unsigned short*)(entry + 0x10e), buf3,
                    *(int*)(entry + 0x110));
            h->set_query(0x4eb7, "%s", buf1);
            h->exec(0x4eb7);
        }
        CMyFileLog log(__FUNCTION__, 0x1a2e);
        log("./log/Statistic", "Exec Query : %s", buf1);
    }
    return 1;
}
char CDBManager::QueryCubeStatisticCreate(Packet_DBMW_Cube_Statistic* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    int count = *(int*)((char*)packet + 0xa);
    CMyFileLog slog("QueryCubeStatisticCreate", 0x1872);
    slog("./log/statistic",
         "CDBManager::QueryCubeStatisticCreate : (%d) °³ ÆÐÅ¶ ¼ö½Å\n", count);
    char buf[0x800] = {0};
    std::string str;
    for (int i = 0; i < count; i++)
    {
        char* e = (char*)packet + i * 0xd;
        if (str.size() != 0)
        {
            sprintf(buf, ",(now(),%d,%d,%d,%d,%d)", *(short*)(e + 0xe),
                    *(short*)(e + 0x10), *(int*)(e + 0x12),
                    *(unsigned char*)(e + 0x1a), *(int*)(e + 0x16));
        }
        else
        {
            sprintf(buf, "(now(),%d,%d,%d,%d,%d)", *(short*)(e + 0xe),
                    *(short*)(e + 0x10), *(int*)(e + 0x12),
                    *(unsigned char*)(e + 0x1a), *(int*)(e + 0x16));
        }
        if (str.length() + 0x800 > 0x6000)
        {
            h->set_query(0x4ec2,
                         "inSert into log_cube_stat (occ_time, channel_no, level, item_index, type, item_count) values%s",
                         str.c_str());
            if (!h->exec(0x4ec2))
            {
                CMyFileLog log(__FUNCTION__, 0x1895);
                log("./log/statistic",
                    "\nQueryCubeStatisticCreate db1 error!!\n");
                return 0;
            }
            str.clear();
            i--;
        }
        else
        {
            str += buf;
        }
    }
    h->set_query(0x4ec2,
                 "inSert into log_cube_stat (occ_time, channel_no, level, item_index, type, item_count) values%s",
                 str.c_str());
    if (!h->exec(0x4ec2))
    {
        CMyFileLog log(__FUNCTION__, 0x18a2);
        log("./log/statistic", "\nQueryCubeStatisticCreate db1 error!!\n");
        return 0;
    }
    return 1;
}
char CDBManager::SaveUnchangableGuildInfo(
    Packet_UnChangable_GuildInfo_Save* packet)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (*(char*)((char*)packet + 0x12) == 0)
    {
        CMyFileLog log(__FUNCTION__, 0x1313);
        log("./log/TraceGuildErr",
            "CDBManager::SaveUnchangableGuildInfo guild(%d), charac_no(%d)\n",
            *(int*)((char*)packet + 0xa), *(int*)((char*)packet + 0xe));
        return 0;
    }
    if (!h->set_query(0x4e86,
                      "seLect master_no from guild_info where guild_id = %d and expire_flag = 0",
                      *(int*)((char*)packet + 0xa)))
    {
        CMyFileLog log(__FUNCTION__, 0x1319);
        log("./log/DBQueryErr",
            "CDBManager::SaveUnchangableGuildInfo() seLect master_no from guild_info where guild_id = %d and expire_flag = 0",
            *(int*)((char*)packet + 0xa));
        return 0;
    }
    if (!h->exec(0x4e86))
        return 0;
    if (!h->fetch())
    {
        CMyFileLog log(__FUNCTION__, 0x1327);
        log("./log/DBQueryErr",
            "CDBManager::SaveUnchangableGuildInfo() seLect master_no from guild_info where guild_id = %d and expire_flag = 0, fetch()",
            *(int*)((char*)packet + 0xa));
        return 0;
    }
    unsigned int masterNo = 0;
    if (!h->get_uint(0, masterNo))
        return 0;
    if (*(int*)((char*)packet + 0xe) == (int)masterNo)
    {
        if (!h->set_query(0x4e87,
                          "upDate guild_info set master_name='%s' where guild_id=%d and expire_flag = 0",
                          (char*)packet + 0x12, *(int*)((char*)packet + 0xa)))
        {
            CMyFileLog log(__FUNCTION__, 0x1348);
            log("./log/DBQueryErr",
                "CDBManager::SaveUnchangableGuildInfo() : upDate guild_info set master_name='%s' where guild_id=%d and expire_flag = 0",
                (char*)packet + 0x12, *(int*)((char*)packet + 0xa));
            return 0;
        }
        if (!h->exec(0x4e87))
        {
            CMyFileLog log(__FUNCTION__, 0x134f);
            log("./log/DBQueryErr",
                "CDBManager::SaveUnchangableGuildInfo() : upDate guild_info set master_name='%s' where guild_id=%d and expire_flag = 0, exe()",
                (char*)packet + 0x12, *(int*)((char*)packet + 0xa));
            return 0;
        }
    }
    else
    {
        if (!h->set_query(0x4e84,
                          "upDate guild_member set charac_name='%s' where guild_id=%d and charac_no=%d",
                          (char*)packet + 0x12, *(int*)((char*)packet + 0xa),
                          *(int*)((char*)packet + 0xe)))
        {
            CMyFileLog log(__FUNCTION__, 0x135e);
            log("./log/DBQueryErr",
                "CDBManager::SaveUnchangableGuildInfo() : upDate guild_member set charac_name=%s where guild_id=%d and charac_no=%d",
                (char*)packet + 0x12, *(int*)((char*)packet + 0xa),
                *(int*)((char*)packet + 0xe));
            return 0;
        }
        if (!h->exec(0x4e84))
        {
            CMyFileLog log(__FUNCTION__, 0x1369);
            log("./log/DBQueryErr",
                "CDBManager::SaveUnchangableGuildInfo() : upDate guild_member set charac_name=%s where guild_id=%d and charac_no=%d, exe()",
                (char*)packet + 0x12, *(int*)((char*)packet + 0xa),
                *(int*)((char*)packet + 0xe));
            return 0;
        }
    }
    return 1;
}
bool CDBManager::InsertLetter(unsigned int characNo, unsigned int sendCharacNo,
                              const char* subject, const char* content,
                              int& letterNo, long expiry)
{
    CDBHandle* h = m_handles[3];    // game db
    char buf1[0x200] = {0};
    h->escape_string(buf1, (char*)content);
    char buf2[0x3c] = {0};
    h->escape_string(buf2, (char*)subject);
    h->set_query(0x4e5c,
                 "inSert into letter(charac_no,send_charac_no,send_charac_name,letter_text,reg_date,stat) values(%d,%d,'%s','%s',from_unixtime(%d),%d)",
                 characNo, sendCharacNo, buf2, buf1, expiry, 1);
    if (!h->exec(0x4e5c))
        return 0;
    letterNo = GetIdentity(h);
    return 1;
}
char CDBManager::AddBuddy(unsigned int characNo, char* name,
                          STBuddyDBInfo& info, int& result)
{
    result = 3;
    char buf[0x3b] = {0};
    unsigned int m_id = 0;
    bool r;
    CDBHandle* h = m_handles[2];    // game db
    int n;
    void* gm;
    memcpy((char*)&info, name, 0x1d);
    h->escape_string(buf, name);
    r = h->set_query(0x4e50,
                     "seLect charac_no, lev, job, grow_type, sex, m_id, charac_name from charac_info where charac_name = '%s' and delete_flag = 0",
                     buf);
    if (!r)
    {
        CMyFileLog log(__FUNCTION__, 0xb8a);
        log("./log/DBQueryErr",
            "seLect charac_no, lev, job, grow_type, sex from charac_info where charac_name = '%s' and delete_flag = 0",
            buf);
        return 0;
    }
    r = h->exec(0x4e50);
    if (!r)
        return 0;
    n = h->get_n_rows();
    if (n == 0)
        return 0;
    if (n > 1)
    {
        CMyFileLog log(__FUNCTION__, 0xb9e);
        log("./log/DBQueryErr",
            "CDBManager::AddBuddy() : n_data != 1( %d ) \n", n);
    }
    r = h->fetch();
    if (!r)
        return 0;
    r = h->get_uint(0, *(unsigned int*)((char*)&info + 0x22));
    if (!r)
        return 0;
    r = h->get_short(1, *(short*)((char*)&info + 0x1e));
    if (!r)
        return 0;
    r = h->get_byte(2, *(char*)((char*)&info + 0x20));
    if (!r)
        return 0;
    r = h->get_byte(3, *(char*)((char*)&info + 0x21));
    if (!r)
        return 0;
    r = h->get_byte(4, *(char*)((char*)&info + 0x26));
    if (!r)
        return 0;
    r = h->get_uint(5, m_id);
    if (!r)
        return 0;
    gm = m_app->GetGMAccounts();
    if (gm)
    {
        if (((WongWork::CGMAccounts*)gm)->isGM(m_id))
        {
            result = 0x5a;
            return 0;
        }
    }
    r = h->get_str(6, (char*)&info, 0x1e);
    if (!r)
        return 0;
    h->set_query(0x4e51, "inSert into charac_friends values (%d, %d)",
                 characNo, info.m_characNo);
    r = h->exec(0x4e51);
    if (!r)
        return 0;
    result = 0;
    return 1;
}
bool CDBManager::QueryIPCounter(
    unsigned char serverGroup, std::vector<st_ip_counter_list>& ipList,
    std::vector<st_full_ip_counter_list>& fullIpList)
{
    CDBHandle* h = m_handles[6];    // guild db
    if (!h->set_query(0x4eda,
                      "seLect hack_type, hack_sub_type, c_class_ip, cnt from  auto_punish_hack_ip where occ_date = now() and cnt >= %d",
                      serverGroup))
    {
        CMyFileLog log("QueryIPCounter", 0x1cd3);
        log("./log/DBQueryErr",
            "CDBManager::QueryIPCounter() seLect hack_type, hack_sub_type, c_class_ip, cnt from  auto_punish_hack_ip where occ_date = now() and cnt >= %d \n",
            serverGroup);
        return 0;
    }
    if (!h->exec(0x4eda))
        return 0;
    int n = h->get_n_rows();
    CMyFileLog log1("QueryIPCounter", 0x1cdf);
    log1("./log/Secu", "[IP Counter] QueryIPCounter (cnt>%d) : %d \n",
         serverGroup, n);
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
        st_ip_counter_list item;
        if (!h->get_ushort(0, item.m_field0))
            return 0;
        if (!h->get_ushort(1, item.m_field2))
            return 0;
        memset(item.m_data, 0, 0xc);
        if (!h->get_str(2, item.m_data, 0xc))
            return 0;
        if (!h->get_uint(3, item.m_field10))
            return 0;
        ipList.push_back(item);
    }
    if (!h->set_query(0x4edb,
                      "seLect hack_type, hack_sub_type, full_ip, cnt from  auto_punish_hack_full_ip where occ_date = now() and cnt >= %d",
                      serverGroup))
    {
        CMyFileLog log("QueryIPCounter", 0x1d0f);
        log("./log/DBQueryErr",
            "CDBManager::QueryIPCounter() seLect hack_type, hack_sub_type, full_ip, cnt from  auto_punish_hack_full_ip where occ_date = now() and cnt >= %d \n",
            serverGroup);
        return 0;
    }
    if (!h->exec(0x4edb))
        return 0;
    n = h->get_n_rows();
    CMyFileLog log2("QueryIPCounter", 0x1d1b);
    log2("./log/Secu", "[D_IP Counter] QueryIPCounter (cnt>%d) : %d \n",
         serverGroup, n);
    for (int j = 0; j < n; j++)
    {
        if (!h->fetch())
            return 0;
        st_full_ip_counter_list item;
        if (!h->get_ushort(0, item.m_field0))
            return 0;
        if (!h->get_ushort(1, item.m_field2))
            return 0;
        memset(item.m_data, 0, 0x10);
        if (!h->get_str(2, item.m_data, 0x10))
            return 0;
        if (!h->get_uint(3, item.m_field14))
            return 0;
        fullIpList.push_back(item);
    }
    return 1;
}
char CDBManager::QueryDeathTowerPlayDataJobStatisticCreate(
    Packet_DBMW_DeathTower_Statistic_Playdata_Job* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    int count = *(int*)((char*)packet + 0xa);
    CMyFileLog slog("QueryDeathTowerPlayDataJobStatisticCreate", 0x17a5);
    slog("./log/statistic",
         "Packet_DBMW_DeathTower_Statistic_Playdata_Job : (%d) °³ ÆÐÅ¶ ¼ö½Å\n",
         count);
    char buf[0x800] = {0};
    std::string str;
    for (int i = 0; i < count / 2; i++)
    {
        char* e = (char*)packet + i * 0x10;
        if (str.size() != 0)
        {
            sprintf(buf, ",(now(),%d,%d,%d,%d,%d,%d)", *(signed char*)(e + 0xe),
                    *(short*)(e + 0xf), *(int*)(e + 0x11),
                    *(signed char*)(e + 0x15), *(int*)(e + 0x16),
                    *(int*)(e + 0x1a));
        }
        else
        {
            sprintf(buf, "(now(),%d,%d,%d,%d,%d,%d)", *(signed char*)(e + 0xe),
                    *(short*)(e + 0xf), *(int*)(e + 0x11),
                    *(signed char*)(e + 0x15), *(int*)(e + 0x16),
                    *(int*)(e + 0x1a));
        }
        str += buf;
    }
    h->set_query(0x4e9f,
                 "inSert into log_deathtower_playdata_job (occ_time, type, level, charac_grow, charac_job, avg_clear_count, playcount ) values%s",
                 str.c_str());
    if (!h->exec(0x4e9f))
    {
        CMyFileLog log(__FUNCTION__, 0x17c1);
        log("./log/statistic",
            "\nQueryDeathTowerPlayDataJobStatisticCreate db1 error!!\n");
        return 0;
    }
    memset(buf, 0, 0x800);
    str.clear();
    for (int j = count / 2 + 1; j < count; j++)
    {
        char* e = (char*)packet + j * 0x10;
        if (str.size() != 0)
        {
            sprintf(buf, ",(now(),%d,%d,%d,%d,%d,%d)", *(signed char*)(e + 0xe),
                    *(short*)(e + 0xf), *(int*)(e + 0x11),
                    *(signed char*)(e + 0x15), *(int*)(e + 0x16),
                    *(int*)(e + 0x1a));
        }
        else
        {
            sprintf(buf, "(now(),%d,%d,%d,%d,%d,%d)", *(signed char*)(e + 0xe),
                    *(short*)(e + 0xf), *(int*)(e + 0x11),
                    *(signed char*)(e + 0x15), *(int*)(e + 0x16),
                    *(int*)(e + 0x1a));
        }
        str += buf;
    }
    h->set_query(0x4e9f,
                 "inSert into log_deathtower_playdata_job (occ_time, type, level, charac_grow, charac_job, avg_clear_count, playcount) values%s",
                 str.c_str());
    if (!h->exec(0x4e9f))
    {
        CMyFileLog log(__FUNCTION__, 0x17df);
        log("./log/statistic",
            "\nQueryDeathTowerPlayDataJobStatisticCreate db2 error!!\n");
        return 0;
    }
    return 1;
}
char CDBManager::QueryDeathTowerValueStatisticCreate(
    Packet_DBMW_DeathTower_Statistic_Value* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    int count = *(int*)((char*)packet + 0xa);
    CMyFileLog slog("QueryDeathTowerValueStatisticCreate", 0x1760);
    slog("./log/statistic",
         "Packet_DBMW_DeathTower_Statistic_Value : (%d) °³ ÆÐÅ¶ ¼ö½Å\n",
         count);
    for (int i = 0; i < count; i++)
    {
        unsigned int vals[0xb] = {0};
        vals[*(int*)((char*)packet + i * 0xf + 0x11)] =
            *(unsigned int*)((char*)packet + i * 0xf + 0x19);
        h->set_query(0x4e9e,
                     "upDate log_deathtower_value set try_cnt=try_cnt+%u, clear_stage=clear_stage+%u, recipeCnt=recipeCnt+%u, commonCnt=commonCnt+%u, uncommonCnt=uncommonCnt+%u, rareCnt=rareCnt+%u, uniqCnt=uniqCnt+%u, card_item_goldprice=card_item_goldprice+%u, card_gold=card_gold+%u, repair_price=repair_price+%u  where ",
                     vals[0], vals[1], vals[2], vals[3], vals[4], vals[5],
                     vals[6], vals[7], vals[8], vals[9]);
        if (!h->exec(0x4e9e))
        {
            h->set_query(0x4e9d,
                         "inSert into log_deathtower_value (occ_date, type, level, try_cnt, clear_stage, recipeCnt, commonCnt, uncommonCnt, rareCnt, uniqCnt, card_item_goldprice, card_gold, repair_price) values (cast(now() as date), %d, %d, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
                         *(signed char*)((char*)packet + i * 0xf + 0xe),
                         *(short*)((char*)packet + i * 0xf + 0xf), vals[0],
                         vals[1], vals[2], vals[3], vals[4], vals[5], vals[6],
                         vals[7], vals[8], vals[9]);
            if (!h->exec(0x4e9d))
            {
                CMyFileLog log(__FUNCTION__, 0x178c);
                log("./log/statistic",
                    "\nQueryDeathTowerValueStatisticCreate db error!!\n");
                return 0;
            }
        }
    }
    return 1;
}
bool CDBManager::queryTowerFullRank(unsigned int towerIndex,
                                    std::vector<stTowerRank_t>& ranks,
                                    unsigned int limit)
{
    CDBHandle* h = m_handles[2];    // game db
    for (unsigned int rank = 1; rank <= 4; rank++)
    {
        char ok = 0;
        if (limit <= 5)
        {
            ok = h->set_query(0x4e69,
                              "seLect charac_no,tower_index,rank,member_info from charac_tower_rank_top5 where tower_index=%d and part_type=%d limit %d",
                              towerIndex, rank, limit);
        }
        else
        {
            ok = h->set_query(0x4e69,
                              "seLect charac_no,tower_index,rank,member_info from charac_tower_rank where tower_index=%d and part_type=%d and rank>5 limit %d",
                              towerIndex, rank, limit);
        }
        if (!ok)
        {
            CMyFileLog log("queryTowerFullRank", 0xf6d);
            log("./log/DBQueryErr",
                "CDBManager::GuildJoin() seLect_charac_no_tower_idx_rank_from_charac_tower_rank Exception Break\n");
            return 0;
        }
        if (!h->exec(0x4e69))
            return 0;
        unsigned int i = 0;
        while (i < (unsigned int)h->get_n_rows())
        {
            if (!h->fetch())
                return 0;
            stTowerRank_t item;
            if (!h->get_uint(0, item.m_characNo))
                return 0;
            if (!h->get_ushort(1, item.m_towerIndex))
                return 0;
            if (!h->get_ushort(2, item.m_rank))
                return 0;
            if (!h->get_binary(3, item.m_memberInfo, rank * 0x17))
                return 0;
            item.m_partType = (char)rank;
            ranks.push_back(item);
            i++;
        }
    }
    return 1;
}
bool CDBManager::UpdateDisjointAvatarStatistic(
    Packet_Avater_Disjoint_Statistic_DB* packet)
{
    CDBHandle* h = m_handles[4];    // log db
    char buf1[0x10] = {0};
    char buf2[0x10] = {0};
    if (!h)
        return 0;
    for (int kind = 0; kind <= 2; kind++)
    {
        if (kind == 0)
            memcpy(buf1, "normal", 7);
        else if (kind == 1)
            memcpy(buf1, "high", 5);
        else
            memcpy(buf1, "rare", 5);
        for (int item = 0; item <= 9; item++)
        {
            if (item == 0)
                memcpy(buf2, "headgear", 9);
            else if (item == 1)
                memcpy(buf2, "hair", 5);
            else if (item == 2)
                memcpy(buf2, "face", 5);
            else if (item == 3)
                memcpy(buf2, "jacket", 7);
            else if (item == 4)
                memcpy(buf2, "pants", 6);
            else if (item == 5)
                memcpy(buf2, "shoes", 6);
            else if (item == 6)
                memcpy(buf2, "breast", 7);
            else if (item == 7)
                memcpy(buf2, "waist", 6);
            else if (item == 8)
                memcpy(buf2, "skin", 5);
            else
                memcpy(buf2, "aurora", 7);
            int idx = kind * 9 + item;
            h->set_query(0x4f47,
                         "upDate log_avatar_grind set avatar_emblem_grind=avatar_emblem_grind+%d, avatar_bindcube_grind=avatar_bindcube_grind+%d, avatar_rechargestone_grind=avatar_rechargestone_grind+%d where cur_date=CURDATE() and grade='%s' and body_part='%s' ",
                         *(int*)((char*)packet + idx * 8 + 0xe),
                         *(int*)((char*)packet + (idx + 0x34) * 8 + 0xa),
                         *(int*)((char*)packet + (idx + 0x34) * 4 + 0x12),
                         buf1, buf2);
            if (!h->exec(0x4f47))
            {
                CMyFileLog log(__FUNCTION__, 0x1eb0);
                log("./log/DBQueryErr",
                    "CDBManager::UpdateDisjointAvatarStatistic() upDate Error");
            }
            if (h->getAffectedRowCount() == 0)
            {
                h->set_query(
                    0x4ee8,
                    "inSert into log_avatar_grind(cur_date, grade, body_part, avatar_emblem_grind, avatar_bindcube_grind, avatar_rechargestone_grind) values(CURDATE(), '%s', '%s', %d, %d, %d)",
                    buf1, buf2,
                    *(int*)((char*)packet + idx * 8 + 0xe),
                    *(int*)((char*)packet + idx * 8 + 0xa),
                    *(int*)((char*)packet + (idx + 0x34) * 4 + 0x12));
                if (!h->exec(0x4ee8))
                {
                    CMyFileLog log(__FUNCTION__, 0x1ebe);
                    log("./log/DBQueryErr",
                        "CDBManager::UpdateDisjointAvatarStatistic(insert) upDate Error");
                }
            }
        }
    }
    return 1;
}
char CDBManager::QueryDeathTowerPlayDataPartyStatisticCreate(
    Packet_DBMW_DeathTower_Statistic_Playdata_Party* packet)
{
    time_t now = time(0);
    CDBHandle* h = m_handles[4];    // log db
    if (!h)
        return 0;
    int count = *(int*)((char*)packet + 0xa);
    CMyFileLog slog("QueryDeathTowerPlayDataPartyStatisticCreate", 0x17f7);
    slog("./log/statistic",
         "Packet_DBMW_DeathTower_Statistic_Playdata_Party : (%d) °³ ÆÐÅ¶ ¼ö½Å\n",
         count);
    char buf[0x800] = {0};
    std::string str;
    for (int i = 0; i < count / 2; i++)
    {
        char* e = (char*)packet + i * 0xa;
        if (str.size() != 0)
        {
            sprintf(buf, ",(now(),%d,%d,%d,%d)", *(signed char*)(e + 0xe),
                    *(signed char*)(e + 0xf), *(int*)(e + 0x10),
                    *(int*)(e + 0x14));
        }
        else
        {
            sprintf(buf, "(now(),%d,%d,%d,%d)", *(signed char*)(e + 0xe),
                    *(signed char*)(e + 0xf), *(int*)(e + 0x10),
                    *(int*)(e + 0x14));
        }
        str += buf;
    }
    h->set_query(0x4ea1,
                 "inSert into log_deathtower_playdata_party (occ_time, type, party_count, avg_clear_count, playcount) values%s",
                 str.c_str());
    if (!h->exec(0x4ea1))
    {
        CMyFileLog log(__FUNCTION__, 0x1813);
        log("./log/statistic",
            "\nQueryDeathTowerPlayDataPartyStatisticCreate db1 error!!\n");
        return 0;
    }
    memset(buf, 0, 0x800);
    str.clear();
    for (int j = count / 2 + 1; j < count; j++)
    {
        char* e = (char*)packet + j * 0xa;
        if (str.size() != 0)
        {
            sprintf(buf, ",(now(),%d,%d,%d,%d)", *(signed char*)(e + 0xe),
                    *(signed char*)(e + 0xf), *(int*)(e + 0x10),
                    *(int*)(e + 0x14));
        }
        else
        {
            sprintf(buf, "(now(),%d,%d,%d,%d)", *(signed char*)(e + 0xe),
                    *(signed char*)(e + 0xf), *(int*)(e + 0x10),
                    *(int*)(e + 0x14));
        }
        str += buf;
    }
    h->set_query(0x4ea1,
                 "inSert into log_deathtower_playdata_party (occ_time, type, party_count, avg_clear_count, playcount) values%s",
                 str.c_str());
    if (!h->exec(0x4ea1))
    {
        CMyFileLog log(__FUNCTION__, 0x1831);
        log("./log/statistic",
            "\nQueryDeathTowerPlayDataPartyStatisticCreate db2 error!!\n");
        return 0;
    }
    return 1;
}
bool CDBManager::AwardGuildCoinByMail(int guildId, unsigned int serverGroup,
                                      unsigned int itemId,
                                      unsigned int endurance, int addInfo,
                                      int upgrade,
                                      std::vector<int>& characNos)
{
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[3];   // game db
    if (!h->set_query(0x4e39,
                      "seLect charac_no,unix_timestamp(member_time) from guild_member where guild_id = %d and server_id = %d and member_flag = 1",
                      guildId, serverGroup))
    {
        CMyFileLog log("AwardGuildCoinByMail", 0x8dd);
        log("./log/DBQueryErr",
            "CDBManager::AwardGuildCoinByMail() select charac_no from guild_member where server_id = %d and guild_id = %d and member_flag = 1\n",
            guildId, serverGroup);
        return 0;
    }
    if (!h->exec(0x4e39))
        return 0;
    int n = h->get_n_rows();
    time_t now = time(0);
    tm* t = localtime(&now);
    t->tm_mday += 1;
    t->tm_hour = 0;
    t->tm_min = 0;
    time_t tomorrow = mktime(t);
    int i = 0;
    while (i < n)
    {
        if (!h->fetch())
            return 0;
        unsigned int characNo = 0;
        if (!h->get_uint(0, characNo))
            return 0;
        unsigned int memberTime = 0;
        if (!h->get_uint(1, memberTime))
            return 0;
        if (upgrade > 2 && !isDayTimeOver(memberTime, 7))
            continue;
        characNos.push_back(characNo + 0);
        char buf[0x1e];
        memset(buf, 0, 0x1e);
        std::string s = g_ServerString_.GetServerString(0x7d0, 0);
        strcpy(buf, s.c_str());
        h2->set_query(0x4e57,
                      "inSert into postal (occ_time, send_charac_no, receive_charac_no, seal_flag, item_id, add_info, endurance, upgrade, gold, send_charac_name ) values ( from_unixtime( %d ), %d, %d, %d, %d, %d, %d, %d, %d,'%s')",
                      tomorrow, 0, characNo, 0, itemId, addInfo, endurance, 0,
                      0, buf);
        if (!h2->exec(0x4e57))
            return 0;
        i++;
    }
    return 1;
}
bool CDBManager::SendGuildCoinByMail(int guildId, unsigned int serverGroup,
                                     unsigned int itemId,
                                     unsigned int endurance, int addInfo,
                                     char* subject, char* content)
{
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[3];   // game db
    if (!h->set_query(0x4e39,
                      "seLect charac_no from guild_member where guild_id = %d and server_id = %d and member_flag = 1",
                      guildId, serverGroup))
    {
        CMyFileLog log(__FUNCTION__, 0x897);
        log("./log/DBQueryErr",
            "CDBManager::SendGuildCoinByMail() select charac_no from guild_member where server_id = %d and guild_id = %d and member_flag = 1\n",
            guildId, serverGroup);
        return 0;
    }
    if (!h->exec(0x4e39))
        return 0;
    int n = h->get_n_rows();
    time_t now = time(0);
    tm* t = localtime(&now);
    t->tm_mday += 1;
    t->tm_hour = 0;
    t->tm_min = 0;
    time_t tomorrow = mktime(t);
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
        unsigned int characNo = 0;
        if (!h->get_uint(0, characNo))
            return 0;
        int letterNo = 0;
        if (!InsertLetter(characNo, 0, subject, content, letterNo, tomorrow))
        {
            CMyFileLog log(__FUNCTION__, 0x8c0);
            log("./log/Postal", "InsertLetter Err, %s(%s)", subject, content);
            continue;
        }
        if (!InsertPostal(characNo, 0, 0, itemId, addInfo, endurance, 0,
                          subject, tomorrow, letterNo))
        {
            CMyFileLog log(__FUNCTION__, 0x8c6);
            log("./log/Postal", "InsertPostal Err, %s(%s)", subject, content);
        }
    }
    return 1;
}
bool CDBManager::InsertPostal(unsigned int receiveCharacNo,
                              unsigned int sendCharacNo, int sealFlag,
                              unsigned int itemId, int addInfo,
                              unsigned int endurance, int upgrade, char* name,
                              long occTime, int letterId)
{
    CDBHandle* db = m_handles[3];    // game db
    if (!db->set_query(0x4e3a,
                       "inSert into postal (occ_time, send_charac_no, receive_charac_no, seal_flag, item_id, add_info, endurance, upgrade, gold, send_charac_name, letter_id ) values ( from_unixtime( %d ), %d, %d, %d, %d, %d, %d, %d, %d,'%s', %d)",
                       occTime, sendCharacNo, receiveCharacNo, sealFlag, itemId,
                       addInfo, endurance, 0, upgrade, name, letterId))
    {
        CMyFileLog log(__FUNCTION__, 0x7bb);
        log("./log/DBQueryErr",
            "CDBManager::AwardGuildTitleByMail() Fatal Error Break : insert into postal (occ_time, send_charac_no, receive_charac_no, seal_flag, item_id, add_info, endurance, upgrade, gold, send_charac_name ) values ( from_unixtime( %d ), %d, %d, %d, %d, %d, %d, %d, %d,'%s', %d)\n",
            occTime, sendCharacNo, receiveCharacNo, sealFlag, itemId, addInfo,
            endurance, 0, upgrade, name, letterId);
    }
    if (!db->exec(0x4e3a))
        return 0;
    return 1;
}
bool CDBManager::QueryLoadARSInfo(std::vector<st_ars_info_list>& arsList)
{
    CDBHandle* h = m_handles[6];    // guild db
    if (!h->set_query(0x4ef3,
                      "seLect hack_type,cnt,etc,hack_sub_type,hack_sub_cnt,apply_flag, ip_cnt from auto_punish_hack_info where apply_flag > 0"))
    {
        CMyFileLog log("QueryLoadARSInfo", 0x20cb);
        log("./log/DBQueryErr",
            "CDBManager::QueryLoadARSInfo() seLect hack_type,cnt,etc,hack_sub_type,hack_sub_cnt,apply_flag, ip_cnt from auto_punish_hack_info where apply_flag > 0 \n");
        return 0;
    }
    if (!h->exec(0x4ef3))
        return 0;
    int n = h->get_n_rows();
    CMyFileLog log("QueryLoadARSInfo", 0x20da);
    log("./log/Secu", "[ARS_INFO] QueryLoadARSInfo Load Cnt : %d \n", n);
    if (n == 0)
        return 1;
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
            return 0;
        st_ars_info_list item;
        if (!h->get_ushort(0, item.m_field0))
            return 0;
        if (!h->get_ushort(1, item.m_field2))
            return 0;
        if (!h->get_ushort(2, item.m_fieldA))
            return 0;
        if (!h->get_ushort(3, item.m_field4))
            return 0;
        if (!h->get_ushort(4, item.m_field6))
            return 0;
        if (!h->get_ubyte(5, item.m_field8))
            return 0;
        if (!h->get_ubyte(6, item.m_field9))
            return 0;
        arsList.push_back(item);
    }
    return 1;
}
bool CDBManager::QuerySubGuildMaster(unsigned char serverGroup,
                                     unsigned int guildId,
                                     Packet_DB_Reply_Query_Guild& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[2];   // game db
    char* info = (char*)&reply + 0x13;
    if (!h->set_query(0x4e58,
                      "seLect charac_no from guild_member where guild_id = %d and server_id = %d and grade =  %d and member_flag = 1 limit %d",
                      guildId, serverGroup, 2, 5))
    {
        CMyFileLog log(__FUNCTION__, 0xd9d);
        log("./log/DBQueryErr", "CDBManager::QueryGuildMember() Exception Break\n");
        *(char*)((char*)&reply + 0xa) = 0;
        return 0;
    }
    if (!h->exec(0x4e58))
    {
        *(char*)((char*)&reply + 0xa) = 0;
        return 0;
    }
    int n = h->get_n_rows();
    *(char*)(info + 0x2d) = (char)n;
    for (int i = 0; i < n; i++)
    {
        if (!h->fetch())
        {
            *(char*)((char*)&reply + 0xa) = 1;
            return 1;
        }
        if (!h->get_uint(0, *(unsigned int*)(info + 0x2e + i * 4)))
        {
            *(char*)((char*)&reply + 0xa) = 3;
            return 0;
        }
    }
    *(char*)((char*)&reply + 0xa) = 1;
    return 1;
}
char CDBManager::QueryOnTimeEventIdxUpdate(
    Packet_Req_Ontime_Event_Idx_Update* packet)
{
    if (!packet)
        return 0;
    CDBHandle* h = m_handles[0xd];    // se_event db
    if (!h->set_query(0x4f14,
                      "seLect ifnull(max(no), 1) from event_1112_ontime_info"))
    {
        CMyFileLog log(__FUNCTION__, 0x24d8);
        log("./log/DBQueryErr",
            "set_query(seLect_from_event_ontime_idx) Query Error ");
        return 0;
    }
    if (h->exec(0x4f14))
    {
        if (!h->fetch())
            return 0;
    }
    unsigned int maxNo = 0;
    if (!h->get_uint(0, maxNo))
        return 0;
    if (*(unsigned int*)((char*)packet + 0x12) > maxNo)
    {
        if (!h->set_query(0x4f19,
                          "inSert into event_1112_ontime_info(no ,item_index,item_count,time ) values(%u,%u,%u,now())",
                          *(unsigned int*)((char*)packet + 0x12),
                          *(unsigned int*)((char*)packet + 0xa),
                          *(unsigned int*)((char*)packet + 0xe)))
        {
            CMyFileLog log(__FUNCTION__, 0x24ec);
            log("./log/DBQueryErr",
                "set_query(inSert_event_ontime_idx_update) Query Error ");
            return 0;
        }
        if (!h->exec(0x4f19))
            return 0;
    }
    else
    {
        *(unsigned int*)((char*)packet + 0x12) = maxNo;
    }
    return 1;
}
char CDBManager::QueryOnTimeEventItem(Packet_Result_Ontime_Event_Item& reply)
{
    CDBHandle* h = m_handles[0xd];    // se_event db
    if (!h->set_query(0x4f18, "seLect idx, cnt from event_ontime_item"))
    {
        CMyFileLog log(__FUNCTION__, 0x24b1);
        log("./log/DBQueryErr",
            "set_query(seLect_from_event_ontime_item) Query Error");
        return 0;
    }
    if (!h->exec(0x4f18) || !h->fetch())
    {
        if (h->get_n_rows() == 0)
            reply.m_field12 = 2;
        return 0;
    }
    if (!h->get_uint(0, *(unsigned int*)((char*)&reply + 0xa)))
        return 0;
    if (!h->get_uint(1, *(unsigned int*)((char*)&reply + 0xe)))
        return 0;
    return 1;
}
bool CDBManager::QueryBuddyInfo(unsigned int characNo, STBuddyDBInfo* buddies,
                                unsigned char& count)
{
    CDBHandle* h = m_handles[2];    // game db
    if (!h->set_query(0x4e52,
                      "seLect b.charac_no, b.charac_name, b.lev, b.job, b.grow_type, b.sex from charac_friends a, charac_info b where b.charac_no = a.friend_no and a.charac_no = %d and b.delete_flag=0 limit %d",
                      characNo, 0x20))
    {
        CMyFileLog log(__FUNCTION__, 0xc24);
        log("./log/DBQueryErr",
            "select_b_charac_info_from_charac_friends_a_charac_friends_b_where_characno_limit where charac_no = %d and friend_no = %d",
            characNo, 0x20);
        return 0;
    }
    if (!h->exec(0x4e52))
        return 0;
    count = (unsigned char)h->get_n_rows();
    for (int i = 0; i < (int)count; i++)
    {
        if (!h->fetch())
            return 1;
        STBuddyDBInfo& b = buddies[i];
        if (!h->get_uint(0, *(unsigned int*)((char*)&b + 0x22)))
            return 0;
        if (!h->get_str(1, b.m_name, 0x1e))
            return 0;
        if (!h->get_short(2, *(short*)((char*)&b + 0x1e)))
            return 0;
        if (!h->get_byte(3, *(char*)((char*)&b + 0x20)))
            return 0;
        if (!h->get_byte(4, *(char*)((char*)&b + 0x21)))
            return 0;
        if (!h->get_byte(5, *(char*)((char*)&b + 0x26)))
            return 0;
    }
    return 1;
}
char CDBManager::GetCoinEventPerDay(int serverId, int add, int& out1,
                                    int& out2)
{
    CDBHandle* h = m_handles[1];    // neople db
    if (!h->set_query(0x4ee5,
                      "seLect log_id, parameter1, parameter2 from dnf_event_log where event_type= %d and end_time = 0 and server_id =%d and now() >= start_time order by start_time",
                      4, serverId))
    {
        CMyFileLog log(__FUNCTION__, 0x1e15);
        log("./log/DBQueryErr", "GetCoinEventPerDay Error\n");
        return 0;
    }
    if (!h->exec(0x4ee5))
        return 0;
    if (h->get_n_rows() == 0)
        return 0;
    if (!h->fetch())
        return 0;
    int logId = 0;
    if (!h->get_int(0, logId))
        return 0;
    int param1 = 0;
    if (!h->get_int(1, param1))
        return 0;
    int param2 = 0;
    if (!h->get_int(2, param2))
        return 0;
    param1 += add;
    if (param1 < 0)
        param1 = 0;
    out1 = param1;
    out2 = param2;
    if (!h->set_query(0x4ee6,
                      "upDate dnf_event_log set parameter1=%d, parameter2=%d where log_id = %u",
                      param1, param2, logId))
    {
        CMyFileLog log(__FUNCTION__, 0x1e44);
        log("./log/DBQueryErr", "GetCoinEventPerDay Error\n");
        return 0;
    }
    if (!h->exec(0x4ee6))
        return 0;
    return 1;
}
bool CDBManager::QueryCharacNoByName(char* name, unsigned int& characNo,
                                     int* result)
{
    CDBHandle* h = m_handles[2];    // game db
    if (result)
    {
        if (!h->set_query(0x4e3e,
                          "seLect charac_no,m_id from charac_info where charac_name = '%s'",
                          name))
        {
            CMyFileLog log(__FUNCTION__, 0xb1a);
            log("./log/DBQueryErr",
                "CDBManager::QueryCharacNoByName() seLect charac_no from charac_info where charac_name = '%s'",
                name);
            return 0;
        }
    }
    else
    {
        if (!h->set_query(0x4e3e,
                          "seLect charac_no from charac_info where charac_name = '%s'",
                          name))
        {
            CMyFileLog log(__FUNCTION__, 0xb12);
            log("./log/DBQueryErr",
                "CDBManager::QueryCharacNoByName() seLect charac_no from charac_info where charac_name = '%s'",
                name);
            return 0;
        }
    }
    if (!h->exec(0x4e3e))
        return 0;
    int n = h->get_n_rows();
    if (n == 0 || n > 1)
    {
        CMyFileLog log(__FUNCTION__, 0xb30);
        log("./log/DBQueryErr",
            "CDBManager::QueryCharacNoByName() : n_data != 1( %d )\n", n);
        return 0;
    }
    if (!h->fetch())
        return 0;
    if (!h->get_uint(0, characNo))
        return 0;
    if (result)
    {
        unsigned int m_id = 0;
        if (!h->get_uint(1, m_id))
            return 0;
        void* gm = m_app->GetGMAccounts();
        if (gm && ((WongWork::CGMAccounts*)gm)->isGM(m_id))
        {
            *result = 0x5a;
            return 0;
        }
    }
    return 1;
}
bool CDBManager::updateCompatibilityIndex(
    Packet_Stat_Compatibility_Index* packet)
{
    CDBHandle* h = m_handles[0xf];    // frame_lag db
    if (!h)
        return 0;
    h->set_query(0x4f4b,
                 "upDate ting_user_spec set reg_datetime=now(), cpu_vendor=%d, cpu_num=%d, cpu_clock=%d, ram=%d, video_vendor=%d, video_device=%d, video_ram=%d, os=%d, os_bit=%d where m_id=%u",
                 *(unsigned char*)((char*)packet + 0xe),
                 *(unsigned char*)((char*)packet + 0xf),
                 *(int*)((char*)packet + 0x10),
                 *(unsigned short*)((char*)packet + 0x14),
                 *(unsigned short*)((char*)packet + 0x16),
                 *(unsigned short*)((char*)packet + 0x18),
                 *(unsigned short*)((char*)packet + 0x1a),
                 *(unsigned char*)((char*)packet + 0x1c),
                 *(unsigned char*)((char*)packet + 0x1d),
                 *(unsigned int*)((char*)packet + 0xa));
    if (!h->exec(0x4f4b))
    {
        CMyFileLog log(__FUNCTION__, 0x2916);
        log("./log/DBQueryErr", "upDate ting_user_spec Query(exec) Error");
        return 0;
    }
    if (h->getAffectedRowCount() == 0)
    {
        h->set_query(
            0x4f4a,
            "inSert into ting_user_spec (m_id, reg_datetime, cpu_vendor, cpu_num, cpu_clock, ram, video_vendor, video_device, video_ram, os, os_bit) values(%u, now(), %d, %d, %d, %d, %d, %d, %d, %d, %d)",
            *(unsigned int*)((char*)packet + 0xa),
            *(unsigned char*)((char*)packet + 0xe),
            *(unsigned char*)((char*)packet + 0xf),
            *(int*)((char*)packet + 0x10),
            *(unsigned short*)((char*)packet + 0x14),
            *(unsigned short*)((char*)packet + 0x16),
            *(unsigned short*)((char*)packet + 0x18),
            *(unsigned short*)((char*)packet + 0x1a),
            *(unsigned char*)((char*)packet + 0x1c),
            *(unsigned char*)((char*)packet + 0x1d));
        if (!h->exec(0x4f4a))
        {
            CMyFileLog log(__FUNCTION__, 0x292d);
            log("./log/DBQueryErr", "inSert ting_user_spec Query(exec) Error");
            return 0;
        }
    }
    return 1;
}
char CDBManager::OnSecretShopStatistic(Packet_Secret_Shop_Statistic* packet)
{
    CDBHandle* h = m_handles[4];    // log db
    int count = *(int*)((char*)packet + 0xa);
    for (int i = 0; i < count; i++)
    {
        char* e = (char*)packet + i * 0x14;
        h->set_query(0x4efc,
                     "upDate log_secret_shop set show_count=show_count+%d,show_charac_count=show_charac_count+%d,buy_count=buy_count+%d,price=price+%d where occ_date=cast(now() as date) and dungeon_idx=%d and npc_idx=%d",
                     *(int*)(e + 0x16), *(int*)(e + 0x1a), *(int*)(e + 0x1e),
                     *(int*)(e + 0x22), *(int*)(e + 0x12),
                     *(int*)((char*)packet + 0xe));
        if (!h->exec(0x4efc))
        {
            CMyFileLog log(__FUNCTION__, 0x21bd);
            log("./log/DBQueryErr",
                "CDBManager::OnSecretShopStatistic() upDate Error");
        }
        h->set_query(
            0x4efb,
            "inSert into log_secret_shop(occ_date,npc_idx,dungeon_idx,show_count,show_charac_count,buy_count,price) values(cast(now() as date), %d, %d, %d, %d, %d, %d)",
            *(int*)((char*)packet + 0xe), *(int*)(e + 0x12),
            *(int*)(e + 0x16), *(int*)(e + 0x1a), *(int*)(e + 0x1e),
            *(int*)(e + 0x22));
        if (!h->exec(0x4efb))
        {
            CMyFileLog log(__FUNCTION__, 0x21cb);
            log("./log/DBQueryErr",
                "CDBManager::OnSecretShopStatistic() inSert Error");
        }
    }
    return 1;
}
bool CDBManager::loadLimitNpcBuyItemInfo(LimitNpcBuyItemRequestInfo* req,
                                         LimitNpcBuyItemResultInfo* result)
{
    CDBHandle* h = m_handles[2];    // game db
    if (!h)
        return 0;
    if (!h->set_query(0x4f45,
                      "seLect item_index, max_count, sell_count from limit_npc_item limit %d",
                      0x1e))
    {
        CMyFileLog log(__FUNCTION__, 0x2811);
        log("./log/DBQueryErr",
            "seLect item_index, max_count, sell_count from limit_npc_item Error");
        return 0;
    }
    if (!h->exec(0x4f45))
    {
        CMyFileLog log(__FUNCTION__, 0x281a);
        log("./log/DBQueryErr", "loadLimitNpcBuyItemInfo Query(exec) Error");
        return 0;
    }
    *(int*)((char*)result + 0xa) = h->get_n_rows();
    if (*(int*)((char*)result + 0xa) == 0)
    {
        CMyFileLog log(__FUNCTION__, 0x2821);
        log("./log/DBQueryErr", "loadLimitNpcBuyItemInfo (Row_Data Not Exist) Error");
        return 0;
    }
    for (int i = 0; i < *(int*)((char*)result + 0xa) && i <= 0x1d; i++)
    {
        if (!h->fetch())
        {
            CMyFileLog log(__FUNCTION__, 0x282a);
            log("./log/DBQueryErr", "loadLimitNpcBuyItemInfo Query(fetch) Error");
            return 0;
        }
        int col = 0;
        if (!h->get_uint(col++, *(unsigned int*)((char*)result + i * 0xc + 0xe)))
        {
            CMyFileLog log(__FUNCTION__, 0x2833);
            log("./log/DBQueryErr", "loadLimitNpcBuyItemInfo (get_uint(itemId)) Error");
            return 0;
        }
        if (!h->get_uint(col++, *(unsigned int*)((char*)result + i * 0xc + 0x12)))
        {
            CMyFileLog log(__FUNCTION__, 0x283a);
            log("./log/DBQueryErr", "loadLimitNpcBuyItemInfo (get_uint(maxCount) Error");
            return 0;
        }
        if (!h->get_uint(col++, *(unsigned int*)((char*)result + i * 0xc + 0x16)))
        {
            CMyFileLog log(__FUNCTION__, 0x2841);
            log("./log/DBQueryErr", "loadLimitNpcBuyItemInfo (get_uint(sellCount)) Error");
            return 0;
        }
    }
    return 1;
}
bool CDBManager::updateLimitNpcBuyItemInfo(LimitNpcBuyItemUpdate* update)
{
    CDBHandle* h = m_handles[2];    // game db
    if (!h)
        return 0;
    if (!h->set_query(0x4f46,
                      "upDate limit_npc_item set sell_count=sell_count+%u where item_index=%u",
                      update->m_fieldA, update->m_field12))
    {
        CMyFileLog log(__FUNCTION__, 0x2857);
        log("./log/DBQueryErr",
            "upDate limit_npc_item set sell_count=%u where item_index=%u Error",
            update->m_fieldA, update->m_field12);
        return 0;
    }
    if (!h->exec(0x4f46))
    {
        CMyFileLog log(__FUNCTION__, 0x2861);
        log("./log/DBQueryErr", "updateLimitNpcBuyItemInfo Query(exec) Error");
        return 0;
    }
    return 1;
}
bool CDBManager::QueryGuildMemberGradeByName(unsigned char serverId,
                                             unsigned int guildId,
                                             char* name,
                                             unsigned char& grade,
                                             unsigned int& m_id,
                                             unsigned int& result)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e5a,
                      "seLect charac_no, grade, m_id from guild_member where guild_id = %d and server_id = %d and charac_name =  '%s' and member_flag = 1",
                      guildId, serverId, name))
    {
        CMyFileLog log(__FUNCTION__, 0xddf);
        log("./log/DBQueryErr",
            "CDBManager::ChangeUnconnectedGuildMemberGrade() Exception Break\n");
        return 0;
    }
    if (!h->exec(0x4e5a))
        return 0;
    if (!h->fetch())
        return 1;
    if (!h->get_uint(0, m_id))
        return 0;
    if (!h->get_ubyte(1, grade))
        return 0;
    if (!h->get_ubyte(2, *(unsigned char*)&result))
        return 0;
    return 1;
}
char CDBManager::ChangeGuildMemberGrade(unsigned char serverId,
                                        unsigned int guildId,
                                        unsigned char grade, char* name)
{
    bool ret;
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e5b,
                      "upDate guild_member set grade = %d where guild_id = %d and server_id = %d and  charac_name = '%s' and member_flag = 1",
                      grade, guildId, serverId, name))
    {
        CMyFileLog(__FUNCTION__, 0xe1f)("./log/DBQueryErr",
            "CDBManager::ChangeGuildMemberGrade() SetQuery Break,guild_id=%d,charac_name=%s",
            guildId, name);
        return 0;
    }
    ret = h->exec(0x4e5b);
    if (!ret)
    {
        CMyFileLog(__FUNCTION__, 0xe26)("./log/DBQueryErr",
            "CDBManager::ChangeGuildMemberGrade() Exce Break,guild_id=%d,charac_name=%s",
            guildId, name);
        return 0;
    }
    return 1;
}
char CDBManager::ChangeGuildMemberGrade(unsigned char serverId,
                                        unsigned int guildId,
                                        unsigned char grade,
                                        unsigned int characNo)
{
    bool ret;
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e5b,
                      "upDate guild_member set grade = %d where guild_id = %d and server_id = %d and  charac_no = %d and member_flag = 1",
                      grade, guildId, serverId, characNo))
    {
        CMyFileLog(__FUNCTION__, 0xe3a)("./log/DBQueryErr",
            "CDBManager::ChangeGuildMemberGrade() SetQuery Break,guild_id=%d,charac_no=%d",
            guildId, characNo);
        return 0;
    }
    ret = h->exec(0x4e5b);
    if (!ret)
    {
        CMyFileLog(__FUNCTION__, 0xe41)("./log/DBQueryErr",
            "CDBManager::ChangeGuildMemberGrade() Exec Break,guild_id=%d,charac_no=%d",
            guildId, characNo);
        return 0;
    }
    return 1;
}
bool CDBManager::ChangeGuildNotifyMessage(int guildId, unsigned int m_id,
                                          char* msg)
{
    CDBHandle* h = m_handles[8];    // guild db
    char buf[0x6002];
    memset(buf, 0, 0x6002);
    h->escape_string(buf, msg);
    if (!h->set_query(0x4e62,
                      "upDate guild_notice set notice='%s' where guild_id = %d",
                      buf, guildId))
    {
        CMyFileLog log(__FUNCTION__, 0xd3b);
        log("./log/DBQueryErr",
            "CDBManager::ChangeGuildNotifyMessage() upDate guild_notice set notice='%s' where guild_id = %d",
            msg, guildId);
        return 0;
    }
    if (h->exec(0x4e62) != 1 || h->getAffectedRowCount() == 0)
    {
        if (!h->set_query(0x4e63,
                          "inSert into guild_notice set guild_id=%d,notice='%s',acc_date=unix_timestamp(now())",
                          guildId, buf))
        {
            CMyFileLog log(__FUNCTION__, 0xd4b);
            log("./log/DBQueryErr",
                "CDBManager::ChangeGuildNotifyMessage() Exception Break\n");
            return 0;
        }
        if (!h->exec(0x4e63))
            return 0;
    }
    return 1;
}
bool CDBManager::QueryGuildNotiMessage(unsigned char serverGroup,
                                       unsigned int guildId,
                                       Packet_DB_Reply_Query_Guild& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    if (!h->set_query(0x4e64, "seLect notice from guild_notice where guild_id = %d",
                      guildId))
    {
        CMyFileLog log(__FUNCTION__, 0xd65);
        log("./log/DBQueryErr", "CDBManager::QueryGuildMember() Exception Break\n");
        return 0;
    }
    if (!h->exec(0x4e64))
        return 0;
    if (!h->fetch())
        return 1;
    if (!h->get_str(0, (char*)&reply + 0xd0, 0x64))
        return 0;
    return 1;
}
bool CDBManager::QueryGuildSkill(unsigned char serverGroup,
                                 unsigned int guildId,
                                 Packet_DB_Reply_Query_Guild& reply)
{
    CDBHandle* h = m_handles[8];    // guild db
    CDBHandle* h2 = m_handles[2];   // game db
    char* info = (char*)&reply + 0x13;
    if (!h->set_query(0x4e56, "seLect remain_sp, used_sp, skill_slot from guild_skill where guild_id = %d",
                      guildId))
    {
        CMyFileLog log(__FUNCTION__, 0xcec);
        log("./log/DBQueryErr",
            "CDBManager::QueryGuild() seLect remain_sp, skill_slot from guild_skill where guild_id = %d and server_id = %d and expire_flag = 0",
            guildId, serverGroup);
        *(char*)((char*)&reply + 0xa) = 0;
        return 0;
    }
    if (!h->exec(0x4e56))
    {
        *(char*)((char*)&reply + 0xa) = 0;
        return 0;
    }
    if (!h->fetch())
    {
        *(char*)((char*)&reply + 0xa) = 1;
        return 1;
    }
    if (!h->get_ushort(0, *(unsigned short*)(info + 0x42)))
    {
        *(char*)((char*)&reply + 0xa) = 3;
        return 1;
    }
    if (!h->get_ushort(1, *(unsigned short*)(info + 0x44)))
    {
        *(char*)((char*)&reply + 0xa) = 3;
        return 1;
    }
    if (*(unsigned char*)(info + 0x44) != 0)
    {
        if (!h->get_binary(2, info + 0x45,
                           *(unsigned char*)(info + 0x44) * 5))
        {
            *(char*)((char*)&reply + 0xa) = 3;
            return 1;
        }
    }
    *(char*)((char*)&reply + 0xa) = 1;
    return 1;
}
void CPacketTranslater::OnChangeUnconnectedGuildMemberGrade(PacketHeader* header)
{
    if (!m_pclApp)
        return;
    try
    {
        char* h = (char*)header;
        Packet_DB_Monitor_Change_Unconnected_GuildMember_Grade pkt;
        if (*(int*)(h + 0xb) != 0)
        {
            memcpy((char*)&pkt + 0x12, h + 0x14, 0x1d);
            *(int*)((char*)&pkt + 0xa) = *(int*)(h + 0xb);
            *(char*)((char*)&pkt + 0x30) = *(char*)(h + 0x32);
            *(int*)((char*)&pkt + 0xe) = *(int*)(h + 0xf);
        }
        CGuildServer* gs = m_pclApp->m_serverHandler->GetGuildServer();
        unsigned int result = 0;
        m_pclApp->m_dbManager.QueryGuildMemberGradeByName(
            *(unsigned char*)(h + 0xa), *(unsigned int*)(h + 0xb), h + 0x14,
            *(unsigned char*)((char*)&pkt + 0x31),
            *(unsigned int*)((char*)&pkt + 0x32), result);
        if (!result)
        {
            *(char*)((char*)&pkt + 0x30) = 0xff;
            gs->SendToServer((char*)&pkt, pkt.packetSize);
            return;
        }
        if (*(unsigned char*)((char*)&pkt + 0x31) == 1)
        {
            gs->SendToServer((char*)&pkt, pkt.packetSize);
            return;
        }
        if (*(unsigned char*)((char*)&pkt + 0x31) == 2)
            goto sendlog;
        if (*(unsigned char*)(h + 0x32) != 2 ||
            *(unsigned char*)(h + 0x13) == 1)
        {
            if (!m_pclApp->m_dbManager.ChangeGuildMemberGrade(
                    *(unsigned char*)(h + 0xa), *(unsigned int*)(h + 0xb),
                    *(unsigned char*)(h + 0x32), h + 0x14))
                *(char*)((char*)&pkt + 0x30) = 0xff;
        }
        else
        {
            *(char*)((char*)&pkt + 0x30) = 0xfe;
            gs->SendToServer((char*)&pkt, pkt.packetSize);
            return;
        }
    sendlog:
        gs->SendToServer((char*)&pkt, pkt.packetSize);
        CMyFileLog log(__FUNCTION__, 0x495);
        log("./log/GuildModify",
            "::OnChangeUnconnectedGuildMemberGrade GRADE_CHANGE Guild(%d) UnConnected Name(%s) Grade(%d) Prev(%d)",
            *(unsigned int*)(h + 0xb), h + 0x14,
            *(unsigned char*)(h + 0x32),
            *(unsigned char*)((char*)&pkt + 0x31));
    }
    DNF_CATCH_LOG("./log/Except.log",
                  "CPacketTranslater::OnChangeUnconnectedGuildMemberGrade() Exception Break",
                  0x49a, 0x49f);
}
