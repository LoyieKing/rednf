// ============================================================================
// ORIG T 0x0860a680 _CountQuestSPBook(const CUser*,int&,int&,int&,int&)
// 经 UserQuest+0x04 的 CQuestClear 子对象遍历已完成任务并回调统计。
// ============================================================================
void CSkillChanger::_CountQuestSPBook(const CUser* user, int& a, int& b,
                                      int& c, int& d)
{
    // ORIG 无槽：getCurCharacQuestR 结果直存 eax、+0x4 经 lea 内联（P0/P183）。
    (&user->getCurCharacQuestR()->m_cleared)
        ->enumQuestClear(&_fnEnumQuestClear, &a, &b, &c, &d,
                         static_cast<CSkillChanger*>(this),
                         const_cast<CUser*>(user));
}