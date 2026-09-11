void CUser::DeleteCharac(unsigned int characNo)
{
    // ORIG 0x0864a63a（报告 DeleteCharac.md + objdump 逐指令核对）。
    // ORIG 不缓存 _Charac_info& 引用：对每个字段读取均重新 m_characList[i]（operator[]）。
    // ORIG 返回 void（Ghidra: return;）；调用方按语句调用不用返回值。
    bool erased = false;
    std::vector<_Charac_info>::iterator delIt = m_characList.begin();
    int i = 0;
    while ((int)m_characList.size() > i)
    {
        if (m_characList[i].m_characNo == characNo)
        {
            HistoryLog::WriteDeleteCh(reinterpret_cast<FILE*>(m_field796f8),
                                      m_characList[i].m_name,
                                      (int)m_characList[i].m_field26,
                                      (int)(short)m_characList[i].m_level,
                                      // ORIG 0x0864b0xx `movzbl 0x29(%eax),%eax; movsbl %al,%edi`
                                      // 双段 = 字节读后**符号扩展**传参（P11）⇒ 显式 signed char cast。
                                      (int)static_cast<signed char>(m_characList[i].m_field29.m_growthType));
            m_historyLog.CharacDel(i);
            erased = true;
        }
        if (m_characList[i].m_field14b8 == characNo)        // +0x14b8 striker link 槽
            m_characList[i].m_field14b8 = 0;
        if (!erased)
            delIt++;
        ++i;
    }
    if (erased)
    {
        m_characList.erase(delIt);
        CalcurateManageLevel();
    }
    m_characterView.enableSaveCharacView();
}