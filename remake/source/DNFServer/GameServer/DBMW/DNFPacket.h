#ifndef DBMW_DNFPACKET_H_
#define DBMW_DNFPACKET_H_

#include "DBMWTypes.h"
#include "DNFFunctionLib.h"

struct STBuddyDBInfo
{
    char m_name[0x1e];        // +0
    short m_lev;              // +0x1e
    char m_job;               // +0x20
    char m_growType;          // +0x21
    unsigned int m_characNo;  // +0x22
    char m_sex;               // +0x26
} __attribute__((packed));

struct __attribute__((packed)) STGuildJoinInfo
{
    unsigned char m_serverId;  // +0
    char m_pad1[3];
    int m_guildId;             // +0x4
    unsigned int m_id;         // +0x8
    unsigned int m_fieldC;     // +0xc
    int m_characNo;            // +0x10
    char m_characName[0x1e];   // +0x14
    unsigned char m_lev;       // +0x32
    unsigned char m_growType;  // +0x33
    unsigned char m_job;       // +0x34
    unsigned char m_sex;       // +0x35
    char m_bornYear[3];        // +0x36
    char m_pad39[3];           // +0x39
};

struct STTodayGuildMember
{
    unsigned int m_field0;   // +0（charac_no）
    char m_name[0x1e];       // +4（charac_name，get_str 0x1d）
    unsigned char m_field22; // +0x22（grade）
    unsigned char m_field23; // +0x23（job）
    unsigned char m_field24; // +0x24（grow_type）
    unsigned char m_field25; // +0x25（sex）
    unsigned char m_field26; // +0x26（lev）
};

class Packet_DBMW_Statistic_Login_Logout : public PacketHeader
{
public:
    Packet_DBMW_Statistic_Login_Logout();
    char m_pad[0x5fe];  // +0xa .. +0x607
    int m_field608;     // +0x608
    int m_field60c;     // +0x60c
    int m_field610;     // +0x610
    int m_field614;     // +0x614
} __attribute__((packed));

class Packet_User_Count_Statistic : public PacketHeader
{
public:
    int m_gcNo;              // +0xa
    int m_userCount;         // +0xe
    short m_occ[0x64];       // +0x12（0..99 年龄段）
} __attribute__((packed));

class PacketInsertUpdate : public PacketHeader
{
public:
    int m_handleIdx;         // +0xa（DB 槽位）
    int m_updateQueryId;     // +0xe（update 查询 id）
    int m_insertQueryId;     // +0x12（insert 查询 id）
    char m_updateSql[0x801]; // +0x16（update SQL，exec 无影响行时执行）
    char m_insertSql[0x1000]; // +0x817（主 SQL）
} __attribute__((packed));

class Packet_Frame_Lag_Statistic_Write_Query : public PacketHeader
{
public:
    char m_query[0x800];     // +0xa
} __attribute__((packed));

class Packet_Frame_Lag_Used_Memory_Write_Query : public PacketHeader
{
public:
    char m_query[0x800];     // +0xa
} __attribute__((packed));

class Packet_Frame_Lag_Statistic_Write_Daily_Bad_Spec : public PacketHeader
{
public:
    int m_specId;            // +0xa
    char m_serverGroup;      // +0xe
    char m_pad[3];
} __attribute__((packed));

class Packet_DBMW_Reason_Crash_Down_Query : public PacketHeader
{
public:
    char m_query[0x800];     // +0xa
} __attribute__((packed));

class Packet_DBMW_TechnicalReport_Common_Query : public PacketHeader
{
public:
    char m_query[0x800];     // +0xa
} __attribute__((packed));

class Packet_DBMW_Powerwar_Lag_Report : public PacketHeader
{
public:
    int m_count;             // +0xa
    char m_sql[1][0x100];    // +0xe（SQL 字符串，步长 0x100）
} __attribute__((packed));

class Packet_DBMW_Powerwar_Loading_Time_Report : public PacketHeader
{
public:
    int m_count;             // +0xa
    char m_sql[1][0x100];    // +0xe（SQL 字符串，步长 0x100）
} __attribute__((packed));

class Packet_DBMW_Ting_User_TimeCheck_Write_Query : public PacketHeader
{
public:
    int m_count;             // +0xa
    struct Entry
    {
        unsigned int m_id;   // +0xe
        int m_minute;        // +0x12
    } m_entries[1];          // +0xe，步长 8
} __attribute__((packed));

class Packet_DBMW_User_Ting_TimeCheck_Write_Query : public PacketHeader
{
public:
    int m_count;             // +0xa
    struct Entry
    {
        int m_minute;        // +0xe
        int m_count;         // +0x12
    } m_entries[1];          // +0xe，步长 8
} __attribute__((packed));

class Packet_DB_Request_Member_Delete : public PacketHeader
{
public:
    unsigned int m_characNo; // +0xa
} __attribute__((packed));

class Packet_GM_List : public PacketHeader
{
public:
    char m_flag;             // +0xa（0 时先 clearGmList）
    char m_count;            // +0xb（GM 数量）
    unsigned int m_ids[0x14];  // +0xc
    char m_flags[0x14];      // +0x5c
} __attribute__((packed));

class Packet_DBMW_Request_Guild_Notify_Message : public PacketHeader
{
public:
    unsigned int m_id;   // +0xa（ORIG 传参 0x8 槽，dword）
    unsigned char m_guildId;  // +0xe（ORIG 传参 0x4 槽，movzbl 单字节）
    char m_msg[0x1000];  // +0xf
} __attribute__((packed));

class Packet_DBMW_Query_Buddy : public PacketHeader
{
public:
    unsigned int m_characNo;  // +0xa
} __attribute__((packed));

class Packet_DBMW_Add_Buddy : public PacketHeader
{
public:
    int m_mid;       // +0xa
    char m_name[0x1e];  // +0xe
} __attribute__((packed));

class Packet_DBMW_PcRoom_PlayTime_Reward : public PacketHeader
{
public:
    unsigned char m_serverInfo;  // +0xa
    int m_accId;      // +0xb
    int m_characNo;   // +0xf
} __attribute__((packed));

class Packet_DBMW_Collect_Items_Update : public PacketHeader
{
public:
    int m_fieldA;          // +0xa
    unsigned char m_serverInfo;  // +0xe
    unsigned char m_field13;     // +0x13
} __attribute__((packed));

class Packet_DBMW_Collect_Items_Gm : public PacketHeader
{
public:
    unsigned char m_serverInfo;  // +0xa
    int m_fieldB;      // +0xb
    unsigned int m_fieldF;  // +0xf
    int m_field13;     // +0x13
} __attribute__((packed));

