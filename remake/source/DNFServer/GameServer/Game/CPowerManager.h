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

    char m_winnerSide;
    char m_pad01[3];
    int m_side1Point;
    int m_side2Point;
    int m_side1ChannelPoint;
    int m_side2ChannelPoint;
    char m_pad14[4];
    int m_side1UserCount;
    int m_side2UserCount;
    char m_pad20[8];
    struct RankingEntry { char m_pad00[4]; unsigned int m_characNo; char m_pad08[0x7c]; } m_rankings[3];
    char m_pad1b4[0x54];
};

#endif
