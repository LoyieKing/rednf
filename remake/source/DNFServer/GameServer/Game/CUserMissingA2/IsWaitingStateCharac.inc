bool UserMercenaryInfoMgr::IsWaitingStateCharac(unsigned int characNo)
{
    // b28（P211 同族）：index 用 int ⇒ vector operator[](size_t) 实参转换
    // 中转触发 RHS 值先载 = ORIG `mov -0xc,%edx; mov this,%eax`（探针
    // /tmp/l12p3.cpp w4 vs w1/w5）。
    const int index = (int)FindIndexByCharacNo(characNo);
    if (!CheckCharacIndex(static_cast<unsigned char>(index)))
        return true;
    return m_characs[index].GetCompetitionState() == 0;
