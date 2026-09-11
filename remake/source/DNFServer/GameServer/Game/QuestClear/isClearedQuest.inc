bool CQuestClear::isClearedQuest(unsigned int questIdx) const
{
    // ORIG 0x0808bae0：idx==0 或 idx>0x752f → 0；否则 movzbl 直返字节（P9 无 setne）。
    if (questIdx == 0 || questIdx > 0x752f)
        return 0;
    return m_cleared[questIdx].m_cleared;
}