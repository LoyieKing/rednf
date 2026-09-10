bool CParty::IsClearedQuest(int questIdx)
{
    for (int i = 0; i <= 3; i++)
    {
        // ORIG O@0x0f `test%al;je` 无 xor（bool 直测跳过 body ⇒ 嵌套 if 守卫）；
        // 内部 isClearQuest `!=1` 用 xor（P194 聚合布尔物化）。
        if (_checkValidUser(i))
        {
            // b28d-45 P248 档①：Main 已落 QuestClear/isClearQuest bool 链
            // （ORIG 0x085a8320 `call; xor $0x1; test %al,%al; je`）⇒ 裸取反形。
            if (!m_member[i].m_pUser->getCurCharacQuestR()
                     ->isClearQuest(questIdx))
            {
                return false;
            }
        }
    }
    return true;
