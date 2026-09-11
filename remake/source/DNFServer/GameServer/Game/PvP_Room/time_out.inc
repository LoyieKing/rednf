// [b28d-7 N82] ORIG 0x085d9b0e：槽 -0x48 与计数槽，4.4 -0x4c/-0x44（槽基差 4 + 帧 365/373）；P167 CLEAR ⇒ 封顶。
// [b28d-solo-PvP_Room] N82: ORIG 0x085d9b0e 槽 -0x48 与计数槽 vs 4.4 -0x4c/-0x44；P167 CLEAR
int PvP_Room::time_out(int* seats)
{
    // ORIG 0x085d9b0e（365 条；本侧 373，+8 全为 N 类形态差，台账豁免）：
    //  - local_4c/local_3d 的 ==1/!=0 检查 4.1 折成 `xor$1,%eax;test%al;je`
    //    常量岛（4.4 恒 `cmpb/cmpl` 直判，setcc-fold 负例在册）；
    //  - PVP_*_BATTLE_TIME[local_4c] 与 local_84[teamwin+5] 索引 4.1 单段
    //    `mov &arr(,%eax,4)`/`lea …,%edx` 直落，4.4 拆 lea+reload（N76/N18a）；
    //  - end_time 日志 3 实参 4.1 以 callee-saved 缓存（push 对），4.4 逐槽
    //    reload（+4 mov）。lock()/unlock() 调用点、模式分派、do-while 比较
    //    （jl/jle 方向）已逐段核对无语义缺口。
    // [b28d-t3-PoolBigThree] **148→84**（本轮起点 148=官方 s408 现值；台账 330 为
    // b28d 期快照）三修：①模式分派 if 链→**switch((int)mode)**（ORIG 0x085d9b81
    // `cmp $1; jl; cmp $2; jle; sub $4; cmp $1; ja` 有符号直跳链实证，(int) 收窄复现
    // jl/jle 非 jb/jbe）；②日志条件 `(x!=y)&&(x-y>=0)` 双条件→ `local_44 > local_48`
    // （ORIG 0x085d9bf3 `test; jle` 单测，旧形产 js 域差岛）；③local_3d int→**bool**
    // （ORIG 0x085d9c51 movb 直存 + 0x085d9cb5 movzbl/xor$1 取反物化，P248 档②）。
    // 残余 84 = 4.1 sub+test 物化（0x085d9be5，4.4 恒 cmp 折叠）+ N76/N18a scaled
    // 折叠 + N82 槽号 + C2/C1/重定位显示差。insn 365/367。
    // [L10b 2026-09-10] 两处实测（本件 367→**366**，ORIG 365）：
    //  ① **C05-F2 生效（-1）**：`unsigned int local_2c` → `int local_2c` ⇒ 复现 ORIG
    //     0x085d9df3 内存直比 `cmpl $0x4,-0x28(%ebp); setle %al`（P209 反向可用形，
    //     旧 unsigned 形恒 `mov -N(%ebp),%eax; cmp $0x4,%eax; setle` 多 1 条）。
    //  ② P311 成员数组视图（`PvPTeamScoreView { int m_cnt0[5]; int m_cnt5[5]; }` 替代
    //     `int local_84[10]` + 下标 `[T+5]`）：**实测中性**（366=366）——4.4 把
    //     `m_cnt5[idx]` 编译为 `lea 0x14(%eax)` + `add $0x14,%eax` 两段（与旧 `[T+5]`
    //     的 lea/add 同条数），未复现 ORIG 单条 scaled disp；保留（更真实的成员数组
    //     建模，零指令回归）。
    //  残余 1 条 = 上述两处形态差中的末条 + N82 槽号。
    if (m_battleEndFlag != 0)
    {
        return 0;
    }
    m_battleEndFlag = 1;
    int local_4c = 0;
    if (G_GameWorld()->GetChannelType() == 7)
    {
        local_4c = 1;
    }
    int local_48 = GlobalData::s_systemTime_.getCurSec() - m_battleStartSec;
    int local_44 = 0;
    // [b28d-t3-PoolBigThree] ORIG 0x085d9b81..9b99 直跳链 `cmp $1; jl <def>;
    // cmp $2; jle <c12>; sub $4; cmp $1; ja <def>` = **switch(case 1,2 / 4,5)**
    // 4.1 比较链展开（旧 if 链形 4.4 产 `cmpl $0;jle; cmpl $2;jg` 极性与指令
    // 数均不合）；日志条件 ORIG 0x085d9be5 `sub; test %eax; jle <skip>` 单测
    // ⇒ 合并 `!= && >=0` 双条件为 `>`（原形 js 与 ORIG jle 在 ==0 值域分叉，
    // 真语义修正候选：ORIG 对 limit==elapsed 走 skip，旧形经 && 折叠同结果，
    // 但物化形 js 域差 -2^31，见判词）。（int) 收窄比较域 ⇒ 复现 ORIG 有符号
    // jl/jle（unsigned 形 4.4 恒 jb/jbe；值域 0..5 下转换不改语义）。
    switch ((int)m_pvpBattleMode)
    {
    case 1:
    case 2:
        local_44 = PVP_NORMAL_BATTLE_TIME[local_4c] / 1000;
        break;
    case 4:
    case 5:
        local_44 = PVP_DM_BATTLE_TIME[local_4c] / 1000;
        break;
    }
    // [FL8PvPWorldLeague 2026-09-10] 源码形试点（语义等价改写）：ORIG 0x085d9be6 段为
    // `mov -0x44(%ebp),%eax; mov -0x40(%ebp),%edx; mov %edx,%ecx; sub %eax,%ecx;
    // mov %ecx,%eax; test %eax,%eax; jle`（**减法物化 + test**，非 cmp）⇒ 原码是
    // 减法形。槽实况：-0x44=local_48(play_time)、-0x40=local_44(end_time)；
    // ORIG 先载 local_48（右操作数）⇒ 4.1/4.4 均为「右操作数先载」，故源形须写
    // `local_44 - local_48 > 0`（⇔ 原 `local_44 > local_48`，整型无溢出，严格等价），
    // 4.4 即先载 -0x44 复现 ORIG 序。旧形 `local_44 > local_48` 被 4.4 折成
    // `mov -0x40,%eax; cmp -0x44,%eax` 两段。
    if (local_44 - local_48 > 0)
    {
        cMyTrace local_5c(__PRETTY_FUNCTION__, 0xa9a, 0);
        local_5c("PVP_TIMEOUT_CHECK end_time(%d), play_time(%d)", local_44, local_48);
    }
    if (m_state != 2)
    {
        return 0x13;
    }
    // [b28d-t3-PoolBigThree] P248 档②：ORIG 0x085d9c51/0x085d9c7b `movb $1/$0,-0x39`
    // + 0x085d9cb5 `movzbl; xor $1; test; je` ⇒ bool 局部（int 形反产 movl+cmpl/jne）。
    bool local_3d = true;
    for (int local_3c = 0; local_3c < 7; local_3c = local_3c + 1)
    {
        if (seats[local_3c] != seats[local_3c + 1])
        {
            local_3d = false;
            break;
        }
    }
    if ((m_pvpBattleMode == 2) || (m_pvpBattleMode == 3))
    {
        lock();
        if (!local_3d)
        {
            // [L10b P311 成员数组视图] ORIG 0x085d9da2 段为**单条 scaled disp**
            // `mov -0x6c(%ebp,%eax,4),%edx`（= base+5 段，索引不含 +5）与
            // `mov -0x80(%ebp,%eax,4)`（= base+0 段）；旧 `int local_84[10]` + 下标
            // `[T+5]` 令 4.4 拆 `lea 0x14(%eax)` + 重算两段（每处 +2 条）。
            // 视图：{m_cnt0[5] @+0, m_cnt5[5] @+0x14}，同址等价（sizeof 40）。
            // [J8PvPRoom 2026-09-11 · P311 第③形实测（同 IsThereSameGuildMember 成功形）]
            // 旧形 `int local_84[10]`+`[T+5]` 与 struct 成员数组视图（偏移 0x14）均令
            // 4.4 拆 `lea 0x14(%eax)`+重算两段；本形改**两个独立 5-int 栈数组**
            // （cnt5 先声明 ⇒ 高址 -0x6c、cnt0 ⇒ 低址 -0x80，与 ORIG 0x085d9da2
            // 单条 `mov -0x6c/-0x80(%ebp,%eax,4),%edx` 同布局），索引即 [i]。
            int local_cnt5[5];
            int local_cnt0[5];
            local_cnt5[0] = 0;
            local_cnt5[1] = 0;
            local_cnt5[2] = 0;
            local_cnt5[3] = 0;
            local_cnt5[4] = 0;
            local_cnt0[0] = 0;
            local_cnt0[1] = 0;
            local_cnt0[2] = 0;
            local_cnt0[3] = 0;
            local_cnt0[4] = 0;
            for (int local_30 = 0; local_30 < 8; local_30 = local_30 + 1)
            {
                if ((P_SEATS(local_30) != 0) && (P_TEAMWIN(local_30) != 0) &&
                    (P_TEAMWIN(local_30) < 0xfe))
                {
                    // [L10b] ORIG 0x085d9da2 为单求值（同一 %eax 索引 load/store
                    // 两次），旧 `+= 1` 形令 4.4 对下标 P_TEAMWIN(local_30) 求值
                    // **两次**（读/写各一次）。
                    // [FL8PvPWorldLeague 2026-09-10] 实测前置 `++` 形即单求值：
                    // 本件 ndiff 99→**85**（u 370→366，gap 回 -1），TU 四数不劣化
                    // ⇒ 沿用 `++`。
                    ++local_cnt5[P_TEAMWIN(local_30)];
                    local_cnt0[P_TEAMWIN(local_30)] += seats[local_30];
                }
            }
            unsigned int local_38 = 0;
            int local_34 = 0x7fffffff;
            // [L10b C05-F2（P209 反向可用形，b28d 本波翻案）] 循环变量原为
            // `unsigned int` ⇒ 4.4 对尾测恒先载寄存器（`mov -N(%ebp),%eax; cmp $K,%eax;
            // setle`，多 1 条）；ORIG 0x085d9df3/0x085d9d.. 为内存直比
            // `cmpl $0x4,-0x28(%ebp); setle %al` ⇒ 局部声明 **int** 即复现。
            for (int local_2c = 1; local_2c < 5; local_2c = local_2c + 1)
            {
                if ((0 < local_cnt5[local_2c]) && (local_cnt0[local_2c] < local_34))
                {
                    local_38 = local_2c;
                    local_34 = local_cnt0[local_2c];
                }
            }
            for (int local_28 = 0; local_28 < 8; local_28 = local_28 + 1)
            {
                if (P_TEAMWIN(local_28) == local_38)
                {
                    P_WIN(local_28) = 1;
                }
                else
                {
                    P_WIN(local_28) = 0;
                }
            }
            m_userTable.SetWinTeam(local_38);
        }
        else
        {
            for (int local_24 = 0; local_24 < 8; local_24 = local_24 + 1)
            {
                P_WIN(local_24) = 0;
            }
        }
        unlock();
    }
    else if (m_pvpBattleMode == 5)
    {
        m_deathMatch.checkWinnerForTeamPlay(m_users, m_winCheck, m_teamWin, m_userTable);
    }
    else if (m_pvpBattleMode == 4)
    {
        m_deathMatch.checkWinnerForSinglePlay(m_users, m_winCheck, m_userTable);
    }
    else
    {
        lock();
        if (local_3d == 1)
        {
            for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
            {
                P_WIN(local_10) = 0;
            }
        }
        else
        {
            int local_20 = 0;
            int local_1c = 0x7fffffff;
            for (int local_18 = 0; local_18 < 8; local_18 = local_18 + 1)
            {
                if ((P_SEATS(local_18) != 0) && (seats[local_18] < local_1c))
                {
                    local_20 = local_18;
                    local_1c = seats[local_18];
                }
            }
            for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
            {
                if (local_20 == local_14)
                {
                    P_WIN(local_14) = 1;
                }
                else
                {
                    P_WIN(local_14) = 0;
                }
            }
            m_userTable.SetWinTeam(local_20);
        }
        unlock();
    }
    pvp_request_pvp_rank();
    return 0;
}