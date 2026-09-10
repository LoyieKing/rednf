int CParty::getMemberPassParty(CUser* user)
{
    // b28d-solo-CPartyOnlyOrig：ORIG 槽 maxLevel=-0x14 / level=-0x10（Ghidra C
    // 同序）——4.4 按首次使用序分配，声明序翻转为 level 在前才得 ORIG 槽序
    //（N93 标量例外）。level 无清零（ORIG 只有 -0x14/-0xc 两条 movl $0）。
    int level;
    int maxLevel = 0;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        // b28d-solo-CPartyOnlyOrig：ORIG 0x6b/0xad 两比较汇入
        // `mov $1; jmp; mov $0; test %al; jne continue` 常量岛物化 ⇒ 源码是
        // 单个 `||` 短路条件（P133 反例：两个独立 if 生 sete/test 而非常量岛）。
        if (reinterpret_cast<CUserCharacInfo*>(user)
                    ->get_connect_upper_member() ==
                m_member[i].m_pUser->get_charac_no(-1) ||
            user->get_charac_no(-1) ==
                m_member[i].m_pUser->get_charac_no(-1))
        {
            continue;
        }
        // ORIG +0xee：std::max 外部实例 call（0808e218）+ 返回指针解引用赋值。
        level = m_member[i].m_pUser->get_charac_level();
        maxLevel = std::max(maxLevel, level);
    }
    // b28d-solo-CPartyOnlyOrig：Ghidra C 尾段 = bVar1 常量岛物化
    //（`mov $1; jmp; mov $0; test %al; je`，ORIG +0x131..+0x13f），随后
    // **再次**判 maxLevel==0（+0x148），否则返回 1 ⇒ 单个 `&&` 条件 +
    // 重复零判，而非两个独立 return（后者 4.4 产 setle/test 物化）。
    if (maxLevel != 0 &&
        maxLevel -
            reinterpret_cast<CUserCharacInfo*>(user)
                ->get_charac_level_before_dungeon() <= 6)
    {
        return 0;
    }
    if (maxLevel == 0)
    {
        return 0;
    }
    return 1;
