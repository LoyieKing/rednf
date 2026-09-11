bool CDeathTower::onLeaveUser(CUser* user)
{
    // [b28d-solo-CDeathTower] verdict=AE（orig 131 / our 131，P167 CLEAR 14/14）：修法 =
    // ①getMemberNames 返回值直接作 LeaveDungeon 第 3 参（ORIG 0x084637b1 `mov %eax,%ebx`
    //   + 0x084637d6 `mov %ebx,0xc(%esp)`）；
    // ②GetInstanceDungeonEntranceLog 链式单表达式（ORIG 0x0846382f/31 get_index 先落 %ebx
    //   跨 receiver 保活，P52；去具名 entranceLog 槽）；
    // ③去 pad5[12] 死局部（去具名槽后 buf 自然落 -0x119 非 4 对齐槽，4.1 对齐剥离 +
    //   rep stos + 尾 2/1 字节慢路保留）。state 槽 -0x19 / push %esi / `add $0x79700`
    //   随之全部与 ORIG 对齐。
    user->setDeathTowerIndex((short)-1);
    // [b28d-solo-CDeathTower] P52/P183：ORIG 0x08463711 为链式单表达式
    //（getCurCharacInvenW 结果直压 esp 调 RemoveFPItem，无具名 inven 槽中转）。
    ((CInventory*)user->getCurCharacInvenW())->RemoveFPItem();
    // [b28d-solo-CDeathTower] ORIG 0x08463724 起内联对齐 memset（无 memset call）⇒
    // 写 `= {0}`（P214③/P232）。ORIG buf 基址 -0x119（非 4 对齐）⇒ 4.1 发
    // 对齐剥离 + rep stos + 尾 2/1 字节慢路。
    char memberNames[0x100] = {0};

    if (m_party)
    {
        // ORIG 0x0846378d：state = (char)m_party->m_battleField.m_bloodState（+0xcd8 低字节）
        char state = (char)m_party->m_battleField.m_bloodState;
        // [b28d-solo-CDeathTower] ORIG 0x084637b1 `mov %eax,%ebx` + 0x084637d6
        // `mov %ebx,0xc(%esp)`：getMemberNames 返回值（char*）直接作 LeaveDungeon
        // 第 3 参（非传 memberNames 数组地址）。
        user->m_historyLog.LeaveDungeon(
            m_dungeonMgr.getDungeon()->GetDungeonName(), 0,
            m_party->getMemberNames(memberNames), state);
    }

    // [b28d-solo-CDeathTower] ORIG 0x084637f7..0x08463845：m_party && getDungeon()!=0，
    // 再重调 getDungeon()->get_index()（P183 重物化，getDungeon 共 2 次）。
    if (m_party && m_dungeonMgr.getDungeon() != 0)
    {
        // [b28d-solo-CDeathTower] ORIG 0x0846382f/0x08463831：get_index 结果先落
        // %ebx 跨 GetInstanceDungeonEntranceLog 保活（P52 链式单表达式，无具名槽）。
        GetInstanceDungeonEntranceLog()->DecrementDungeonEntrance(
            m_dungeonMgr.getDungeon()->get_index(), false);
    }

    // ORIG 0x0846384a：!m_party || get_member_count() <= 0 → _destroy()
    if (!m_party || m_party->get_member_count() <= 0)
    {
        _destroy();
    }
    return 1;
}