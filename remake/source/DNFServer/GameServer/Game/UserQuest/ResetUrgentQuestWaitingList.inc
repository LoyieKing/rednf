void UserQuest::ResetUrgentQuestWaitingList()
{
    // [b28c-L9] fresh 复验 ndiff 11→1：唯一残差 = 4.4 把 quest2!=0 臂尾的
    // 「写 -1 + jmp end」与 else 臂尾的 jmp 合并（共享尾块少一枚 jmp）。
    Quest* quest2;
    for (int i = 0; i <= 19; ++i)
    {
        Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
        if (quest != 0 && quest->m_type == 8)
        {
            return;
        }
    }

    if (hasEpicQuest())
    {
        m_authenData.m_urgentQuest = -1;
    }
    else
    {
        quest2 = G_CDataManager()->find_quest(m_authenData.m_urgentQuest);
        // b25c 翻案：ORIG 4.1 把 quest2==0 的块外置在后（je 落点岛），
        // 且槽序 quest2/-0x18 i/-0x14 quest/-0x10 levelLimit/-0xc。
        // b28d-40：两臂 return 改自然结束（ORIG 0x086ad24e/0x086ad25d 各一条 jmp，
        // return 形多一条 nop+jmp 中继）。
        if (quest2 != 0)
        {
            int levelLimit = quest2->m_levelMax;
            if (levelLimit > UrgentQuestLevelLimit::kEndLvl)
            {
                levelLimit = UrgentQuestLevelLimit::kEndLvl;
            }
            if (m_user->get_charac_level() + 1 > levelLimit)
            {
                m_authenData.m_urgentQuest = -1;
            }
        }
        else
        {
            m_authenData.m_urgentQuest = -1;
        }
    }
}