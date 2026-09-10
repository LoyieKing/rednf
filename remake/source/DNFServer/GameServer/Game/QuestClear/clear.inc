void CQuestClear::clear()
{
    // ORIG 0x0808bb06：memset 0x7530 + save-flag(+0x7530) 单指令清零。
    memset(m_cleared, 0, 0x7530);
    m_saveFlag = 0;
