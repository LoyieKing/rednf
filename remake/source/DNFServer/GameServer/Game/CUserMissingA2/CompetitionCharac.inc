int UserMercenaryInfoMgr::CompetitionCharac(CUser* user, unsigned int characNo,
                                            char modeA, char modeB)
{
    const int index = (int)FindIndexByCharacNo(characNo);  // b28 P211 同族
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return 0;
    if (!m_characs.operator[](index).Competition(user, modeA, modeB))
        return 0;
    return 1;
