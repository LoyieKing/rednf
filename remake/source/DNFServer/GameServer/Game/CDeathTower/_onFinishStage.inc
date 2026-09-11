void CDeathTower::_onFinishStage()
{
    // [b28d-solo-CDeathTower] verdict=AE（orig 85 / our 85，P167 CLEAR 11/11）：修法 =
    // P52/P183：ORIG 0x08467d79/0x08467dbb 两处链式单表达式
    // （getHackAnalyzer 结果直压 (%esp) 无具名槽；getStageMapIndex 结果先物化 %ebx 跨
    // getCurCharacQuestW 保活）⇒ 去 hack/quest 具名局部，槽基回到 ORIG -0xc/-0x10。
    m_stage.onFinishStage();
    m_playData.addPlayTime(m_stage.getStageClearTime());

    for (int i = 0; i <= 3; ++i)
    {
        // [b28d-solo-CDeathTower] P197③：ORIG 0x08467d2a 循环体首 `movl $0,-0xc`
        // 预清 ⇒ 声明=0 与赋值分两句式。
        CUser* user = 0;
        user = m_party->get_user(i);
        if (user == 0 || !m_party->checkValidUser(i))
            continue;
        // [b28d-D13] P196 真语义修正：ORIG 0x08467d24 cmp $0x176f; setbe（无符号
        // 比较）⇒ 源码须为 unsigned 比较（int 版 4.4 产 setle，负数时语义分叉）。
        if ((unsigned int)m_stage.getStageClearTime() <= 0x176f)
        {
            user->getHackAnalyzer()->addServerHackCnt(
                user, (WongWork::ENUM_HACK_TYPE)0x19a, 1u, 0u, 0u);
        }
        user->getCurCharacQuestW()->set_authen_data(
            (QUEST_CONDITION)1, m_stage.getStageMapIndex(), 0);
    }
}