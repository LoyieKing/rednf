// ORIG T 0x085a82d2（IsClearedQuest.md）：任一有效成员未清任务即 false。
// b28d-45 IsClearedQuest 0x085a82d2：FIX→AE —— Main 已落 QuestClear bool 链
// （STClearedQuest::m_cleared / CQuestClear::isClearedQuest / CUser.h:1270），
// 调用点改裸取反 `if(!isClearQuest(...))` 后 40:40 槽位集全等、xor 段逐条对齐；
// 余为 R_386_32 重定位显示差（链接后即 ORIG 绝对地址）。
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
}