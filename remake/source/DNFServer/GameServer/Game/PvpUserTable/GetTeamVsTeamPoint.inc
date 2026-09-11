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
    // [P2S1PvPQuest 2026-09-11 P339 命中] ORIG 的 `dbl` 槽在**最深端**
    //   （0x085d4eee `fstpl -0x48(%ebp)`，全函数最深局部）：声明前移到函数头后
    //   残差块 26→**7**（帧 0x70 不变、其余槽已全对齐）；若声明在 if 之前
    //   （0x085d4eeX 之后）实测块数反升到 17，if 块内声明则全槽表深 8（26 块）
    //   ⇒ 函数头是本 TU 下唯一与 ORIG 相合的声明位置。赋值语句仍在 if 体内。
    //   残余 7 块（N82·不可达已封顶）：ORIG `dbl`-0x48 / `tr`-0x38 vs 我方
    //   `dbl`-0x30 / `tr`-0x40 = 两者**相对深度互换**（各差 0x10）+ 帧
    //   0x60 vs 0x70（0x10B）。三种声明位置已扫（26/17/**7** 块，单调改善），
    //   声明序方向"越早越深"已到函数头尽头 ⇒ 余量为 gcc 内部 slot 分配。
    double dbl;
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
    // [P2S1PvPQuest 2026-09-11] 判据库「bool 化」旋钮：ORIG 0x085d4edc 起
    //   `setne %al; setp %dl; or %edx,%eax; xor $0x1,%eax; test %al,%al; je`
    //   = 比较物化为 bool（`!(a != b)` 形），而非 `jne/jp` 直接分支；
    //   /tmp/p2s1/zb.cc 六形探针（4.4.7 -m32 -O0 -march=i486）只有
    //   `!(ratioSum != 0.0f)` 逐条命中该形（f2 的 sete/setnp/and 不同）。
    //   IEEE 语义等价（NaN: `==` 假、`!(NaN!=0)` 亦假）。
    if (!(ratioSum != 0.0f))
    {
        dbl = ratioSum;
        // [G5 2026-09-10 P227 试验·无效已回滚] ORIG 0x085d4eee 的 `fstpl -0x48`（dbl 具名
        //   double 落槽）在 cMyTrace ctor 之前，理论上为全表达式临时链；改链式形实测
        //   四数不变、槽表逐值不变（我方 -0x10..-0x4c vs ORIG -0xc..-0x48）⇒ 回滚。
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