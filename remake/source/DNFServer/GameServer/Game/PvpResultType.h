#ifndef DNF_GAME_PVP_RESULT_TYPE_H_
#define DNF_GAME_PVP_RESULT_TYPE_H_

class PvpResultType
{
public:
    PvpResultType();
    ~PvpResultType();
    void Clear();
    void Set(const PvpResultType& other);
    void Get(PvpResultType& out) const;
    void AddNewResult(const PvpResultType& other);
    int GetTotalScore() const;

    int m_totalWinCount;
    int m_totalLoseCount;
    int m_expPoint;
    int m_oldExpPoint;
    int m_field10;
    int m_pvpGrade;
    int m_avg[15];
    int m_count;
    int m_winPoint;
    int m_todayWinCount;
    int m_todayLoseCount;
    int m_todayPlayCount;
    int m_nextResetTime;
    int m_field6c;
    int m_field70;
    int m_field74;
    int m_field78;
    int m_field7c;
    int m_field80;
};

#endif
