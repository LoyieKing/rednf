// ----------------------------------------------------------------------------
// ORIG T 0x081b4536：无效下标→0；有效项 Competition(user,modeA,modeB) 成功→1。
// modeB/modeA 先 movsbl 入 esi/ebx 跨 operator[] 存活（P52）。
// ----------------------------------------------------------------------------
int UserMercenaryInfoMgr::CompetitionCharac(CUser* user, unsigned int characNo,
                                            char modeA, char modeB)
{
    const int index = (int)FindIndexByCharacNo(characNo);  // b28 P211 同族
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return 0;
    if (!m_characs.operator[](index).Competition(user, modeA, modeB))
        return 0;
    return 1;
}