class Packet_DBMW_Request_Guild_Join : public PacketHeader
{
public:
    unsigned char m_serverId;    // +0xa
    unsigned int m_guildId;      // +0xb
    unsigned int m_id;           // +0xf
    unsigned int m_field13;      // +0x13
    int m_characNo;              // +0x17
    char m_characName[0x1d];     // +0x1b
    unsigned char m_lev;         // +0x39
    unsigned char m_growType;    // +0x3a
    unsigned char m_job;         // +0x3b
    unsigned char m_sex;         // +0x3c
    char m_bornYear[3];          // +0x3d
} __attribute__((packed));

class Packet_DBMW_Request_BlackList : public PacketHeader
{
public:
    unsigned int m_mid;      // +0xa
    char m_name[0x1e];       // +0xe
    int m_characNo;          // +0x2c
    int m_result;            // +0x30（仅 Register 使用）
} __attribute__((packed));

class Packet_DBMW_Send_Mail_Coin_Guild_Event : public PacketHeader
{
public:
    unsigned char m_serverId;  // +0xa
    unsigned int m_guildId;    // +0xb
    int m_count;               // +0xf
} __attribute__((packed));

class Packet_DBMW_Request_Change_Unconnected_GuildMember_Grade : public PacketHeader
{
public:
    unsigned char m_serverId;  // +0xa
    unsigned int m_guildId;    // +0xb
    unsigned int m_fieldF;     // +0xf
    unsigned char m_field13;   // +0x13
    char m_name[0x1d];         // +0x14
    unsigned char m_field32;   // +0x32
} __attribute__((packed));

class Packet_DBMW_Request_BlackList_Login : public PacketHeader
{
public:
    unsigned int m_mid;    // +0xa
    unsigned char m_fieldE;  // +0xe
} __attribute__((packed));

class Packet_DBMW_Request_Guild_Booting : public PacketHeader
{
public:
    unsigned char m_serverId;  // +0xa
} __attribute__((packed));

class Packet_DBMW_Request_Today_Guild_Member : public PacketHeader
{
public:
    unsigned int m_guildId;  // +0xa
} __attribute__((packed));

class Packet_DBMW_Connection_Check : public PacketHeader
{
public:
    unsigned char m_serverId;  // +0xa
} __attribute__((packed));

class Packet_DBMW_Query_Guild : public PacketHeader
{
public:
    unsigned char m_serverId;  // +0xa
    unsigned int m_guildId;    // +0xb
    unsigned int m_serverGroup;  // +0xf
} __attribute__((packed));

class Packet_DBMW_Query_Tower_Full_Rank : public PacketHeader
{
public:
    unsigned int m_towerIndex;  // +0xa
    unsigned int m_limit;       // +0xe
} __attribute__((packed));

class Packet_DBMW_Del_Buddy : public PacketHeader
{
public:
    unsigned int m_mid;      // +0xa
    unsigned int m_characNo; // +0xe
    char m_name[0x1d];       // +0x12
} __attribute__((packed));

class Packet_DB_Save_Power_War_Bonus_Point : public PacketHeader
{
public:
    int m_count;   // +0xa
    struct Entry
    {
        int m_characNo;  // +0xe
        int m_addInfo;   // +0x12
    } m_entries[1];      // +0xe，步长 8
} __attribute__((packed));

class Packet_DB_Load_Request_Guild_Board_Write : public PacketHeader
{
public:
    unsigned int m_guildId;  // +0xb
    unsigned int m_field13;  // +0x13
} __attribute__((packed));

class Packet_DB_Load_Request_Web_Guild_Board_Write : public PacketHeader
{
public:
    unsigned int m_guildId;  // +0xa
    unsigned int m_fieldE;   // +0xe
} __attribute__((packed));

class Packet_DB_Load_Request_Guild_Board_Delete : public PacketHeader
{
public:
    unsigned int m_fieldB;   // +0xb
    unsigned int m_fieldF;   // +0xf
    unsigned int m_field13;  // +0x13
} __attribute__((packed));

class Packet_DB_Load_Guild_Agit : public PacketHeader
{
public:
    unsigned int m_guildId;  // +0xa
} __attribute__((packed));

class Packet_DB_Create_Guild_Agit : public PacketHeader
{
public:
    unsigned int m_guildId;  // +0xa
    unsigned int m_fieldE;   // +0xe
} __attribute__((packed));

class Packet_DB_Delete_Guild_Agit : public PacketHeader
{
public:
    unsigned int m_guildId;  // +0xa
    unsigned int m_fieldE;   // +0xe
} __attribute__((packed));

class Packet_DB_Upgrade_Guild_Agit : public PacketHeader
{
public:
    unsigned int m_guildId;  // +0xa
    unsigned int m_fieldE;   // +0xe
} __attribute__((packed));

class Packet_StartGameEventFromServer : public PacketHeader
{
public:
    int m_eventType;      // +0xa
    int m_eventFlag;      // +0xe
    int m_serverId;       // +0x12
    int m_parameter1;     // +0x16（handler 按 int 读；CDBManager 按 ushort）
    unsigned short m_parameter2;  // +0x18
} __attribute__((packed));

class Packet_StopGameEventFromServer : public PacketHeader
{
public:
    int m_eventType;      // +0xa
    int m_serverId;       // +0xe
    unsigned int m_endTime;  // +0x12
} __attribute__((packed));

class Packet_DBMW_Save_Member_Exp : public PacketHeader
{
public:
    unsigned int m_characNo;  // +0xa
    unsigned int m_masterNo;  // +0xe
    unsigned int m_exp;       // +0x12
} __attribute__((packed));

class Packet_DBMW_Save_Power_Secede_Time : public PacketHeader
{
public:
    unsigned char m_serverId;   // +0xa
    unsigned int m_secedeTime;  // +0xb
} __attribute__((packed));

class Packet_DBMW_Query_Member : public PacketHeader
{
public:
    unsigned int m_characNo;  // +0xa
} __attribute__((packed));

class Packet_DBMW_Query_Guild_Member : public PacketHeader
{
public:
    unsigned char m_serverId;  // +0xa
    unsigned int m_guildId;    // +0xb
} __attribute__((packed));

class Packet_DBMW_Save_Member_Update_CharInfo : public PacketHeader
{
public:
    unsigned int m_characNo;  // +0xa
    unsigned char m_serverId; // +0xe
} __attribute__((packed));

class Packet_DB_Save_Power_War_Point : public PacketHeader
{
public:
    unsigned char m_serverId;    // +0xa
    signed char m_winnerSide;    // +0xb
    unsigned int m_aSidePoint;   // +0xc
    unsigned int m_bSidePoint;   // +0x10
} __attribute__((packed));

class Packet_DBMW_Send_Guild_Letter : public PacketHeader
{
public:
    unsigned int m_fieldA;   // +0xa
    unsigned char m_serverId; // +0xe
    unsigned int m_guildId;  // +0xf
    char m_content[0x100];   // +0x13
    char m_subject[0x100];   // +0x113
    int m_flag;              // +0x124
} __attribute__((packed));

