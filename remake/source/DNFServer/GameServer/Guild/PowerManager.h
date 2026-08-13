#ifndef POWERMANAGER_H_
#define POWERMANAGER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"
#include "PacketHeader.h"

#include "PacketHeader.h"
#include "PowerWarTypes.h"
#include "Power.h"
#include "PowerWar.h"

class CApplication;
class CPowerManager;
class CUser;
struct Packet_Channel_Power_War_Process_Info;
struct Packet_DB_Save_Power_War_Bonus_Point;
struct Packet_DB_Save_Power_War_Guild_Rank;
struct Packet_DB_Save_Power_War_Point;
struct Packet_DB_Save_Power_War_Point_Reward;
struct Packet_DB_Save_Power_War_Statue_Ranker;
struct Packet_DB_Save_Power_War_User_Rank;
struct Packet_Notice_Power_War_End_Time;
struct Packet_Notice_Power_War_Info;
struct Packet_Notice_Power_War_Rank;
struct Packet_Notice_Power_war_End_Info;
struct Packet_Reply_Power_War_Score;
struct STGuildRank;
struct STPowerWarPointInfo;
struct STUserPoint;
struct STUserRank;

// from GuildDomain.h
enum ENUM_POWER_SIDE_TYPE {};

// from GuildDomain.h
class CPowerManager
{
public:
    CPowerManager();
    virtual ~CPowerManager();
    void InitPowerManager(char* path, CApplication* app);
    void Process();
    void ProcessByMinute();
    bool IsPowerWarOn();
    void SetPowerInfo(char side, int score1, int score2);
    void CleanPowerWar();
    int GetPowerScore(ENUM_POWER_SIDE_TYPE side);
    char GetWinnerSide();
    int IncPowerScore(ENUM_POWER_SIDE_TYPE side, int score);
    void SetWinnerSide(char side);
    void PrintDebugInfo();
    void SetPowerDBFlag(unsigned short flag);
    void LoadPowerWarCfg(char* path);
    void CalcPowerWarRank(bool flag);
    void EndPowerWarEvent();
    void RewardBonusPoint();
    void SendPowerWarInfo();
    int ComputeWinnerSide();
    void SendPowerWarScore();
    void SaveDBPowerWarRank();
    void StartPowerWarEvent();
    void UpdatePowerWarInfo(bool flag, ENUM_POWER_SIDE_TYPE side, int score,
                            unsigned int* p);
    void SaveDBPowerWarPoint();
    void SendPowerWarEndInfo();
    void SendPowerWarEndInfo(int time);
    void SendPowerWarEndTime(int time);
    unsigned int GetUserPowerWarPoint(ENUM_POWER_SIDE_TYPE side, unsigned int charNo);
    unsigned int GetUserRankingInPower(ENUM_POWER_SIDE_TYPE side, unsigned int charNo);
    void SetPowerWarRewardInfo(int a, int b, int c, int d);
    unsigned int GetGuildRankingInPower(ENUM_POWER_SIDE_TYPE side, unsigned int guildKey);
    unsigned short GetPowerWarEndKillPoint();
    void SendPowerWarProcessInfo(unsigned int charNo);
    void SetPowerWarEndKillPoint(unsigned short point);
    void RewardGuildPowerWarPoint();
    void SaveDBPowerWarBonusPoint();
    void SaveDBPowerWarPointReward();
    void SendPowerWarEndInfoToSpecificUser(CUser* user, unsigned int b, unsigned char c,
                                           unsigned int d, unsigned int e, unsigned int f,
                                           unsigned int g, unsigned int h);
    void SendPowerWarEndInfoInSpecificPower(char side);
    unsigned int m_field4;       // +4（ORIG C1 置 0）
    CPower m_power[3];           // +8（0x6c × 3 → +8..+0x14c）
    CPowerWar m_powerWar;        // +0x14c（0x38 → +0x14c..+0x184）
    char m_winnerSide;           // +0x184
    char m_pad185;
    unsigned short m_powerWarEndKillPoint;  // +0x186
    char m_sideCount;            // +0x188
    char m_pad189;
    unsigned short m_field18a;   // +0x18a
    int m_reward1;               // +0x18c
    int m_reward2;               // +0x190
    int m_reward3;               // +0x194
    int m_reward4;               // +0x198
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Channel_Power_War_Process_Info : public PacketHeader {
public:
    Packet_Channel_Power_War_Process_Info();
    unsigned int m_idByChannel;  // +0xa
    unsigned int m_charNo;       // +0xe
    int m_scoreA;                // +0x12
    int m_scoreB;                // +0x16
    unsigned int m_userPoint;    // +0x1a
    unsigned int m_guildRank;    // +0x1e
    unsigned int m_userRank;     // +0x22
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_DB_Save_Power_War_Bonus_Point : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Bonus_Point();
    int m_bonus;                 // +0xa
    STUserPoint m_points[0xfa];  // +0xe
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_DB_Save_Power_War_Guild_Rank : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Guild_Rank();
    unsigned char m_a;           // +0xa
    unsigned char m_b;           // +0xb
    unsigned int m_c;            // +0xc
    STGuildRank m_ranks[0x64];   // +0x10
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_DB_Save_Power_War_Point : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Point();
    unsigned char m_group;   // +0xa
    char m_winner;           // +0xb
    int m_scoreA;            // +0xc
    int m_scoreB;            // +0x10
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_DB_Save_Power_War_Point_Reward : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Point_Reward();
    unsigned char m_a;           // +0xa
    unsigned int m_b;            // +0xb
    STPowerWarPointInfo m_points[0xfa];  // +0xf
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_DB_Save_Power_War_Statue_Ranker : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Statue_Ranker();
    unsigned char m_group;       // +0xa（server group）
    unsigned int m_rankers[3];   // +0xb（3 × 4 = 0xc）
} __attribute__((packed));

// from GuildPackets.h
#pragma pack(push,1)
class Packet_DB_Save_Power_War_User_Rank : public PacketHeader {
public:
    Packet_DB_Save_Power_War_User_Rank();
    unsigned char m_a;           // +0xa
    unsigned char m_b;           // +0xb
    unsigned char m_c;           // +0xc
    unsigned int m_d;            // +0xd
    unsigned int m_e;            // +0x11
    STUserRank m_ranks[0xfa];    // +0x15
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Notice_Power_War_End_Time : public PacketHeader {
public:
    Packet_Notice_Power_War_End_Time();
    unsigned char m_time;    // +0xa
    int m_scoreA;            // +0xb
    int m_scoreB;            // +0xf
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Notice_Power_War_Info : public PacketHeader {
public:
    Packet_Notice_Power_War_Info();
    int m_scoreA;    // +0xa
    int m_scoreB;    // +0xe
    char m_winner;   // +0x12
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Notice_Power_War_Rank : public PacketHeader {
public:
    Packet_Notice_Power_War_Rank();
    char m_data[0xc];
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Notice_Power_war_End_Info : public PacketHeader {
public:
    Packet_Notice_Power_war_End_Info();
    unsigned int m_idByChannel;  // +0xa
    unsigned int m_b;            // +0xe
    unsigned char m_c;           // +0x12
    unsigned int m_d;            // +0x13
    unsigned int m_e;            // +0x17
    unsigned int m_f;            // +0x1b
    unsigned int m_g;            // +0x1f
    unsigned int m_h;            // +0x23
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Reply_Power_War_Score : public PacketHeader {
public:
    Packet_Reply_Power_War_Score();
    int m_scoreA;   // +0xa
    int m_scoreB;   // +0xe
};
#pragma pack(pop)

#endif
