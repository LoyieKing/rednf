int CParty::GetPartyType()
{
    // [SweepCParty] p167 CLEAR；gap 1（106 vs 107）= N18b（`mov -0x1c,%edx; cmp %edx,%eax`
    // 两段 vs 4.4 恒折 cmp mem；int 化实测同形，S1 一次旋钮已用尽）。封顶。
    // b28d-solo-CParty：ORIG 0x85b637d `mov -0x1c(%ebp),%edx; cmp %edx,%eax`
    // 为两段（P254/P211 档③）：局部与 callee 返回类型异型 ⇒ 4.4 才发 eax→edx
    // 中转而非 mem 直比。guildKey 声明 unsigned int。
    // N18b 封顶：int 化实测同形（cmp mem,%eax 恒折）⇒ 恢复直证形态。
    // [2026-09-10 复测] P211 档③ 的**同型旋钮**（局部 unsigned int → int，与 callee
    // `CUserCharacInfo::get_charac_guildkey()` 的 int 同型）与 F2（C05 范式②）两态
    // 实测**均仍折** `cmp -0x1c(%ebp),%eax`（ORIG 0x85b637d 为两段
    // `mov -0x1c(%ebp),%edx; cmp %edx,%eax`）⇒ 恢复直证 unsigned 形，判 N18b。
    unsigned int guildKey = 0;
    bool sameGuild = false;
    int onlineCnt = 0;
    int validCnt = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        // ORIG 0x85b6322/0x85b6350/0x85b63a0：get_charac_guildkey 三处独立调用
        // （O0 无 CSE，P48），勿缓存 key。
        if (m_member[i].m_pUser->get_charac_guildkey() != 0)
        {
            if (guildKey == 0)
            {
                guildKey = m_member[i].m_pUser->get_charac_guildkey();
                sameGuild = true;
            }
            // [SweepCParty2] 试形记录：交换比较操作数（callee 结果居左）实测仍产
            // `cmp -0x1c(%ebp),%eax` 折叠（.o 0x6743 实证）⇒ 与 int 化同为无效旋钮，
            // N18b 封顶（ORIG 0x85b637d 两段 `mov -0x1c(%ebp),%edx; cmp %edx,%eax`）。
            else if (guildKey != m_member[i].m_pUser->get_charac_guildkey())
            {
                sameGuild = false;
            }
        }
        // b28d-solo-CParty：ORIG 0x85b63a7 无 connectId 槽（call 结果经 %eax 直传
        // 实参槽，P0 寄存器直传）⇒ 内联表达式，勿具名局部。
        if (IsExistCharacter(m_member[i].m_pUser->get_connect_upper_member()))
            ++onlineCnt;
        ++validCnt;
    }
    if (sameGuild)
        return 0;
    // b28d-solo-CParty：ORIG 0x85b63ea `jle <尾 mov $2 岛>` ⇒ 正条件包裹
    // （P214①/P137：失败块落尾，4.4 才发 jle 而非 jg 前置）。
    if (onlineCnt >= 1)
    {
        if (validCnt - 1 == onlineCnt)
            return 1;
        return 4;
    }
    return 2;
