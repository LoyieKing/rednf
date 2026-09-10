bool CQuestClear::setClearedQuest(unsigned int questIdx)
{
    // save-flag=1 后置 m_cleared[idx]=1（单指令 disp 写入）。
    if (questIdx == 0 || questIdx > 0x752f)
        return false;
    m_saveFlag = 1;
    m_cleared[questIdx].m_cleared = 1;
    return true;
