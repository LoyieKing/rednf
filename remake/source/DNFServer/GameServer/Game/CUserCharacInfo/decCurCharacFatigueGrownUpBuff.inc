void CUserCharacInfo::decCurCharacFatigueGrownUpBuff(unsigned int value)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    if ((unsigned int)CUR->m_fatigueGrownUpBuff > value)
        CUR->m_fatigueGrownUpBuff = (unsigned short)(CUR->m_fatigueGrownUpBuff - value);
    else
        CUR->m_fatigueGrownUpBuff = 0;
    }
}