class Packet_DB_Save_Guild_War_Info : public PacketHeader
{
public:
    unsigned int m_serverId;    // +0xa
    unsigned int m_guildIds[10];  // +0xb
    unsigned int m_points[10];    // +0x33
} __attribute__((packed));

class Packet_DBMW_Save_Member : public PacketHeader
{
public:
    unsigned char m_type;    // +0xa
    unsigned int m_fieldB;   // +0xb
    unsigned int m_fieldF;   // +0xf
    unsigned char m_field13; // +0x13
} __attribute__((packed));

class Packet_TowerOfDespair_Statistic_STD : public PacketHeader
{
public:
    int m_serverId;  // +0xa
    int m_uv;        // +0xe（与 m_entries[0] 同址）
    struct Entry
    {
        int m_fieldE;   // +0xe
        int m_field12;  // +0x12
    } m_entries[1];     // +0xe，步长 8（条目从 i=1 起用）
} __attribute__((packed));

class Packet_DBMW_Request_IPCounter_List : public PacketHeader
{
public:
    unsigned char m_serverGroup;  // +0xa
    unsigned char m_fieldB;       // +0xb
} __attribute__((packed));

class Packet_DBMW_Request_Approve_Join_Guild : public PacketHeader
{
public:
    unsigned int m_guildId;   // +0xa
    unsigned int m_id;        // +0xe
    unsigned int m_characNo;  // +0x12
    signed char m_serverId;   // +0x16
} __attribute__((packed));

class Packet_Result_OnTimeEvent_Idx : public PacketHeader
{
public:
    Packet_Result_OnTimeEvent_Idx();
    int m_fieldA;       // +0xa
    char m_fieldE;      // +0xe
} __attribute__((packed));

class Packet_Frame_Lag_Statistic_Reload_Spec : public PacketHeader
{
public:
    unsigned char m_fieldA;  // +0xa
    int m_fieldB;            // +0xb
} __attribute__((packed));

class Packet_Frame_Lag_Collect_Interval_Check : public PacketHeader
{
public:
    Packet_Frame_Lag_Collect_Interval_Check();
    short m_fieldA;          // +0xa
} __attribute__((packed));

class Packet_Frame_Lag_Statistic_Result_Reload_Spec : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Result_Reload_Spec();
    unsigned char m_fieldA;              // +0xa
    int m_batchIndex;                    // +0xb（每 6 条发送后自增）
    int m_count;                         // +0xf
    int m_uniqueId[6];                   // +0x13
    unsigned int m_modifyTime[6];        // +0x2b
    int m_specId[6];                     // +0x43
    unsigned char m_cpuVendor[6];        // +0x5b
    unsigned char m_cpuProcessorNum[6];  // +0x61
    int m_aboveCpuClock[6];              // +0x67
    int m_belowCpuClock[6];              // +0x7f
    unsigned short m_ram[6];             // +0x97
    int m_videocardVendor[6];            // +0xa3
    int m_videocardDevice[6];            // +0xbb
    unsigned short m_videocardTextureMem[6]; // +0xd3
    unsigned char m_osVersion[6];        // +0xdf
} __attribute__((packed));

class Packet_Frame_Lag_Statistic_Result_Load_Spec : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Result_Load_Spec();
    unsigned char m_fieldA;  // +0xa
    char m_pad[0xda];  // 数据区（ORIG ctor size 0xe5）
} __attribute__((packed));

class Packet_Frame_Lag_Statistic_Load_Spec : public PacketHeader
{
public:
    unsigned char m_fieldA;  // +0xa
} __attribute__((packed));

class Packet_Frame_Lag_Spec_Delete_Notify : public PacketHeader
{
public:
    Packet_Frame_Lag_Spec_Delete_Notify();
    char m_pad[0x4];  // 数据区（ORIG ctor size 0xe）
} __attribute__((packed));

class Packet_Frame_Lag_Statistic_Write_Lag_Index : public PacketHeader
{
};

class Packet_CollectItemsResult : public PacketHeader
{
public:
    Packet_CollectItemsResult();
    int m_fieldA;       // +0xa
    int m_fieldE;       // +0xe
    int m_field12;      // +0x12
} __attribute__((packed));

class Packet_DBMW_Add_Buddy_Reply : public PacketHeader
{
public:
    Packet_DBMW_Add_Buddy_Reply();
    int m_fieldA;          // +0xa
    STBuddyDBInfo m_info;  // +0xe（STBuddyDBInfo 区）
    char m_field35;        // +0x35（ORIG ctor size 0x36）
} __attribute__((packed));

class Packet_DBMW_Del_Buddy_Reply : public PacketHeader
{
public:
    Packet_DBMW_Del_Buddy_Reply();
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    char m_rest[0x1f];      // +0x12（ORIG ctor size 0x31）
} __attribute__((packed));

class Packet_DB_Reply_Guild_Secede : public PacketHeader
{
public:
    Packet_DB_Reply_Guild_Secede();
    unsigned int m_fieldA;   // +0xa
    unsigned int m_fieldE;   // +0xe
    unsigned int m_field12;  // +0x12
    unsigned int m_field16;  // +0x16
    char m_field1A;          // +0x1a
    unsigned int m_field1B;  // +0x1b
    char m_pad[0x1e];        // +0x1f..0x3c
    unsigned int m_field3D;  // +0x3d（ORIG ctor size 0x41）
} __attribute__((packed));

class Packet_Notify_New_Group_Mail : public PacketHeader
{
public:
    Packet_Notify_New_Group_Mail();
    char m_pad[0x4b4];  // 数据区（ORIG ctor size 0x4be）
} __attribute__((packed));

class Packet_Response_IPCounterList : public PacketHeader
{
public:
    Packet_Response_IPCounterList();
    char m_fieldA;       // +0xa
    char m_fieldB;       // +0xb
    char m_rest[0xbb8];  // 数据区（ORIG ctor size 0xbc4）
} __attribute__((packed));

class Packet_DBMW_Reply_Guild_Create : public PacketHeader
{
public:
    Packet_DBMW_Reply_Guild_Create();
    unsigned int m_fieldA;   // +0xa
    unsigned int m_fieldE;   // +0xe
    unsigned int m_field12;  // +0x12
    char m_rest[0x17];       // +0x16（ORIG ctor size 0x2d）
} __attribute__((packed));

class Packet_Reply_Today_Guild_Member : public PacketHeader
{
public:
    Packet_Reply_Today_Guild_Member();
    int m_fieldA;        // +0xa
    char m_data[0x27];   // +0xe（STTodayGuildMember，0x27 字节）
} __attribute__((packed));

