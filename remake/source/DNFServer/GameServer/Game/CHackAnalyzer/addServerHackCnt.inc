// b28d-solo-CHackAnalyzer: AE（CParty 视图 + 嵌套 if + 三目 lvDiff + 去
// (int)count cast + 去 dungeon/memberCount 局部；旧 N82/P183 判词作废）
// ——ORIG 0x080f8c7e。
void CHackAnalyzer::addServerHackCnt(CUser* user, ENUM_HACKTYPE type,
                                     unsigned int count, unsigned int a,
                                     unsigned int b)
{
    // ORIG：直接用形参比较，不落局部（保持指令形态）。
    if ((int)type < 0 || 0xfa1 < (int)type)
    {
        // ORIG：SetETC 返回值直通实参（无中间变量物化）。
        addServerHackCnt(user, (ENUM_HACKTYPE)0x34e, count,
                         (unsigned int)user->SetETC(0x34e, (int)getBaseHackType()),
                         0u);
        setBaseHackType((ENUM_HACKTYPE)0);
        return;
    }

    // ORIG：CheckExceptionCnt 返回真（异常额度内）→ 跳过统计；假 → 继续
    // 并在尾部推进计数器（见 addServerHackCnt.md 汇编 +0x93/+0x98）。
    if (CheckExceptionCnt(type, (int)count))
        return;
    if (Secu_AccountHacking::checkAccountHackingHackType(type) &&
        HACK_SLOT_B(type) != 0)
        return;


    CParty* party = (CParty*)user->GetParty();
    // b28d-solo-CHackAnalyzer: ORIG 0x080f8d66 为嵌套 if（第二条件发
    // `cmp $5; sete; test; je`，探针 /tmp/p_and.cpp f2 实证 4.4 唯一同形；
    // `&&` 形 f1 恒产 mov$1/mov$0 岛）。
    if (party != 0)
    {
        if (user->get_state() == 5)
        {
            // ORIG：get_member_count 结果直通实参（无 memberCount 局部）。
            if (!CheckHackRateByPartyCount(type, party->get_member_count()))
                return;
            // [hc 收] ORIG：party+0xcac 为 CDungeon*（指针）、party+0xb24 为
            // 内嵌 CBattle_Field（§8.3 未建模子对象具名视图，P82/P301：视图
            // 成员访问保 disp 折叠 `mov 0xcac(%eax),%eax`；char* 加法会退化成
            // add $0xcac）。ORIG dungeon 不落槽、两臂各重读一次（P48/P183）。
            if (((CPartyHackView*)party)->m_pDungeon != 0)
            {
                // ORIG 三目形（test/js 直跳，P47；if/else 赋值会发 shr$31 物化）。
                int lvDiff =
                    (((CUserCharacInfo*)user)->get_charac_level() -
                         ((CPartyHackView*)party)->m_pDungeon->get_standard_level() <
                     0)
                        ? 0
                        : ((CUserCharacInfo*)user)->get_charac_level() -
                              ((CPartyHackView*)party)->m_pDungeon
                                  ->get_standard_level();
                if (!CheckHackRateByDifficult(
                        type, lvDiff,
                        ((CBattle_Field*)((char*)party + 0xb24))->get_dungeon_diff()))
                    return;
            }
        }
    }

    if (((int)type == 0x25b) || ((int)type == 0x25c) ||
        ((int)type == 0x25d) || ((int)type == 0x25e) ||
        ((int)type == 0x25f) || ((int)type == 0x260) ||
        ((int)type == 0x325) || ((int)type == 0x6f))
        return;

    {
        // b28d-solo-CHackAnalyzer: 勿加 (int)count 冗余 cast——ORIG 0x080f8eb9
        // `movl 0x8(%eax,%edx,4),%eax; addl 0x14(%ebp),%eax; mov %eax,%edx` 为
        // int 字段 += unsigned 形参（异型 ⇒ add mem+中转，P274 反向；探针
        // /tmp/p_add.cpp f2 逐条同形；加 cast 变同型恒产 lea 三操作数 f1）。
        HACK_CNT_A(type) += count;
        HACK_CNT_B(type) += count;
        onUpdateHackCount(user, type, count, a, b);
    }
}