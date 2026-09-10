bool CQuestClear::resetClearedQuests(unsigned int questIdx)
{
    // ORIG 0x0808baac：save-flag=1 后清 m_cleared[idx]=0（movb $1,0x7530(%eax) 单指令）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    m_saveFlag = 1;
    m_cleared[questIdx].m_cleared = 0;
    return true;
