#ifndef DNF_GAME_POWER_MANAGER_H_
#define DNF_GAME_POWER_MANAGER_H_

class CUser;
struct SIG_LOAD_POWER_WAR_STATUE_INFO;
enum ENUM_POWER_SIDE_TYPE { ENUM_POWER_SIDE_TYPE_DUMMY = 0 };

// =====================================================================
// CPowerWarLog —— Power War 统计日志 POD（ORIG 全局类 _ZN12CPowerWarLog…）。
// Reset ORIG T 0x08480232：逐一清 int@+0x00..+0x38（sizeof=0x3c）。
// 各 Inc* W 内联自增偏移实证：见字段注释；定义由 W6_AssaultMgrDeps.cpp 转写。
// =====================================================================
class CPowerWarLog
{
public:
    CPowerWarLog();                       // ORIG T 0x0848021e
    void Reset();                         // ORIG T 0x08480232

    void IncAssaultAttemptCount();        // ORIG W 0x82f082a（++m_attemptCount）
    void IncAssaultAttemptFailureCount(); // ORIG W 0x82f0850（++m_failureCount）
    void IncAssaultAttemptSuccessCount(); // ORIG W 0x82f083c（++m_successCount）
    void IncLoadingTimeOutCountSecede();  // ORIG W 0x82f0864（+++0x10）
    void IncP2PTimeOutCountSecede();      // ORIG W 0x82f0878（+++0x18）
    void IncP2PBadCountFailure();         // ORIG W 0x82f088c（+++0x1c）
    void IncP2PBadCountSecede();          // ORIG W 0x82f08a0（+++0x20）
    void IncStartSuccessCount();          // ORIG W 0x82f08c8（+++0x24）
    void IncStartFailureCount();          // ORIG W 0x82f08b4（+++0x28）
    void IncTotalJoinAssaultUser(int n);  // ORIG W 0x82f08dc（m_totalJoinAssaultUser += n）
    void IncTotalLeaveAssaultUser();      // ORIG W 0x82f08f2（+++0x38）
    void PrintPowerWarAssultReport();     // ORIG T 0x084802cc

    int m_attemptCount;             // +0x00
    int m_successCount;             // +0x04
    int m_failureCount;             // +0x08
    int m_field0c;                  // +0x0c（Reset 清零，无独立 Inc 符号）
    int m_loadingTimeoutSecede;     // +0x10
    int m_field14;                  // +0x14（Reset 清零）
    int m_p2pTimeoutSecede;         // +0x18
    int m_p2pBadFailure;            // +0x1c
    int m_p2pBadSecede;             // +0x20
    int m_startSuccess;             // +0x24
    int m_startFailure;             // +0x28
    int m_field2c[2];               // +0x2c..+0x30（Reset 清零）
    int m_totalJoinAssaultUser;     // +0x34
    int m_totalLeaveAssaultUser;    // +0x38
};                                  // sizeof == 0x3c

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
    // PowerUp 定时器四件套：ORIG 均为 inline 弱符号，+0x1c/+0x20 即下方
    // m_field1c/m_field20（GetPowerUpRemainTime 读 +0x1c、GetPowerUpTimerPeriod
    // 读 +0x20、两个 setter 分别写 +0x1c/+0x20，见 ORIG 0x863bdac/0x863bdb8/
    // 0x826888be/0x826888cc）。
    int GetPowerUpRemainTime() { return m_field1c; }
    void SetPowerUpRemainTime(int remainSec) { m_field1c = remainSec; }
    int GetPowerUpTimerPeriod() { return m_field20; }
    void SetPowerUpTimerPeriod(int period) { m_field20 = period; }

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
    CPowerWarLog m_powerWarLog;         // +0x1b4（sizeof 0x3c；ORIG 嵌入对象，GetPowerWarLog 返回此址）
    char m_pad1f0[0x18];                // +0x1f0..+0x207（原 pad 尾段；CPowerWarPacketLog 区待建模）


public:
    CPowerWarLog* GetPowerWarLog();     // ORIG W 0x82f0906：返回 this+0x1b4 cast
};



#endif
