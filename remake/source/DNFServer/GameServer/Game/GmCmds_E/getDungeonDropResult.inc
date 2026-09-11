// ORIG getDungeonDropResult(list&, map&, map2&) 0x084ae0e2：按 mode 归账。
// b28d-solo-GmCmds_E：P251——ORIG 为无栈槽的 `cmp $2;je / cmp $3;je /
// test;jne` 决策链（m_mode 值恒在 %eax），原码是 switch(m_mode) 而非 if 链。
void CMonsterDropTest::getDungeonDropResult(std::list<map_item>& items, DropStat& drop,
                                            GradeStat& grade)
{
    switch (m_mode)
    {
    case 2:
    case 0:
        makeGeneralItemDropList(items, drop);
        makeUpgradeDropList(items, grade);
        break;
    case 3:
        makeGoldItemDropList(items, drop);
        break;
    }
}