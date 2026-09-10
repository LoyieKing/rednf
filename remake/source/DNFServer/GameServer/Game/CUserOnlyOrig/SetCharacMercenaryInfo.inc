void CUser::SetCharacMercenaryInfo(int count, CHARAC_LOAD_MERCENARY* infos)
{
    // SetCharacMercenaryInfo.md @0x0868e544：i<count && i<0x24 循环，
    // 逐元素 UserMercenaryInfoMgr::AddCharac(infos[i])（stride 0x54）。
    for (int i = 0; i < count && i < 0x24; ++i)
    {
        m_mercenaryMgr.AddCharac(infos[i]);
    }