class Packet_Response_D_IPCounterList : public PacketHeader
{
public:
    Packet_Response_D_IPCounterList();
    char m_fieldA;       // +0xa
    char m_fieldB;       // +0xb
    char m_rest[0xe10];  // 数据区（ORIG ctor size 0xe1c）
} __attribute__((packed));

class Packet_Result_Ontime_Event_Item : public PacketHeader
{
public:
    Packet_Result_Ontime_Event_Item();
    int m_fieldA;             // +0xa
    int m_fieldE;             // +0xe
    unsigned short m_field12; // +0x12
} __attribute__((packed));

class Packet_DB_Create_Guild_Agit_Reply : public PacketHeader
{
public:
    Packet_DB_Create_Guild_Agit_Reply();
    int m_fieldA;    // +0xa
    int m_fieldE;    // +0xe
    int m_field12;   // +0x12
} __attribute__((packed));

class Packet_DB_Delete_Guild_Agit_Reply : public PacketHeader
{
public:
    Packet_DB_Delete_Guild_Agit_Reply();
    int m_fieldA;    // +0xa
    int m_fieldE;    // +0xe
    int m_field12;   // +0x12
} __attribute__((packed));

class Packet_DBMW_Query_Buddy_Info_Reply : public PacketHeader
{
public:
    Packet_DBMW_Query_Buddy_Info_Reply();
    int m_fieldA;             // +0xa
    unsigned char m_fieldE;   // +0xe
    STBuddyDBInfo m_rest[32]; // +0xf（ORIG ctor size 0x4ef；0x27*32=0x4e0）
} __attribute__((packed));

class Packet_DB_Upgrade_Guild_Agit_Reply : public PacketHeader
{
public:
    Packet_DB_Upgrade_Guild_Agit_Reply();
    int m_fieldA;    // +0xa
    int m_fieldE;    // +0xe
    int m_result;    // +0x12
} __attribute__((packed));

class Packet_DB_Reply_Guild_Master_Delegate : public PacketHeader
{
public:
    Packet_DB_Reply_Guild_Master_Delegate();
    unsigned int m_fieldA;   // +0xa
    unsigned int m_fieldE;   // +0xe
    unsigned int m_field12;  // +0x12
    char m_rest[0x22];       // +0x16（ORIG ctor size 0x38）
} __attribute__((packed));

class Packet_DB_Response_Approve_Join_Guild : public PacketHeader
{
public:
    Packet_DB_Response_Approve_Join_Guild();
    int m_fieldA;     // +0xa
    int m_fieldE;     // +0xe
    int m_field12;    // +0x12
    int m_field16;    // +0x16
    STGuildJoinInfo m_joinInfo;  // +0x1a（0x3c 字节）
} __attribute__((packed));

class Packet_Result_Loading_Periodic_Message : public PacketHeader
{
public:
    Packet_Result_Loading_Periodic_Message();
    char m_message[0x200];  // +0xa
    int m_field20A;      // +0x20a
    int m_field20E;      // +0x20e
} __attribute__((packed));

class Packet_DB_Load_Reply_Guild_Board_Delete : public PacketHeader
{
public:
    Packet_DB_Load_Reply_Guild_Board_Delete();
    unsigned short m_fieldA;  // +0xa
    int m_fieldC;             // +0xc
    int m_field10;            // +0x10
    int m_field14;            // +0x14
} __attribute__((packed));

class Packet_DB_Monitor_Change_Unconnected_GuildMember_Grade : public PacketHeader
{
public:
    Packet_DB_Monitor_Change_Unconnected_GuildMember_Grade();
    int m_fieldA;      // +0xa
    int m_fieldE;      // +0xe
    char m_pad[0x1e];  // +0x12
    unsigned char m_field30;  // +0x30
    unsigned char m_field31;  // +0x31
    int m_field32;     // +0x32
} __attribute__((packed));

class Packet_DBMW_Save_Client_Spec_Statistic : public PacketHeader
{
};

class Packet_DBMW_Assert_Manager_Info_Write_Query : public PacketHeader
{
};

class Packet_DBMW_Cube_Statistic : public PacketHeader
{
};

class Packet_DBMW_DeathTower_Statistic_Playdata_Job : public PacketHeader
{
};

class Packet_DBMW_DeathTower_Statistic_Value : public PacketHeader
{
};

class Packet_Avater_Disjoint_Statistic_DB : public PacketHeader
{
};

class Packet_DBMW_DeathTower_Statistic_Playdata_Party : public PacketHeader
{
};

class Packet_Req_Ontime_Event_Idx_Update : public PacketHeader
{
public:
    unsigned int m_itemIndex;  // +0xa
    unsigned int m_itemCount;  // +0xe
    unsigned int m_no;         // +0x12
} __attribute__((packed));

class Packet_Stat_Compatibility_Index : public PacketHeader
{
};

class Packet_Secret_Shop_Statistic : public PacketHeader
{
};

class Packet_UnChangable_GuildInfo_Save : public PacketHeader
{
};

class Packet_DB_Load_Request_Guild_Board_Open : public PacketHeader
{
};

class Packet_Server_Queue_Load_Statistic : public PacketHeader
{
public:
    Packet_Server_Queue_Load_Statistic();
    unsigned char m_fieldA;       // +0xa
    unsigned char m_fieldB;       // +0xb
    unsigned short m_fieldC;  // +0xc
} __attribute__((packed));

class Packet_DB_Query_Reply_On_Guild_Booting : public PacketHeader
{
public:
    Packet_DB_Query_Reply_On_Guild_Booting();
    unsigned int m_fieldA;   // +0xa
    unsigned int m_fieldE;   // +0xe
    signed char m_field12;   // +0x12（ORIG ctor size 0x13）
} __attribute__((packed));

class Packet_Monitor_Notify_New_Mail : public PacketHeader
{
public:
    Packet_Monitor_Notify_New_Mail();
    unsigned int m_fieldA;  // +0xa
    char m_rest[4];         // +0xe（ORIG ctor size 0x12）
} __attribute__((packed));

class Packet_DBMW_Reply_Guild_Mail : public PacketHeader
{
public:
    Packet_DBMW_Reply_Guild_Mail();
    unsigned int m_fieldA;   // +0xa
    unsigned int m_fieldE;   // +0xe
    char m_field12;          // +0x12（ORIG ctor size 0x13）
} __attribute__((packed));

class Packet_DBMW_Save_Guild_Join_Reply : public PacketHeader
{
public:
    Packet_DBMW_Save_Guild_Join_Reply();
    unsigned int m_fieldA;   // +0xa
    unsigned int m_fieldE;   // +0xe
    unsigned int m_field12;  // +0x12
    unsigned int m_field16;  // +0x16（ORIG ctor size 0x1a）
} __attribute__((packed));

