void CUser::verifyDoingQuest()
{
    for (int i = 0; i <= 0x13; ++i)
    {
        // ORIG 0x0868d156：doingIdx 槽 -0xc 先死初始化 0（4.1 具名局部）；
        // 0x0868d163/0x0868d184：getCurCharacQuestR 每使用点各调一次（免 spill 槽，
        // 技巧37）；0x0868d1a2/0x0868d1be：getCurCharacQuestW 同样两次独立调用。
        int doingIdx = 0;
        doingIdx = getCurCharacQuestR()->m_quest[i];         // 类内 +0x7538
        if (doingIdx == 0)
            continue;
        if (getCurCharacQuestR()->isClearQuest(doingIdx))
        {
            getCurCharacQuestW()->m_quest[i]   = 0;
            getCurCharacQuestW()->m_trigger[i] = 0;          // 类内 +0x7588
        }
    }
