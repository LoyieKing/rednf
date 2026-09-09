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
//   +0x344 bool m_dan[8]            丹位标记（b28d-solo：P248 档② bool 化）
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

// [b28d-54 N] ORIG 0x085d4980：帧 0x24→0x28 + `push %ebx` 差（ORIG ebx 缓存
//   成员基址）——4.4 寄存器分配（N7/N82）。语义等价。
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
    // b28d-solo-PvpUserTable：ORIG 0x085d49df..0x085d4a04 先把 8 字节局部写满
    //   （-0x14 user / -0x10 flag）再整体拷贝到 m_users[team][count]
    //   （`mov %eax,(%ecx,%ebx,8); mov %edx,0x4(%ecx,%ebx,8)`，ebx 缓存元素基址，
    //   函数头 push %ebx）⇒ 局部 UserSlot + 结构整体赋值（P247；逐字段赋值时
    //   4.4 每条重发完整 team*8+count 寻址链）。
    UserSlot slot;
    slot.user = user;
    slot.flag = 0;
    m_users[team][count] = slot;
    return 1;
}

bool PvpUserTable::IsValidTeam(int team)
{
    // ORIG 0x085d4a14：`cmpl $0; js 岛; cmpl $7; jg 岛; mov $1; jmp; mov $0`
    // （返回类型按 caller 0x085d5081 的 xor$1+testb 形定为 bool，见 PvPTypes.h）
    return (team >= 0 && team <= 7);
}

// b28d-54 判词：ORIG 0x085d4a32（GetExpectVal 两处调用点 0x085d4b4a/0x085d4b5f 均
//   `call; xor $1,%eax; test %al,%al; je`）⇒ 返回 bool（P248 档①）。
bool PvpUserTable::IsValidConvertedGrade(int grade)
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

