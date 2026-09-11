// ORIG 0x0808ba78：TU 内第一个**非内联**函数 ⇒ 决定 _GLOBAL__I_ 键名
// （ORIG 键 = _GLOBAL__I__ZN8WongWork11CQuestClear15setClearedQuestEj）；
// 构造/析构已按 ORIG 弱符号形改头内联定义（见 QuestClear.h）。
bool CQuestClear::setClearedQuest(unsigned int questIdx)
{
    // save-flag=1 后置 m_cleared[idx]=1（单指令 disp 写入）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    m_saveFlag = 1;
    m_cleared[questIdx].m_cleared = 1;
    return true;
}