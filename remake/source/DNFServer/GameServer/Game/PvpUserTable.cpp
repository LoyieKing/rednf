// ============================================================================
// df_game_r PvpUserTable（G2-5 PvP 队伍表，0x34c 字节）
// 逐函数对照 docs/class_func_reports/PvpUserTable.md（汇编 + Ghidra C）与
// ORIG 反汇编实现；GetWinTeam/GetUserVsTeamPoint 为 ORIG 弱符号，直接读
// 0x85df7a4 / 0x85dfddc 反汇编实现。
// 布局（自 ORIG 构造/析构 + 各函数寻址推导）：
//   +0x000 float m_teamVsTeam[8][8]  队伍间分差矩阵（0x100）
//   +0x100 float m_teamPoint[8]      队伍期望分累计（0x20）
//   +0x120 m_users[8][8]（CUser*+uchar，8 字节/项，0x200）
//   +0x320 int m_winTeam
//   +0x324 float m_minPoint[8]      各队伍最小分（0x20，GetLeavePenalty 用）
//   +0x344 uchar m_dan[8]           丹位标记
// 大小合计 0x34c。
// ============================================================================

#include <algorithm>
#include <cstring>

#include "CUserCharacInfo.h"
#include "GameWorld.h"

class CUser;

// cMyTrace 权威声明见 GameTypes.h（经 GameWorld.h 链包含），本地类已删除（§7）。

// ---- PvpUserTable（0x34c） ----
class PvpUserTable
{
public:
    PvpUserTable();
    ~PvpUserTable();
    void Clear();
    int  Set(int team, CUser* user);
    int  IsValidTeam(int team);
    int  IsValidConvertedGrade(int grade);
    int  GetTeamUserCount(int team);
    float GetExpectVal(int grade1, int grade2, CUser* user);
    int  Calculate();
    int  GetTeamVsTeamPoint(int team1, int team2);
    int  GetGhostVsTeamPoint(int team, int unused);
    int  GetResult(int team, bool isWinTeam, int mode);
    int  GetLeavePenalty(int team);
    int  GradeFilter(int grade);
    void CheckDan();
    void SetWinTeam(int team);
    int  GetWinTeam();
    int  GetUserVsTeamPoint(int userIdx, int myTeam, int enemyTeam,
                            CUser* user);

    struct UserSlot
    {
        CUser* user;        // +0x00
        unsigned char flag; // +0x04
    };

    float m_teamVsTeam[8][8]; // +0x000
    float m_teamPoint[8];     // +0x100
    UserSlot m_users[8][8];   // +0x120
    int m_winTeam;            // +0x320
    float m_minPoint[8];      // +0x324
    unsigned char m_dan[8];   // +0x344
};

PvpUserTable::PvpUserTable()
{
    Clear();
}

PvpUserTable::~PvpUserTable()
{
}

void PvpUserTable::Clear()
{
    for (int team = 0; team < 8; ++team)
    {
        for (int idx = 0; idx < 8; ++idx)
        {
            m_users[team][idx].user = 0;
            m_users[team][idx].flag = 0;
        }
    }
    m_winTeam = -1;
}

int PvpUserTable::Set(int team, CUser* user)
{
    if (!IsValidTeam(team) || user == 0)
    {
        return 0;
    }
    int count = GetTeamUserCount(team);
    if (count >= 8)
    {
        return 0;
    }
    m_users[team][count].user = user;
    m_users[team][count].flag = 0;
    return 1;
}

int PvpUserTable::IsValidTeam(int team)
{
    return (team >= 0 && team <= 7) ? 1 : 0;
}

int PvpUserTable::IsValidConvertedGrade(int grade)
{
    return (grade >= 0 && grade <= 0x14) ? 1 : 0;
}

int PvpUserTable::GetTeamUserCount(int team)
{
    if (!IsValidTeam(team))
    {
        return -1;
    }
    int count = 0;
    for (int i = 0; i < 8; ++i)
    {
        if (m_users[team][i].user != 0)
        {
            ++count;
        }
    }
    return count;
}

