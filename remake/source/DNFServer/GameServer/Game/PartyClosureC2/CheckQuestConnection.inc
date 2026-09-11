// ===========================================================================
// CParty::CheckQuestConnection —— T 0x085b6478
// 报告：CheckQuestConnection.md：
// type==0 → 任一有效成员 IsDoingQuest；type==1 → isClearQuest；否则 false。
// ===========================================================================
bool CParty::CheckQuestConnection(int questIdx, ENUM_MAZE_QUEST_TYPE type)
{
    // [b28d-46 判词·N/P168a③b] ORIG 0x085b6478：64 vs 66，p167 CLEAR；残余唯一处
    // `call; test %al; je`（uchar callee 直判）vs 4.4 `setne %al; test` 归一
    // （跨版本 bool ABI 差）⇒ 封顶。
    for (int i = 0; i <= 3; ++i)
    {
        if (_checkValidUser(i))
        {
            switch ((int)type)
            {
                case 0:
                    if (m_member[i].m_pUser->getCurCharacQuestR()->IsDoingQuest(
                            questIdx))
                        return true;
                    break;
                case 1:
                    if (m_member[i].m_pUser->getCurCharacQuestR()->isClearQuest(
                            questIdx))
                        return true;
                    break;
            }
        }
    }
    return false;
}