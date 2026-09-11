// ---- startBreakAwayPreventSystem（ORIG 0x0831ce78）-------------------------
void CBreakAwayPreventSystem::startBreakAwayPreventSystem(
    SIG_LOGIN_DATA const* login, CUser* user)
{
    int dayDiff =
        static_cast<int>(getDiffConnectDay(login->m_breakAwayLastRewardTime));

    // §4.3 具名化：+0x0c = 基类 SIG_BREAK_AWAY_PREVENT_SYSTEM_MANAGER::m_fieldC
    // （unsigned short，SigTypes_A.h:470）。ORIG T 0x0831ce78
    // startBreakAwayPreventSystem 汇编 @+0x021..+0x029：
    //   `mov 0x8(%ebp),%eax` / `movzwl 0xc(%eax),%eax` / `cwtl` /
    //   `mov %eax,-0x10(%ebp)` ⇒ 无符号 16 位装载 + 显式 (short) 拓宽落 int 槽
    //   （有槽 ⇒ 原码确有具名局部，P0），故写 `int dayLimit = (short)m_fieldC;`。
    int dayLimit = (short)m_fieldC;
    // §4.3 具名化（+0x302/0x303/0x308/0x309 裸偏移清零）：+0x2fc 起为
    // m_keys[5]（STRewardDataKey 3 字节紧排，见 BreakAwayPreventSystem.h 布局
    // 注释），故 +0x302/0x303 = m_keys[2].m_data[0]/[1]、+0x308/0x309 =
    // m_keys[4].m_data[0]/[1]。ORIG T 0x0831ce78 对这四个字节是**无栈槽**的
    // 内联直读（@0x0831cfad `movzx eax,BYTE PTR [eax+0x302]` + `movsx eax,al`
    // 后紧接 `cmp eax,[ebp-0x14]`=dayDiff；另三点 +0x303/@0x0831cfc3、
    // +0x308/@0x0831d0ae、+0x309/@0x0831d0c4 同形）⇒ 按 P0 原码无对应局部，
    // 故读取写成条件内右值（原 `signed char winALow=…` 四个具名槽系重建侧
    // 发明）；`(signed char)(unsigned char)` 双步保 movzx→movsx 有符号日界
    // 语义（非 P196 反演）。实测 ndiff 263→186、verdict 桶不变。

    // A 窗口：无归还记录或回归天数越过上限 → luck_point 主通道。
    if (login->m_breakAwayLastRewardTime == 0 || dayDiff >= dayLimit)
    {
        bool pointOk = calBreakAwayUserPoint();
        if (pointOk)
        {   // ORIG 内联（applyWindowReward key=0 order=1 writeOrder）
            if (login->m_breakAwayAccureCera >= 0)
            {
                // ORIG 0x0831cee2：bool 预清 + 赋值两句式（P198①）。
                bool ok = false;
                ok = selectReward(0, login->m_breakAwayAccureCera, user);
                if (ok)
                {
                    user->setBreakAwayRewardData(true);
                    user->setBreakAwayRewardOrder(
                        static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(1));
                }
            }
            else
            {
                if (login->m_breakAwayLuckPoint == 0)
                    user->setBreakAwayRewardData(false);
            }
        }
        return;
    }

    // 区间 1：second < first（u32 比较）且 dayDiff ∈ [winA_low, win_a_high] → order 3。
    // ORIG 0x0831cf6e：三条件独立短路（无 questFieldsIdle/firstReward 具名局部）。
    if (login->m_breakAwayDungeonClearCnt == 0 &&
        login->m_breakAwayQuestTime != 0 &&
        login->m_breakAwayRewardBoundA > login->m_breakAwayRewardBoundB)
    {
        // ORIG 0x0831cfba/0x0831cfd0：日界两守卫各落**独立** nop 中继岛
        // （0831d158/0831d15b），非共享尾 —— 必须写成两条独立否定 goto
        // （P223 双守卫版；合并进外层 && 链会被 4.4 jump-thread，少 4 条）。
        if ((signed char)(unsigned char)m_keys[2].m_data[0] > dayDiff)
            goto WIN1_LOW_FAIL;
        if (dayDiff > (signed char)(unsigned char)m_keys[2].m_data[1])
            goto WIN1_HIGH_FAIL;
        if (login->m_breakAwayAccureCera >= 0)
        {   // ORIG 内联（applyWindowReward key=2 order=3 writeOrder）
            bool ok = false;
            ok = selectReward(2, login->m_breakAwayAccureCera, user);
            if (ok)
            {
                user->setBreakAwayRewardData(true);
                user->setBreakAwayRewardOrder(
                    static_cast<ENUM_BREAK_AWAY_REWARD_ORDER>(3));
            }
        }
        else
        {
            if (login->m_breakAwayLuckPoint == 0)
                user->setBreakAwayRewardData(false);
        }
        return;
    }

    // 区间 2：first < second 且 dayDiff ∈ [winB_low, win_b_high] → 只置 reward 标志。
    if (login->m_breakAwayDungeonClearCnt == 0 &&
        login->m_breakAwayQuestTime != 0 &&
        login->m_breakAwayRewardBoundA < login->m_breakAwayRewardBoundB &&
        (signed char)(unsigned char)m_keys[4].m_data[0] <= dayDiff &&
        dayDiff <= (signed char)(unsigned char)m_keys[4].m_data[1])
    {
        if (login->m_breakAwayAccureCera >= 0)
        {   // ORIG 内联（applyWindowReward key=4 无 order 写）
            bool ok = false;
            ok = selectReward(4, login->m_breakAwayAccureCera, user);
            if (ok)
            {
                user->setBreakAwayRewardData(true);
            }
        }
        else
        {
            if (login->m_breakAwayLuckPoint == 0)
                user->setBreakAwayRewardData(false);
        }
    }
WIN1_LOW_FAIL: ;
WIN1_HIGH_FAIL: ;
}