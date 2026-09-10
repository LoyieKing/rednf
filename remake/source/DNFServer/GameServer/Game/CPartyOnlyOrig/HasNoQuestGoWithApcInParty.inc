bool CParty::HasNoQuestGoWithApcInParty(Quest const* quest)
{
    for (int i = 0; i <= 3; i++)
    {
        if (_checkValidUser(i))
        {
            // C05③ 试形：ORIG 0x73/0xa8/0xb3/0xc0 四条失败 jcc 汇同一 false
            // 出口（0x666b `mov $0,%eax`）= 4.1 单条短路链的 lowering
            // （0x41 `je body` 为 `||` 首条件直跳）。原两层 if 写法令 4.4 逐层
            // 各物化一次（多 `mov $1; jmp; mov $0; test; je` 5 条）。
            if ((m_member[i].m_pUser->get_state() == 5 ||
                 m_member[i].m_pUser->get_state() == 10) &&
                m_member[i].m_pUser->getCurCharacQuestR()
                    ->IsDoingQuest(quest->m_index) &&
                quest->m_triggerType == 3 && quest->m_triggerSubType == 0)
            {
                return false;
            }
        }
    }
    return true;