class Packet_Reply_Load_Tower_Full_Rank : public PacketHeader
{
public:
    Packet_Reply_Load_Tower_Full_Rank();
    char m_fieldA;            // +0xa
    char m_fieldB;            // +0xb
    char m_rest[0x17b3];      // +0xc（ORIG ctor size 0x17bf）
} __attribute__((packed));

class Packet_Set_ARS_Info : public PacketHeader
{
public:
    Packet_Set_ARS_Info();
    int m_fieldA;        // +0xa（ORIG 以 32 位存储 count）
    char m_fieldE;       // +0xe
    char m_rest[0x4b0];  // +0xf（ORIG ctor size 0x4bf）
} __attribute__((packed));

class Packet_Result_Ontime_Event_Idx_Update : public PacketHeader
{
public:
    Packet_Result_Ontime_Event_Idx_Update();
    int m_fieldA;        // +0xa（ORIG ctor 报文长 0x16，数据区仅此字段）
} __attribute__((packed));

struct STDBConnInfo
{
    STDBConnInfo();
    char m_host[0x11];      // +0
    char m_pad11[3];
    unsigned int m_port;    // +0x14
    char m_user[0x15];      // +0x18
    char m_pass[0x15];      // +0x2d
    char m_db[0x1f];        // +0x42
    char m_pad61[0x100];    // +0x61
    int m_tail;             // +0x164
};

struct STGuildRankInfo
{
    STGuildRankInfo();
    int m_field0;    // +0
    unsigned int m_field4;    // +4
    unsigned int m_field8;    // +8（rank）
};

struct STGuildWarRankInfo
{
    STGuildWarRankInfo();
    unsigned int m_field0;  // +0（guild_id）
    unsigned int m_field4;  // +4（guild_war_point，pair.first）
    unsigned int m_field8;  // +8（guild_point_prev）
    char m_name[0x17];      // +0xc（guild_name）
    unsigned int m_field24; // +0x24
};

struct STGuildSkill
{
    STGuildSkill();
    int m_field0;    // +0（默认 0xffffffff）
    char m_field4;   // +4（默认 0xff）
} __attribute__((packed));

struct CPairDataCompare
{
    bool operator()(const std::pair<unsigned int, STGuildRankInfo*>& a,
                    const std::pair<unsigned int, STGuildRankInfo*>& b) const;
    bool keyLess(const unsigned int& a, const unsigned int& b) const;
};

struct CPairDataGuildWarCompare
{
    bool operator()(const std::pair<unsigned int, STGuildWarRankInfo*>& a,
                    const std::pair<unsigned int, STGuildWarRankInfo*>& b) const;
    bool keyLess(const unsigned int& a, const unsigned int& b) const;
};

struct STGuildMemberProxy
{
    STGuildMemberProxy();
    int m_no;               // +0
    char m_name[0x1e];      // +4
    unsigned char m_field22;   // +0x22
    unsigned char m_field23;   // +0x23
    unsigned short m_field24;  // +0x24
    unsigned char m_field26;   // +0x26
    unsigned char m_field27;   // +0x27
    int m_field28;          // +0x28
    char m_data2c[0x15];    // +0x2c
} __attribute__((packed));

struct RandomOptionSeed
{
    void reset();
    unsigned char m_seed;  // +0
};

struct RandomOptionField
{
    void reset();
    unsigned char m_b0;    // +0
    unsigned char m_b1;    // +1
    unsigned char m_b2;    // +2
};

struct RandomOption
{
    void reset();
    RandomOptionField m_field0;  // +0（3）
    RandomOptionField m_field1;  // +3
    RandomOptionField m_field2;  // +6
    RandomOptionSeed m_seed0;    // +9
    RandomOptionField m_field3;  // +0xa
    RandomOptionSeed m_seed1;    // +0xd
};

struct UpgradeSeparateInfo
{
    UpgradeSeparateInfo();
    void reset();
    unsigned char GetUpgradeSeparate() const;
    unsigned char m_bits;  // +0（b0:5 / b1:1 / b2:2）
} __attribute__((packed));

struct ReservedCapacity
{
    ReservedCapacity();
    void reset();
    int m_field0;   // +0
    int m_field4;   // +4
    char m_field8;  // +8
} __attribute__((packed));

struct NpcBuyLimitItem
{
    NpcBuyLimitItem();
    void clear();
    int m_field0;   // +0
    int m_field4;   // +4
    int m_field8;   // +8
} __attribute__((packed));

struct STGuildAgitDBInfo
{
    STGuildAgitDBInfo();
    char m_field0;  // ORIG ctor 只写 +0，Agit 报文总长 0xf
} __attribute__((packed));

struct DnfItemInfo
{
    DnfItemInfo();
    void reset();
    char m_field0;               // +0
    int m_field1;                // +1
    char m_field5;               // +5
    int m_field6;                // +6
    unsigned short m_fieldA;     // +0xa
    int m_fieldC;                // +0xc
    char m_field10;              // +0x10
    unsigned short m_field11;    // +0x11
    char m_pad13[0xa];           // +0x13..0x1c
    RandomOption m_option;       // +0x1d
    UpgradeSeparateInfo m_upg;   // +0x2b
    ReservedCapacity m_resv;     // +0x2c（总大小 0x35）
} __attribute__((packed));

struct STGuildMemberCharacData
{
    STGuildMemberCharacData();
    unsigned char m_field0;      // +0
    unsigned char m_field1;      // +1
    unsigned char m_field2;      // +2
    char m_name[0x1e];           // +3（总大小 0x21）
} __attribute__((packed));

struct STBlackUserDBType
{
    STBlackUserDBType();
    int m_field0;     // +0
    char m_pad4[0x20];  // +4..0x23
    int m_field24;    // +0x24（总大小 0x28）
} __attribute__((packed));

struct STGuildBoardDBInfo
{
    STGuildBoardDBInfo();
    char m_pre[0x78];                    // +0
    int m_field78;                       // +0x78
    int m_field7c;                       // +0x7c
    int m_field80;                       // +0x80
    STGuildMemberCharacData m_member;    // +0x84（0x21 字节，总大小 0xa5）
} __attribute__((packed));

struct STGuildCargoDBInfo
{
    STGuildCargoDBInfo();
    DnfItemInfo m_items[0x78];  // +0（0x18d8 字节）
    int m_tail;                  // +0x18d8（总大小 0x18dc）
} __attribute__((packed));

struct STGuildMemerDBInfo
{
    STGuildMemerDBInfo();
    char m_pad[0x15];  // +0..0x14
    char m_field15;    // +0x15
    int m_field16;     // +0x16（总大小 0x1a）
} __attribute__((packed));