// b28d-solo-PvpUserTable：ORIG 0x085d4ab8，AE。翻正：空 then 的
//   `if (user != 0 && grade <= 9) {} else {回退}` 使 4.4 物化成功值
//   （`mov $1/jmp/mov $0/test %al,%al/jne`，P272 德摩根块序）；此前
//   `!(A&&B)` 与 `A'||B'` 两形恒物化失败值（jle/je 反极性）。
float PvpUserTable::GetExpectVal(int grade1, int grade2, CUser* user)
{
    int saved1 = grade1;
    int saved2 = grade2;
    grade1 = GradeFilter(grade1);
    grade2 = GradeFilter(grade2);

    bool changed = false;
    if ((grade1 == 1 && grade2 == 0) || (grade1 == 0 && grade2 == 1))
    {
        // b28d-solo-PvpUserTable：ORIG 0x085d4b2d `cmpl $0,0x14(%ebp); je 岛` +
        //   `cmp $9,%eax; jg 岛`（两条失败路径同落 `mov $0` 块）+ `mov $1/jmp/
        //   mov $0/test %al,%al/jne`（成功才跳过回退体）⇒ 4.1 物化的是
        //   `user != 0 && grade <= 9`（正向合取，P272 的德摩根块序）。
        if (user != 0 && ((CUserCharacInfo*)user)->get_pvp_grade() <= 9)
        {
        }
        else
        {
            grade1 = saved1;
            grade2 = saved2;
            changed = true;
        }
    }

    // b28d-54 翻正：ORIG 0x085d4b4a/0x085d4b5f 两个独立守卫各自
    //   `call; xor $1,%eax; test %al,%al; je <独立 return -1 块>`（P133：
    //   合并 `||` 会生 bool 汇聚 setne 物化）。
    if (!IsValidConvertedGrade(grade1))
        return -1.0f;
    if (!IsValidConvertedGrade(grade2))
        return -1.0f;
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

// b28d-solo-PvpUserTable：ORIG 0x085d4bd0。三处翻正——
//   ① 0x085d4bd7 `mov 0x8(%ebp),%eax` 先于两个常量压栈 ⇒ P68 `(char*)this`；
//   ② 0x085d4caa 段先算第二实参（&m_teamVsTeam[j][i]）再算第一实参
//      （&m_minPoint[i]，`lea 0x4(%edx)`）⇒ std::min 实参序为
//      (m_minPoint[i], m_teamVsTeam[j][i])；
//   ③ 0x085d4c64 空 then 占位岛（`je +0x94(nop)` + body 尾 `jmp +0x95`）⇒
//      P223 正条件 goto 尾空标签形。
int PvpUserTable::Calculate()
{
    memset((char*)this, 0, 0x100);
    memset(m_teamPoint, 0, sizeof(m_teamPoint));
    CheckDan();

    for (int i = 0; i < 8; ++i)
    {
        SetWinTeam(i);
        for (int j = 0; j < 8; ++j)
        {
            if (i == j)
                goto skip_vs;
            GetTeamVsTeamPoint(i, j);
        skip_vs:;
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        m_minPoint[i] = 0.0f;
        for (int j = 0; j < 8; ++j)
        {
            m_minPoint[i] = std::min(m_minPoint[i], m_teamVsTeam[j][i]);
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

// b28d-solo-PvpUserTable：ORIG 0x085d4d5e，AE（其余件）⇒ 本件 byte 层 N 封顶。
//   翻正——
//   ① 0x085d4db6 `movzbl flag; xor $1; test %al; je` ⇒ `!flag`（字段已 bool 化）；
//   ② 0x085d4e6b `mov -0x28,%eax; cmp -0x24,%eax; jle`（count1 vs count2）⇒
//      条件写 `count1 > count2`（写 count2<count1 时 4.4 载 count2 反序）；
//   ③ 0x085d4f38 ratio 段先 `fstps -0x1c` 存局部再 `flds -0x1c` 回读 ⇒ 两条语句；
//   ④ 零分支 0x085d4eee `fstpl -0x48` 在 ctor 前 ⇒ 具名 double 局部（N80 求值序）。
//   残余：①0x085d4ed2 fucompp/fnstsw/sahf vs 4.4 fucomip/fstp（N85 x87 比较
//   lowering，-march=i486 已生效仍不可翻）；②槽号整体深 8/帧 0x70 vs 0x60
//   （N82；4.4 把 double 临时放浅槽 -0x10 vs ORIG -0x48）。指令 191 vs 194
//   （差即 N85 段）、p167 CLEAR 7/7 ⇒ 语义完成。
bool PvpUserTable::GetTeamVsTeamPoint(int team1, int team2)
{
    int count1 = GetTeamUserCount(team1);
    int count2 = GetTeamUserCount(team2);
    if (count1 == 0 || count2 == 0)
    {
        return 0;
    }
    if (!m_users[team1][0].flag)
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

    if (count1 > count2)
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
        double dbl = ratioSum;
        cMyTrace tr(__PRETTY_FUNCTION__, 0x162, 5);
        tr("[FPE][PvpUserTable::GetTeamVsTeamPoint] RatioSum: %f",
           dbl);
        return 0;
    }

    float ratio = m_teamPoint[team2] / ratioSum;
    ratio *= (float)(count1 + count2);
    m_teamVsTeam[team1][team1] += ratio / (float)count1;
    m_teamVsTeam[team1][team2] -= ratio / (float)count2;
    return 1;
}

int PvpUserTable::GetGhostVsTeamPoint(int team, int)
{
    // ORIG 0x085d4fd1/0x085d4fd8 两条 `movl $0,-0x10/-0xc` 初始化 ⇒ 源码 u 先
    // 声明（槽 -0x10，初始化为 0）再 for-init i（槽 -0xc）；把 u 放循环体内
    // 会让 i 占 -0x10、u 占 -0xc（槽序反，P28/N93 标量可翻）。
    CUser* u = 0;
    for (int i = 0; i < 8; ++i)
    {
        u = m_users[team][i].user;
        // ORIG 0x085d5047 体尾 `jmp +0x80; nop` 空 then 占位岛 ⇒ P223 goto 尾
        // 空标签形（4.4 对 if(cond){body} 恒 jump-thread 掉 jmp+nop）。
        if (u == 0)
            goto next;
        m_teamPoint[team] += GetExpectVal(((CUserCharacInfo*)u)->get_pvp_grade(),
                                          0, 0);
    next:;
    }
    return 1;
}

// b28d-solo-PvpUserTable：ORIG 0x085d5064，byte 层 N85 封顶。翻正——
//   ① m_dan bool 化（P248 档②，0x085d511d xor 形）；
//   ② 四处 trace 改全表达式临时链（P227：double 实参 ctor 前 `flds;fstpl`、
//      int 结果 `mov -0x74,%ebx` 跨 ctor 保活）；
//   ③ revision 分支物理序改 A(mode<=14)→B(mode>14)→C(m_winTeam==team)
//      （P222/P47；4.4 对独立 if+return 恒 then 放前）；
//   ④ 0x1dd trace 的 %d 实参修正为 `(int)m_teamVsTeam[...]`（ORIG
//      0x085d569e 无 ×1000 乘法，真语义差异）。
//   残余：0x085d5148 fucompp/fnstsw/sahf vs 4.4 fucomip/fstp（N85）；
//   467=467 条、帧 0xb4+push %ebx 全等、p167 CLEAR 13/13 ⇒ 语义完成。
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
        // b28d-solo-PvpUserTable：ORIG 分支物理序 = A(mode<=14, [team][team],
        //   +0x128) → B(mode>14, [m_winTeam][team], +0x28d) → C(m_winTeam==team,
        //   ×120, +0x410)。4.4 对「独立 if + return」恒把 then 放前 ⇒ 写成
        //   `if (m_winTeam != team) { if (mode <= 0xe) A else B } else C`
        //   （P222/P47 分支序翻正）。
        if (m_winTeam != team)
        {
            if (mode <= 0xe)
            {
                // A：0x085d5182 区（line 0x1b9），下标 [team][team]
                // （0x085d5195 `shl $0x3,%ecx; lea (%ecx,%edx,1)` ecx=edx=team）
                if ((int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f) < -100000 ||
                    100000 < (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f))
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x1b9, 5)("pvp_exp_error1 %d, %f",
                       (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f),
                       (double)m_teamVsTeam[team][team]);
                }
                return (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f);
            }
            else
            {
                // B：0x085d52f1 区（line 0x1c4），mode > 14
                if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f) < -100000 ||
                    100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f))
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x1c4, 5)("pvp_exp_error2 %d, %f",
                       (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f),
                       (double)m_teamVsTeam[m_winTeam][team]);
                }
                return (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f);
            }
        }
        else
        {
            // C：0x085d5410 区（line 0x1d0），下标 [m_winTeam][team]
            // ORIG 0x085d5217 段先 `flds; fstpl -0x98` 求值 double 实参、再
            // 重算 int 并 `mov -0x74,%ebx` 缓存，全部位于 ctor call 之前
            // （P227 全表达式临时链形；具名 tr 对象 4.4 恒 ctor 后求值实参）。
            if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f) < -100000 ||
                100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x1d0, 5)("pvp_exp_error3 %d, %f",
                   (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f),
                   (double)m_teamVsTeam[m_winTeam][team]);
            }
            return (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f);
        }
    }

    // 0x085d55f7 区（line 0x1dd）：非 revision 频道 ⇒ 仅 ×1000。
    // b28d-solo-PvpUserTable：ORIG trace 的 %d 实参为 0x085d569e
    //   `flds (%eax,%edx,4); fldcw -0x70; fistpl -0x74`（**无 ×1000 乘法**）
    //   ⇒ 原码写 `(int)m_teamVsTeam[m_winTeam][team]`（此前多乘 1000，真语义差异）。
    if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f) < -100000 ||
        100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1dd, 5)("pvp_exp_error4 %d, %f",
           (int)m_teamVsTeam[m_winTeam][team],
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
            // b28d-54 判词：ORIG 0x085d57c2（CheckDan）。
            // ORIG 形 `je 0x085d583d(nop)` + `je 0x085d583e`（两守卫落点相差 1 字节
            // nop 岛）⇒ 空 then 占位岛，4.4 对 if/else-if 恒 jump-thread 合并两出口。
            // P223：否定 goto + 尾空标签串复现（标签序=守卫序）。
            if (u == 0)
            {
                goto check_dan_then_end;
            }
            if (((CUserCharacInfo*)u)->get_pvp_grade() > 9)
            {
                m_dan[team] = 1;
                break;
            }
        check_dan_then_end:
            ;
        }
    }
}

// b28d-solo-PvpUserTable：ORIG 0x085d585e 两分支各自 `movb $0x1/$0x0` 直写
//   （then 落 0x085d589c、else 落 0x085d58b5，无 al 物化、无 push %ebx、
//   循环槽 -0x8/-0x4）⇒ 三目写形在 4.4 恒物化 `mov $1/jmp/mov $0` 并缓存 i 到
//   %ebx（P171 反向：ORIG 不物化就必须写成两分支独立赋值）。
void PvpUserTable::SetWinTeam(int team)
{
    m_winTeam = team;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (i == team)
                m_users[i][j].flag = 1;
            else
                m_users[i][j].flag = 0;
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
        // ORIG 0x085dfe80 体尾 `jmp +0xaa; nop` ⇒ P223 goto 尾空标签形。
        if (other == 0)
            goto next;
        m_teamPoint[myTeam] +=
            GetExpectVal(((CUserCharacInfo*)me)->get_pvp_grade(),
                         ((CUserCharacInfo*)other)->get_pvp_grade(), user);
    next:;
    }
    return 1;
}