float PvpUserTable::GetExpectVal(int grade1, int grade2, CUser* user)
{
    int saved1 = grade1;
    int saved2 = grade2;
    grade1 = GradeFilter(grade1);
    grade2 = GradeFilter(grade2);

    bool changed = false;
    if ((grade1 == 1 && grade2 == 0) || (grade1 == 0 && grade2 == 1))
    {
        // 用户存在且段位 <= 9 时保留换算结果，否则回退原段位
        if (!(user != 0 && ((CUserCharacInfo*)user)->get_pvp_grade() <= 9))
        {
            grade1 = saved1;
            grade2 = saved2;
            changed = true;
        }
    }

    if (!IsValidConvertedGrade(grade1) || !IsValidConvertedGrade(grade2))
    {
        return -1.0f;
    }
    if (grade1 != grade2 && !changed)
    {
        if (grade1 == 0)
        {
            return 0.0f;
        }
        if (grade2 == 0)
        {
            return 1.0f;
        }
    }
    return 0.024f * (float)(grade1 - grade2) + 0.5f;
}

int PvpUserTable::Calculate()
{
    memset(this, 0, 0x100);
    memset(m_teamPoint, 0, sizeof(m_teamPoint));
    CheckDan();

    for (int i = 0; i < 8; ++i)
    {
        SetWinTeam(i);
        for (int j = 0; j < 8; ++j)
        {
            if (i != j)
            {
                GetTeamVsTeamPoint(i, j);
            }
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        m_minPoint[i] = 0.0f;
        for (int j = 0; j < 8; ++j)
        {
            m_minPoint[i] = std::min(m_teamVsTeam[j][i], m_minPoint[i]);
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            m_users[i][j].user = 0;
        }
    }
    return 1;
}

int PvpUserTable::GetTeamVsTeamPoint(int team1, int team2)
{
    int count1 = GetTeamUserCount(team1);
    int count2 = GetTeamUserCount(team2);
    if (count1 == 0 || count2 == 0)
    {
        return 0;
    }
    if (m_users[team1][0].flag != 1)
    {
        return 0;
    }

    m_teamPoint[team1] = 0.0f;
    m_teamPoint[team2] = 0.0f;

    for (int i = 0; i < 8; ++i)
    {
        CUser* u = m_users[team1][i].user;
        GetUserVsTeamPoint(i, team1, team2, u);
        GetUserVsTeamPoint(i, team2, team1, u);
    }

    if (count2 < count1)
    {
        int diff = count1 - count2;
        for (int k = 0; k < diff; ++k)
        {
            GetGhostVsTeamPoint(team1, team2);
        }
    }

    float ratioSum = m_teamPoint[team1] + m_teamPoint[team2];
    if (ratioSum == 0.0f)
    {
        cMyTrace tr("bool PvpUserTable::GetTeamVsTeamPoint(int, int)", 0x162, 5);
        tr("[FPE][PvpUserTable::GetTeamVsTeamPoint] RatioSum: %f",
           (double)ratioSum);
        return 0;
    }

    float ratio = (m_teamPoint[team2] / ratioSum) * (float)(count1 + count2);
    m_teamVsTeam[team1][team1] += ratio / (float)count1;
    m_teamVsTeam[team1][team2] -= ratio / (float)count2;
    return 1;
}

int PvpUserTable::GetGhostVsTeamPoint(int team, int)
{
    for (int i = 0; i < 8; ++i)
    {
        CUser* u = m_users[team][i].user;
        if (u != 0)
        {
            m_teamPoint[team] += GetExpectVal(((CUserCharacInfo*)u)->get_pvp_grade(),
                                              0, 0);
        }
    }
    return 1;
}

int PvpUserTable::GetResult(int team, bool isWinTeam, int mode)
{
    if (!IsValidTeam(team))
    {
        return -1;
    }

    if (isWinTeam)
    {
        if (team != 2 && team != 1)
        {
            cMyTrace tr("int PvpUserTable::GetResult(int, bool, int)", 0x1a0, 5);
            tr("\xea\xb2\xb0\xed\x88\xac\xeb\xa1\x9c\xec\xa7\x81"
               " \xec\x98\xa4\xeb\xa5\x98"
               " PvpUserTable::GetResult Invalid Team<%d>", team);
            return 0;
        }
        int winSide = (team == 1) ? 2 : 1;
        if (m_winTeam == team && m_dan[winSide] != 1 &&
            m_teamVsTeam[m_winTeam][team] != 0.0f)
        {
            return 0;
        }
    }

    if (G_GameWorld()->IsCharacterPvPExpRevisionChannel())
    {
        if (m_winTeam == team)
        {
            float f = m_teamVsTeam[m_winTeam][team];
            int v = (int)(f * 1000.0f * 120.0f / 100.0f);
            if (v < -100000 || 100000 < v)
            {
                cMyTrace tr("int PvpUserTable::GetResult(int, bool, int)",
                            0x1d0, 5);
                tr("pvp_exp_error3 %d, %f", (int)(f * 1000.0f * 120.0f / 100.0f),
                   (double)f);
            }
            return (int)(f * 1000.0f * 120.0f / 100.0f);
        }

        if (mode > 14)
        {
            float f = m_teamVsTeam[m_winTeam][team];
            int v = (int)(f * 1000.0f * 40.0f / 100.0f);
            if (v < -100000 || 100000 < v)
            {
                cMyTrace tr("int PvpUserTable::GetResult(int, bool, int)",
                            0x1c4, 5);
                tr("pvp_exp_error2 %d, %f", (int)(f * 1000.0f * 40.0f / 100.0f),
                   (double)f);
            }
            return (int)(f * 1000.0f * 40.0f / 100.0f);
        }

        float f = m_teamVsTeam[team][team];
        int v = (int)(f * 1000.0f * 40.0f / 100.0f);
        if (v < -100000 || 100000 < v)
        {
            cMyTrace tr("int PvpUserTable::GetResult(int, bool, int)", 0x1b9, 5);
            tr("pvp_exp_error1 %d, %f", (int)(f * 1000.0f * 40.0f / 100.0f),
               (double)f);
        }
        return (int)(f * 1000.0f * 40.0f / 100.0f);
    }

    float f = m_teamVsTeam[m_winTeam][team];
    int v = (int)(f * 1000.0f);
    if (v < -100000 || 100000 < v)
    {
        cMyTrace tr("int PvpUserTable::GetResult(int, bool, int)", 0x1dd, 5);
        tr("pvp_exp_error4 %d, %f", (int)(f * 1000.0f), (double)f);
    }
    return (int)(f * 1000.0f);
}

int PvpUserTable::GetLeavePenalty(int team)
{
    if (!IsValidTeam(team))
    {
        return -1;
    }
    return (int)(m_minPoint[team] * 1000.0f);
}

int PvpUserTable::GradeFilter(int grade)
{
    if (grade < 0 || grade > 0x22)
    {
        return -1;
    }
    if (grade > 0x14)
    {
        return 0x14;
    }
    if (grade > 9)
    {
        return grade - 9;
    }
    return 0;
}

void PvpUserTable::CheckDan()
{
    memset(m_dan, 0, sizeof(m_dan));
    for (int team = 0; team < 8; ++team)
    {
        for (int idx = 0; idx < 8; ++idx)
        {
            CUser* u = m_users[team][idx].user;
            if (u != 0 && ((CUserCharacInfo*)u)->get_pvp_grade() > 9)
            {
                m_dan[team] = 1;
                break;
            }
        }
    }
}

void PvpUserTable::SetWinTeam(int team)
{
    m_winTeam = team;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            m_users[i][j].flag = (i == team) ? 1 : 0;
        }
    }
}

int PvpUserTable::GetWinTeam()
{
    return m_winTeam;
}

int PvpUserTable::GetUserVsTeamPoint(int userIdx, int myTeam, int enemyTeam,
                                     CUser* user)
{
    CUser* me = m_users[myTeam][userIdx].user;
    if (me == 0)
    {
        return 0;
    }
    for (int i = 0; i < 8; ++i)
    {
        CUser* other = m_users[enemyTeam][i].user;
        if (other != 0)
        {
            m_teamPoint[myTeam] +=
                GetExpectVal(((CUserCharacInfo*)me)->get_pvp_grade(),
                             ((CUserCharacInfo*)other)->get_pvp_grade(), user);
        }
    }
    return 1;
}