struct st_ip_counter_list
{
    ~st_ip_counter_list();
    void CopyStruct(const st_ip_counter_list& other);
    unsigned short m_field0;  // +0（hack_type）
    unsigned short m_field2;  // +2（hack_sub_type）
    char m_cClassIp[0xc];     // +4
    unsigned int m_field10;   // +0x10（cnt）
};

struct st_full_ip_counter_list
{
    ~st_full_ip_counter_list();
    void CopyStruct(const st_full_ip_counter_list& other);
    unsigned short m_field0;  // +0（hack_type）
    unsigned short m_field2;  // +2（hack_sub_type）
    char m_fullIp[0x10];      // +4
    unsigned int m_field14;   // +0x14（cnt）
};

struct stTowerRank_t
{
    ~stTowerRank_t();
    unsigned int m_characNo;     // +0
    unsigned short m_towerIndex; // +4
    unsigned short m_rank;       // +6
    char m_partType;             // +8
    char m_memberInfo[0x5c];     // +9（len = rank*0x17）
};

struct st_ars_info_list
{
    ~st_ars_info_list();
    void CopyStruct(const st_ars_info_list& other);
    unsigned short m_field0;  // +0（hack_type）
    unsigned short m_field2;  // +2（cnt）
    unsigned short m_field4;  // +4（etc）
    unsigned short m_field6;  // +6（hack_sub_type）
    unsigned char m_field8;   // +8（hack_sub_cnt）
    unsigned char m_field9;   // +9（apply_flag）
    unsigned short m_fieldA;  // +0xa（ip_cnt）
};

struct STGuildDBInfoOnly
{
    STGuildDBInfoOnly();
    char m_guildName[0x17];       // +0
    int m_masterNo;               // +0x17
    unsigned char m_lev;          // +0x1b
    union
    {
        unsigned short m_ability; // +0x1c（bit 标志）
        struct
        {
            unsigned char b0;
            unsigned char b1;
        };
    };
    int m_field1E;                // +0x1e..0x21
    unsigned short m_memberCount; // +0x22
    int m_guildPoint;             // +0x24
    unsigned char m_guildRank;    // +0x28
    int m_guildExp;               // +0x29
    char m_field2D;               // +0x2d
    char m_pad2E[0x14];           // +0x2e..0x41
    unsigned short m_field42;     // +0x42..0x43
    char m_field44;               // +0x44
    STGuildSkill m_skills[16];    // +0x45..0x94（16×STGuildSkill(5B)=0x50）
    unsigned char m_powerSide;    // +0x95
    int m_powerSecedeTime;        // +0x96
    int m_powerWarPoint;          // +0x9a
    unsigned char m_guildAgitFlag; // +0x9e
    unsigned char m_powerJoinCount; // +0x9f
    int m_guildFund;              // +0xa0
    char m_masterName[0x15];      // +0xa4
    int m_fieldB9;                // +0xb9
} __attribute__((packed));

class Packet_DBMW_Save_Guild : public PacketHeader
{
public:
    unsigned char m_serverId;    // +0xa
    unsigned int m_guildId;      // +0xb
    STGuildDBInfoOnly m_info;    // +0xf（0xbd 字节，至 +0xcb）
    int m_fieldCC;               // +0xcc（0=SaveGuildInfo，1=SaveGuildSkill）
} __attribute__((packed));

class Packet_DBMW_Save_Guild_Member : public PacketHeader
{
public:
    unsigned char m_serverId;    // +0xa
    unsigned int m_guildId;      // +0xb
    unsigned int m_fieldF;       // +0xf
    STGuildMemerDBInfo m_info;   // +0x13（0x1a 字节，至 +0x2c）
    unsigned char m_field2D;     // +0x2d
} __attribute__((packed));

class Packet_DBMW_Insert_Mail : public PacketHeader
{
public:
    unsigned int m_characNo;  // +0xa
    unsigned int m_fieldE;    // +0xe
    int m_field12;            // +0x12
    int m_field16;            // +0x16
    char m_subject[0x15];     // +0x1a
    char m_content[0x100];    // +0x2f
    int m_delayHours;         // +0x12f
} __attribute__((packed));

class LimitNpcBuyItemRequestInfo;  // 请求包（load 流程未使用字段，占位）

class LimitNpcBuyItemResultInfo : public PacketHeader
{
public:
    LimitNpcBuyItemResultInfo();
    int m_count;                 // +0xa
    NpcBuyLimitItem m_items[0x1e];  // +0xe（0x1e × 0xc = 0x168，总大小 0x176）
} __attribute__((packed));

class LimitNpcBuyItemUpdate : public PacketHeader
{
public:
    unsigned int m_fieldA;   // +0xa（sell_count 增量）
    unsigned int m_fieldE;   // +0xe
    unsigned int m_field12;  // +0x12（item_index）
    unsigned int m_field16;  // +0x16
} __attribute__((packed));

struct ST_ServerInfo
{
    ST_ServerInfo();
    ~ST_ServerInfo();
    char m_type;       // +0
    char m_flag;       // +1
    char m_idx;        // +2
    char m_pad3;
    std::string m_name;  // +4
    unsigned short m_port;  // +8
    char m_padA[2];
};

struct stPacketProcess
{
    double m_accTime;      // +0（map 节点 +4）
    unsigned int m_count;  // +8（map 节点 +0xc）
};

class Packet_DB_Call_Unconn_Guild_Member : public PacketHeader
{
public:
    unsigned int m_fieldA;  // +0xa（guildKey）
    unsigned int m_fieldE;  // +0xe（characNo）
} __attribute__((packed));

class Packet_DB_Call_Guild_All_Members : public PacketHeader
{
public:
    unsigned int m_fieldA;  // +0xa（guildKey）
    unsigned int m_fieldE;  // +0xe（characNo）
} __attribute__((packed));

class Packet_Tcp_Server_Login : public PacketHeader
{
public:
    unsigned char m_idx;  // +0xa
} __attribute__((packed));

class Packet_Tcp_Server_Logout : public PacketHeader
{
public:
    unsigned char m_idx;  // +0xa
} __attribute__((packed));

class Packet_Tcp_Server_Heartbeat : public PacketHeader
{
public:
    unsigned char m_idx;  // +0xa
} __attribute__((packed));

class Packet_Monitor_UDP_HeartBeat : public PacketHeader
{
public:
    Packet_Monitor_UDP_HeartBeat();
    char m_fieldA;      // +0xa
} __attribute__((packed));

class Packet_Notice_Guild_Mail_Arrived : public PacketHeader
{
public:
    Packet_Notice_Guild_Mail_Arrived();
    char m_fieldA;      // +0xa
    int m_guildId;      // +0xb
    char m_rest[0x28];  // +0xf..0x33
} __attribute__((packed));

struct ST_MemberProxy
{
    ST_MemberProxy();
    int m_no;               // +0
    unsigned char m_lev;    // +4
    char m_name[0x1e];      // +5（memset 0x1e）
    int m_field23;          // +0x23
} __attribute__((packed));

