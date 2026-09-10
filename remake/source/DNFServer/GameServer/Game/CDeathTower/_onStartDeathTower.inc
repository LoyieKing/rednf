void CDeathTower::_onStartDeathTower()
{
    // [b28d-solo-CDeathTower] verdict=AE（orig 94 / our 94，P167 CLEAR 12/12）：修法 =
    // P254/P265 档③：EnterDungeon 第 4 参改用 CBattle_Field.h 匿名 union 的 enum 视角
    // `m_bloodStateE`（ORIG 0x08467bec 段 `mov 0xcd8(%eax),%eax; mov %eax,%ebx` 两段 =
    // 4.1 对 enum 字段保留 eax→ebx 中转；int 视角 4.4 恒折单段 ⇒ 此前误判 N18a）。
    // [b28d-solo-CDeathTower] ORIG 0x08467b5b：memberNames 0x100 字节内联清零
    //（mov $0x40,%edx + rep stos = 0x40 dwords，P116）；旧 0x40 尺寸为误读。
    char memberNames[0x100] = {0};
    // ORIG 0x08467b83：getMemberNames 填充（转发 _getMemberNames）
    m_party->getMemberNames(memberNames);

    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x08467b94：循环体首 movl $0,-0xc（P197③），再 get_user 赋值。
        CUser* user = 0;
        user = m_party->get_user(i);
        // [b28d-solo-CDeathTower] P0 清绑（N96）：ORIG 0x08467bd3 mov$1 岛无槽。
        if ((user == 0) || (int)!m_party->checkValidUser(i))
            continue;
        // ORIG 0x08467bec：EnterDungeon 第 4 参 = m_party->m_battleField.m_bloodState
        //（+0xcd8 直读；非 get_party_type 虚调，p167 EXTRA 实证）。
        // [b28d-solo-CDeathTower] 残差 = ORIG `mov 0xcd8(%eax),%eax; mov %eax,%ebx`
        // 两段 vs 4.4 单段。P265 实参 `(ENUM_BLOOD_STATE)` cast 实测无效（形参为
        // int，enum→int 折叠；且改形参类型会改 mangled）⇒ N18a/P248 档③封顶。
        user->m_historyLog.EnterDungeon(
            m_dungeonMgr.getDungeon()->GetDungeonName(), 0, memberNames,
            m_party->m_battleField.m_bloodStateE);
        // [b28d-solo-CDeathTower] P183：ORIG 每使用点重调 getCurCharacQuestW（×2）
        // 与 getDungeon（×2）；UserQuest +0x75d8 = m_authenData。
        user->getCurCharacQuestW()->m_authenData.reset();
        user->getCurCharacQuestW()->set_authen_data(
            (QUEST_CONDITION)0, m_dungeonMgr.getDungeon()->get_index(), -1);
    }
