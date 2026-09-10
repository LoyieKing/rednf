const _Additioal_info* CCharacter::get_growtype_stat(int firstGrow,
                                                     int secondGrow) const
{
    if (firstGrow > 5 || secondGrow > 2)
        return 0;
    if (secondGrow == 0)
        return &m_baseInfoGrow[firstGrow];
    return &m_secondGrowTypes[firstGrow + 1].m_addInfo[secondGrow - 1];