struct STMemberDBInfo
{
    STMemberDBInfo();
    ST_MemberProxy m_proxy;     // +0
    char m_count;               // +0x27
    ST_MemberProxy m_lowers[10]; // +0x28
} __attribute__((packed));

class Packet_DB_Reply_Query_Member : public PacketHeader
{
public:
    Packet_DB_Reply_Query_Member();
    char m_flag;             // +0xa
    int m_fieldB;            // +0xb
    int m_fieldF;            // +0xf（maxExp）
    int m_field13;           // +0x13（maxIdx）
    STMemberDBInfo m_master; // +0x17
    char m_rest[0x1c5 - 0x17 - 0x1ae];
} __attribute__((packed));

class Packet_DB_Reply_Unconn_Guild_Member : public PacketHeader
{
public:
    Packet_DB_Reply_Unconn_Guild_Member();
    int m_fieldA;           // +0xa
    int m_fieldE;           // +0xe
    STGuildMemberProxy m_proxy;  // +0x12
} __attribute__((packed));

class Packet_DB_Reply_Guild_All_Members : public PacketHeader
{
public:
    Packet_DB_Reply_Guild_All_Members();
    int m_fieldA;            // +0xa
    int m_fieldE;            // +0xe
    unsigned char m_flag;    // +0x12
    unsigned char m_count;   // +0x13
    STGuildMemberProxy m_members[0x5d];  // +0x14（93 × 0x41 = 0x179d）
} __attribute__((packed));

class Packet_DB_Reply_Query_Guild : public PacketHeader
{
public:
    Packet_DB_Reply_Query_Guild();
    char m_fieldA;               // +0xa
    int m_fieldB;                // +0xb
    int m_fieldF;                // +0xf
    STGuildDBInfoOnly m_guildInfo;  // +0x13（0xbd 字节）
    char m_rest[0x65];           // +0xd0（ORIG ctor size 0x135）
} __attribute__((packed));

class Packet_Guild_Load_Guild_Agit : public PacketHeader
{
public:
    Packet_Guild_Load_Guild_Agit();
    int m_fieldA;               // +0xa
    STGuildAgitDBInfo m_info;   // +0xe
} __attribute__((packed));

class Packet_DBMW_Reponse_BlackList : public PacketHeader
{
public:
    Packet_DBMW_Reponse_BlackList();
    int m_fieldA;                        // +0xa
    STBlackUserDBType m_blackList[10];   // +0xe（10 × 0x28 = 0x190）
} __attribute__((packed));

class Packet_Guild_Load_Guild_Cargo : public PacketHeader
{
public:
    Packet_Guild_Load_Guild_Cargo();
    unsigned int m_fieldA;  // +0xa
    STGuildCargoDBInfo m_cargo;  // +0xe（0x18dc 字节）
} __attribute__((packed));

class Packet_DB_Reply_Query_Guild_Member : public PacketHeader
{
public:
    Packet_DB_Reply_Query_Guild_Member();
    char m_flag;              // +0xa
    int m_fieldB;             // +0xb
    int m_fieldF;             // +0xf
    STGuildMemerDBInfo m_info; // +0x13（0x1a 字节，总大小 0x2d）
} __attribute__((packed));

class Packet_DB_Load_Reply_Guild_Board_Open : public PacketHeader
{
public:
    Packet_DB_Load_Reply_Guild_Board_Open();
    unsigned short m_fieldA;  // +0xa
    char m_fieldC;            // +0xc
    int m_fieldD;             // +0xd
    int m_field11;            // +0x11
    char m_field15;           // +0x15
    STGuildBoardDBInfo m_boards[10];  // +0x16（10 × 0xa5 = 0x672）
} __attribute__((packed));

class Packet_Guild_Load_Guild_Cargo_History : public PacketHeader
{
public:
    Packet_Guild_Load_Guild_Cargo_History();
    int m_fieldA;                    // +0xa
    int m_fieldE;                    // +0xe
    STGuildCargoLog m_logs[50];      // +0x12（50 × 0x30 = 0x960，总大小 0x972）
} __attribute__((packed));

class Packet_DB_Load_Reply_Guild_Board_Write : public PacketHeader
{
public:
    Packet_DB_Load_Reply_Guild_Board_Write();
    unsigned short m_fieldA;  // +0xa
    int m_fieldC;             // +0xc
    int m_field10;            // +0x10
    STGuildBoardDBInfo m_board;  // +0x14（总大小 0xb9）
} __attribute__((packed));

class Packet_DB_Load_Reply_Web_Guild_Board_Write : public PacketHeader
{
public:
    Packet_DB_Load_Reply_Web_Guild_Board_Write();
    unsigned short m_fieldA;  // +0xa
    int m_fieldC;             // +0xc
    int m_field10;            // +0x10
    STGuildBoardDBInfo m_board;  // +0x14（总大小 0xb9）
} __attribute__((packed));

struct STPartyMemberStat
{
    char m_data0[0xe];      // +0..0xd
    unsigned short m_fieldE;  // +0xe
    int m_field10;            // +0x10
    char m_field14;           // +0x14（movsbl）
    unsigned char m_field15;  // +0x15
    unsigned char m_field16;  // +0x16
    unsigned char m_field17;  // +0x17
    unsigned char m_field18;  // +0x18
    char m_field19;           // +0x19（movsbl）
    int m_field1A;            // +0x1a
    int m_field1E;            // +0x1e
    int m_field22;            // +0x22
    int m_field26;            // +0x26
    int m_field2A;            // +0x2a
    int m_field2E;            // +0x2e
    int m_field32;            // +0x32
    int m_field36;            // +0x36
    int m_field3A;            // +0x3a
    int m_field3E;            // +0x3e
    int m_field42;            // +0x42
    int m_field46;            // +0x46
} __attribute__((packed));

class Packet_DBMW_Dungeon_Statistic_Party : public PacketHeader
{
public:
    Packet_DBMW_Dungeon_Statistic_Party() : PacketHeader(0x439, 0x34) {}
    int m_count;            // +0xa
    char m_padE[2];
    STPartyMemberStat m_members[1];  // +0x10（i*0x3c）
} __attribute__((packed));

struct STPartyJobMemberStat
{
    char m_data0[0xe];      // +0..0xd
    unsigned short m_fieldE;  // +0xe
    int m_field10;            // +0x10
    char m_field14;           // +0x14（movsbl）
    unsigned char m_field15;  // +0x15
    unsigned char m_field16;  // +0x16
    unsigned char m_field17;  // +0x17
    unsigned char m_field18;  // +0x18
    char m_field19;           // +0x19（movsbl）
    int m_field1A;            // +0x1a
    char m_field1E;           // +0x1e（movsbl）
    int m_field1F;            // +0x1f
    int m_field23;            // +0x23
} __attribute__((packed));

