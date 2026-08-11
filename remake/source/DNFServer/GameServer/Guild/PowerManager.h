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
    unsigned char IsPowerWarOn();
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
    char m_rest[0x19c - 0x18c];  // +0x18c
};

// from GuildPackets.h
class Packet_Channel_Power_War_Process_Info : public PacketHeader {
public:
    Packet_Channel_Power_War_Process_Info();
    char m_data[0x1c];
};

// from GuildPackets.h
class Packet_DB_Save_Power_War_Bonus_Point : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Bonus_Point();
    char m_data[0x7d4];
};

// from GuildPackets.h
class Packet_DB_Save_Power_War_Guild_Rank : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Guild_Rank();
    char m_data[0x326];
};

// from GuildPackets.h
class Packet_DB_Save_Power_War_Point : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Point();
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_DB_Save_Power_War_Point_Reward : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Point_Reward();
    char m_data[0x7d5];
};

// from GuildPackets.h
class Packet_DB_Save_Power_War_Statue_Ranker : public PacketHeader {
public:
    Packet_DB_Save_Power_War_Statue_Ranker();
    char m_data[0xd];
};

// from GuildPackets.h
class Packet_DB_Save_Power_War_User_Rank : public PacketHeader {
public:
    Packet_DB_Save_Power_War_User_Rank();
    char m_data[0x7db];
};

// from GuildPackets.h
class Packet_Notice_Power_War_End_Time : public PacketHeader {
public:
    Packet_Notice_Power_War_End_Time();
    char m_data[0x9];
};

// from GuildPackets.h
class Packet_Notice_Power_War_Info : public PacketHeader {
public:
    Packet_Notice_Power_War_Info();
    char m_data[0x9];
};

// from GuildPackets.h
class Packet_Notice_Power_War_Rank : public PacketHeader {
public:
    Packet_Notice_Power_War_Rank();
    char m_data[0x12];
};

// from GuildPackets.h
class Packet_Notice_Power_war_End_Info : public PacketHeader {
public:
    Packet_Notice_Power_war_End_Info();
    char m_data[0x1d];
};

// from GuildPackets.h
class Packet_Reply_Power_War_Score : public PacketHeader {
public:
    Packet_Reply_Power_War_Score();
    char m_data[0xa];
};

#endif
