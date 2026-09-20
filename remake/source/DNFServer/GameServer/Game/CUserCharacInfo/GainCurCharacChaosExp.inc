void CUserCharacInfo::GainCurCharacChaosExp(int exp)
{
    // P63 正极性包裹：否定早返 `if(!sel||cp<=0)return` 使 4.4 为两条短路测试
    // 各发空 else 桩（nop;jmp 中继 ×2，+4 条）；正极 `if(sel&&cp>0){主流程}`
    // 时 je/jle 直达尾 epilogue，与 ORIG 0x08645e5e 一致。
    if (m_selected && m_selected->m_fielde49 > 0)
    {
        m_selected->m_fielde4d += exp;
        int percent = G_CDataManager()->get_level_exp_percent(get_charac_level(), 1);
        if (m_selected->m_fielde4d > percent && percent > 0)
        {
            enableSaveCharacStat();
            m_selected->m_fielde49 -= m_selected->m_fielde4d / percent;
            m_selected->m_fielde4d %= percent;
            if (m_selected->m_fielde49 < 0)
            {
                m_selected->m_fielde49 = 0;
                m_selected->m_fielde4d = 0;
            }
        }
    }
}