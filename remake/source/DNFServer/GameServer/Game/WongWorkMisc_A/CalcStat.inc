_Additioal_info* CCalcCharacStat::CalcStat(_Additioal_info* out, int job,
                                           int firstGrow, int secondGrow,
                                           int level)
{
    out->clear();
    // ORIG 四次独立 call G_CDataManager + mov +0x14 + imul $0x7dc 重算行地址
    // （CalcStat.md +0x011/+0x034/+0x063/+0x090），故逐处表达式访问、不缓存行引用。
    out->add(G_CDataManager()->m_characList->m_characs[job].m_baseInfo0);

    const _Additioal_info* pFirst = G_CDataManager()->m_characList
                                        ->m_characs[job]
                                        .get_growtype_stat(0, 0);
    const _Additioal_info* pSecond = G_CDataManager()->m_characList
                                         ->m_characs[job]
                                         .get_growtype_stat(firstGrow, 0);
    const _Additioal_info* pThird = G_CDataManager()->m_characList
                                        ->m_characs[job]
                                        .get_growtype_stat(firstGrow, secondGrow);
    for (int lv = 1; lv < level; ++lv)
    {
        if (lv <= 0xe)                     // cmpl $0xe,jg→else-if（即 lv<15）
            out->add(*pFirst);
        else if (lv <= 0x31)               // cmpl $0x31（即 lv<50）
            out->add(*pSecond);
        else
            out->add(*pThird);
    }
    return out;
