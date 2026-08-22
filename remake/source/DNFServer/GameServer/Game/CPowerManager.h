#ifndef DNF_GAME_POWER_MANAGER_H_
#define DNF_GAME_POWER_MANAGER_H_

class CUser;
struct SIG_LOAD_POWER_WAR_STATUE_INFO;
enum ENUM_POWER_SIDE_TYPE { ENUM_POWER_SIDE_TYPE_DUMMY = 0 };

class CPowerManager
{
public:
    CPowerManager();
    bool Init();
    bool CheckCompleteDBLoadStatueInfo();
    static void LoadPowerWarInfo();
    static void LoadRankerInfo();
    bool IsPowerWarEventOn();
    float GetPowerWarResponPenalty(CUser* user);
    int GetPowerWarGhostTime(CUser* user, float f);
    void IncreasePowerUserCount(char side);
    void DecreasePowerUserCount(char side);
    int CheckPowerWarEnterLimit(CUser* user);
    int ProcessJoinPowerWar(CUser* user, char side);
    void SendPowerWarUserStatueInfo(CUser* user);
    void SetPowerInfo(char side, int point, int channelPoint);
    void SetPowerWarStartTime(unsigned char year, unsigned char month,
                              unsigned char day, unsigned char hour);
    void SetPowerWarUserRankingInfo(unsigned int* charNos);
    void SetPowerWarUserStatueInfo(SIG_LOAD_POWER_WAR_STATUE_INFO* info);
    void ClearMVPInfo();
    bool IsAbleToRevival(CUser* user, unsigned int charNo, int& result);
    void IncreaePowerWarPointInChannel(char side, int point);
    int GetPowerUserCount(char side);
    int GetPowerWarPoint(ENUM_POWER_SIDE_TYPE side);
    int GetPowerWarPointInChannel(char side);
    int GetRankingByCharacNo(unsigned int charNo);
    char GetWinnerSide();
    bool IsWinerSide(char side);

    char m_winnerSide;          // +0x00
    char m_pad01[3];            // +0x01..+0x03
    int m_side1Point;           // +0x04（SetPowerInfo 写 +0x04）
    int m_side2Point;           // +0x08
    int m_side1ChannelPoint;    // +0x0c
    int m_side2ChannelPoint;    // +0x10
    int m_side1UserCount;       // +0x14（GetPowerUserCount: side==1 -> +0x14）
    int m_side2UserCount;       // +0x18
    int m_field1c;              // +0x1c（Init 置 0）
    int m_field20;              // +0x20（Init 置 0）
    unsigned char m_powerWarStartTime[4]; // +0x24（SetPowerWarStartTime 写 0x24..0x27）

    // STPowerWarStatueRankerInfo：雕像排行条目，3 条，+0x28 起，entry 0x84。
    // 布局依据 ORIG GetRankingByCharacNo(+0x2c 读 characNo) / CheckCompleteDBLoadStatueInfo
    // (+0x28 读 flag) / SendPowerWarUserStatueInfo(+0x30 name, +0x4e job, +0x4f level,
    // +0x50 grow, +0x51 guildName, +0x7c guildId, +0x80..equipment[11]) 推导。
    struct RankerEntry
    {
        unsigned char m_flag;           // +0x00 (1=已加载)
        char m_pad01[3];
        unsigned int m_characNo;        // +0x04
        char m_name[0x1e];              // +0x08（30B）
        char m_job;                     // +0x26
        char m_level;                   // +0x27
        char m_grow;                    // +0x28
        char m_guildName[0x28];         // +0x29（40B）
        int m_guildId;                  // +0x51（对齐到 +0x54）
        int m_equipment[11];            // +0x58（0x80 起，11 槽）
    };
    RankerEntry m_rankings[3];
    char m_pad1b4[0x54];                // +0x1b4（CPowerWarLog 0x1b4 / CPowerWarPacketLog 0x1f0 占位）

};

#endif