class Packet_DBMW_Dungeon_Statistic_Party_Job : public PacketHeader
{
public:
    Packet_DBMW_Dungeon_Statistic_Party_Job() : PacketHeader(0x439, 0x34) {}
    int m_count;            // +0xa
    char m_padE[2];
    STPartyJobMemberStat m_members[1];  // +0x10（i*0x19）
} __attribute__((packed));

struct STPartyCharacMemberStat
{
    char m_data0[0xe];       // +0..0xd
    unsigned short m_fieldE; // +0xe
    int m_field10;           // +0x10
    int m_field11;           // +0x11（与 +0x10 重叠读）
    char m_field14;          // +0x14（movsbl）
    unsigned char m_field15; // +0x15
    unsigned char m_field16; // +0x16
    int m_field17;           // +0x17
    char m_field1B;          // +0x1b（movsbl）
    char m_field1C;          // +0x1c（movsbl）
    int m_field1D;           // +0x1d
    int m_field21;           // +0x21
    int m_field25;           // +0x25
    int m_field29;           // +0x29
    int m_field2D;           // +0x2d
    int m_field31;           // +0x31
    int m_field35;           // +0x35
    int m_field39;           // +0x39
    int m_field3D;           // +0x3d
    int m_field41;           // +0x41
    int m_field45;           // +0x45
    int m_field49;           // +0x49
    int m_field4D;           // +0x4d
} __attribute__((packed));

class Packet_DBMW_Dungeon_Statistic_Party_Charac : public PacketHeader
{
public:
    Packet_DBMW_Dungeon_Statistic_Party_Charac() : PacketHeader(0x439, 0x34) {}
    int m_count;            // +0xa
    char m_padE[2];
    STPartyCharacMemberStat m_members[1];  // +0x10（i*0x43）
} __attribute__((packed));

struct STItemLimitItem
{
    unsigned int m_ipgNo;                     // +0（ipg_no）
    unsigned int m_itemNo;                    // +4（item_no）
    unsigned int m_itemCnt;                   // +8（item_cnt）
    unsigned char m_avatarPeriodType;         // +0xc（avatar_period_type）
    char m_padD[3];                           // +0xd
    unsigned int m_ceraPrice;                 // +0x10（cera_price）
    unsigned int m_goldPrice;                 // +0x14（gold_price）
    unsigned int m_sellCnt;                   // +0x18（sell_cnt）
    int m_totalCnt;                           // +0x1c（total_cnt）
    unsigned int m_restrictNo;                // +0x20（restrict_no）
    unsigned int m_startTime;                 // +0x24（start_time）
    unsigned int m_endTime;                   // +0x28（end_time）
    unsigned int m_npcIdx;                    // +0x2c（npc_idx）
    unsigned int m_condCharacJob;             // +0x30（cond_charac_job）
    unsigned short m_condLevBegin;            // +0x34（cond_lev_begin）
    unsigned short m_condLevEnd;              // +0x36（cond_lev_end）
    unsigned int m_condAccCreateTimeBegin;    // +0x38（cond_acc_create_time_begin）
    unsigned int m_condAccCreateTimeEnd;      // +0x3c（cond_acc_create_time_end）
    unsigned int m_condChaCreateTimeBegin;    // +0x40（cond_cha_create_time_begin）
    unsigned int m_condChaCreateTimeEnd;      // +0x44（cond_cha_create_time_end）
} __attribute__((packed));

class Packet_Item_Limit_Edition_Load_Data_Req : public PacketHeader
{
public:
    Packet_Item_Limit_Edition_Load_Data_Req() : PacketHeader(0x1008, 0x7ef) {}
    unsigned char m_fieldA;    // +0xa
    int m_fieldB;              // +0xb
    unsigned int m_fieldF;     // +0xf
    unsigned int m_list[0x100]; // +0x13
} __attribute__((packed));

class Packet_Item_Limit_Edition_Load_Data_Rpy : public PacketHeader
{
public:
    Packet_Item_Limit_Edition_Load_Data_Rpy();
    unsigned char m_fieldA;    // +0xa
    int m_fieldB;              // +0xb
    STItemLimitItem m_items[1]; // +0xf（0x48 × n，上限 0x1c）
    char m_pad[0x798];  // 数据区（ORIG ctor size 0x7ef）
} __attribute__((packed));

class Packet_Item_Limit_Edition_Update : public PacketHeader
{
public:
    Packet_Item_Limit_Edition_Update() : PacketHeader(0x1008, 0x7ef) {}
    int m_serverId;            // +0xa
    unsigned int m_count;      // +0xe
    char m_items[0x1000];      // 项在 +0，步长 9
} __attribute__((packed));

class Packet_DBMW_Change_Char_Name : public PacketHeader
{
public:
    Packet_DBMW_Change_Char_Name() : PacketHeader(0x439, 0x34) {}
    char m_serverId;           // +0xa
    int m_characNo;            // +0xb
    char m_name[0x25];         // +0xf..0x34
} __attribute__((packed));

class Packet_DB_Request_Guild_Secede : public PacketHeader
{
public:
    Packet_DB_Request_Guild_Secede() : PacketHeader(0x439, 0x34) {}
    int m_guildId;              // +0xa
    int m_characNo;             // +0xe
    char m_grade;               // +0x12
    int m_secedeType;           // +0x13
    char m_characName[0x1e];    // +0x17
    int m_masterCharacNo;       // +0x35
} __attribute__((packed));

class Packet_DBMW_Request_Guild_Create : public PacketHeader
{
public:
    Packet_DBMW_Request_Guild_Create() : PacketHeader(0x439, 0x34) {}
    unsigned char m_serverId;   // +0xa
    unsigned int m_id;          // +0xb
    int m_characNo;             // +0xf
    char m_characName[0x1e];    // +0x13
    char m_job;                 // +0x31（ORIG movsbl 有符号）
    char m_growType;            // +0x32（ORIG movsbl 有符号）
    char m_lev;                 // +0x33（ORIG movsbl 有符号）
    char m_sex;                 // +0x34（ORIG movsbl 有符号）
    char m_bornYear[3];         // +0x35
    char m_guildName[0x17];     // +0x38
    char m_guildUrl[0x40];      // +0x4f
} __attribute__((packed));

struct StackBufferContext
{
    StackBufferContext();
    ~StackBufferContext();
    struct Buffer
    {
        int m_blockIndex;  // +0
        int m_offset;      // +4
        int m_size;        // +8
    };
    std::vector<Buffer> m_buffers;        // +0
    std::vector<unsigned char*> m_blocks; // +0xc
    int m_blockIndex;                     // +0x18
    int m_offset;                         // +0x1c
};

#endif
