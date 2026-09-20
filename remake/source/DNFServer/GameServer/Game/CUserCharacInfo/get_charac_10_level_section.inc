// [P8 2026-09-11 新口径复核] n1_v（objdump -dr + reloc 归一）verdict=AE，62/63；
//   旧判词 NEAR 系 `game_func_report.py --no-compile`（objdump 无 -r）的假残差。
int CUserCharacInfo::get_charac_10_level_section() const
{
    if (m_selected)
    {
        if (m_selected->m_level % 10 == 0)
            return m_selected->m_level;
        else
            return m_selected->m_level - (m_selected->m_level % 10) + 10;
    }
    return 10;
}