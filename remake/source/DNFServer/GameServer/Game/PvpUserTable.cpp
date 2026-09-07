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

#include "PvPTypes.h"  // PvpUserTable 权威完整定义（本 TU 唯一实现归属）

#include "CUserCharacInfo.h"
#include "GameWorld.h"

class CUser;

// cMyTrace 权威声明见 GameTypes.h（经 GameWorld.h 链包含），本地类已删除（§7）。


// ORIG GetResult/GetLeavePenalty 以 fnstcw→mov $0xc,%ah（RC=11 截断）→fldcw→
// fistpl 转 int（0x85d51bd 等实证），即 C 的 float→int 截断转换，非 ±0.5 舍入。
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
            m_users[idx][team].user = 0;
            m_users[idx][team].flag = 0;
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

bool PvpUserTable::IsValidTeam(int team)
{
    // ORIG 0x085d4a14：`cmpl $0; js 岛; cmpl $7; jg 岛; mov $1; jmp; mov $0`
    // （返回类型按 caller 0x085d5081 的 xor$1+testb 形定为 bool，见 PvPTypes.h）
    return (team >= 0 && team <= 7);
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
        cMyTrace tr(__PRETTY_FUNCTION__, 0x162, 5);
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
            cMyTrace tr(__PRETTY_FUNCTION__, 0x1a0, 5);
            tr("\xea\xb2\xb0\xed\x88\xac\xeb\xa1\x9c\xec\xa7\x81"
               " \xec\x98\xa4\xeb\xa5\x98"
               " PvpUserTable::GetResult Invalid Team<%d>", team);
            return 0;
        }
        int winSide = (team == 1) ? 2 : 1;
        // 0x085d511d..0x085d512a：`movzbl 0x344(%edx,%eax,1),%eax; xor $0x1,%eax;
        // test %al,%al; je` ⇒ 原码是"值即 bool 的取反"（技巧 1/19），
        // 写 `!= 1` 时 4.4 出 `cmp $0x1,%al`。
        if (m_winTeam == team && !m_dan[winSide] &&
            m_teamVsTeam[m_winTeam][team] != 0.0f)
        {
            return 0;
        }
    }

    // 常数经 .rodata 实测核对（objdump -s -j .rodata 0x08cc27d0 段）：
    //   0x08cc27d8 = 1000.0f、0x08cc27dc = 40.0f、0x08cc27e0 = 100.0f、
    //   0x08cc27e4 = 120.0f ⇒ 四分支乘除常数与此前一致（复核通过）。
    // 关键：**不能有 f / v 中间局部**。ORIG 每个边界测试、trace 实参与 return
    // 各自完整重算一遍 x87 链（0x085d5433/0x085d54c4/0x085d550b/0x085d55b6 段
    // 各出现 4 次 flds+shl+fmulp+fldcw+fistpl），指令数差即本函数 gap 主体；
    // 一旦引入具名局部，4.4 会复用 ⇒ 少 ~110 条（技巧 36/37 的浮点推广）。
    if (G_GameWorld()->IsCharacterPvPExpRevisionChannel())
    {
        if (m_winTeam == team)
        {
            // 0x085d5410 区（line 0x1d0）：下标 [m_winTeam][team]
            if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f) < -100000 ||
                100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f))
            {
                cMyTrace tr(__PRETTY_FUNCTION__, 0x1d0, 5);
                tr("pvp_exp_error3 %d, %f",
                   (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f),
                   (double)m_teamVsTeam[m_winTeam][team]);
            }
            return (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f);
        }

        if (mode >= 0xf)
        {
            // 0x085d52f1 区（line 0x1c4）：cmpl $0xe,0x14; jg → mode > 14 走本支
            if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f) < -100000 ||
                100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f))
            {
                cMyTrace tr(__PRETTY_FUNCTION__, 0x1c4, 5);
                tr("pvp_exp_error2 %d, %f",
                   (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f),
                   (double)m_teamVsTeam[m_winTeam][team]);
            }
            return (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f);
        }

        // 0x085d5182 区（line 0x1b9）：mode < 0xf ⇒ 下标 [team][team]
        // （0x085d5195 `shl $0x3,%ecx; lea (%ecx,%edx,1)` ecx=edx=team ⇒ team*9*4=team*0x24）
        if ((int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f) < -100000 ||
            100000 < (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f))
        {
            cMyTrace tr(__PRETTY_FUNCTION__, 0x1b9, 5);
            tr("pvp_exp_error1 %d, %f",
               (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f),
               (double)m_teamVsTeam[team][team]);
        }
        return (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f);
    }

    // 0x085d55f7 区（line 0x1dd）：非 revision 频道 ⇒ 仅 ×1000
    if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f) < -100000 ||
        100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f))
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1dd, 5);
        tr("pvp_exp_error4 %d, %f",
           (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f),
           (double)m_teamVsTeam[m_winTeam][team]);
    }
    return (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f);
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
    if (grade <= 9)        // ORIG: false-branch(return 0) 作 fall-through，grade-9 作跳岛
        return 0;
    return grade - 9;
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
