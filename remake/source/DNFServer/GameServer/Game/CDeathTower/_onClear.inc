void CDeathTower::_onClear(bool isLast)
{
    int playTime = 0;   // ORIG +0x00c 头部 movl $0,-0x14（P197③ 声明=0）；
                        // /1000 为 signed magic 削减（imul/sar/sub @0x8467f2a）⇒ int
    if (isLast)
    {
        playTime = m_playData.getPlayTime();
    }
    else
    {
        playTime = m_playData.getLastPlayTime();
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* user = 0;              // ORIG +0x04d 循环体首 movl $0,-0xc（P197③）
        user = m_party->get_user(i);
        if (user == 0 || !m_party->checkValidUser(i))
            continue;
        // ORIG 二次 get_user 后 eax 直传 setChattingMessageCount 实参槽
        //（无 -0xc 回读；具名 user 复制生 [mov %eax,-0xc; mov -0xc,%eax]
        // 额外往返）。
        m_party->get_user(i)->setChattingMessageCount(0);
        // CUser +0x79700：cUserHistoryLog（public 成员 m_historyLog，CUser.h 2197）
        user->m_historyLog.DungeonClearInfo((int)isLast, playTime / 1000);
    }
}