void CUser::calc_lev_stat(_Additioal_info& info)
{
    // ORIG：G_CDataManager() 在基础 add/get_growtype_stat×(3) 四处各自重调，
    // getCurCharFirstGrowType 重调两次——访问器结果禁缓存（战术 P183/P48）。
    info.add(*(const _Additioal_info*)
        &G_CDataManager()->m_characList->m_characs[get_charac_job()]);

    const void* statBase = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()].get_growtype_stat(0, 0);          // 无成长
    const void* statFirst = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()]
            .get_growtype_stat((signed char)getCurCharFirstGrowType(), 0);  // 一转
    int secondGrow = (signed char)getCurCharSecondGrowType();
    const void* statSecond = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()].get_growtype_stat(
            (signed char)getCurCharFirstGrowType(), secondGrow);        // 二转

    for (int level = 1; get_charac_level() > level; ++level)
    {
        if (level <= 14)
            info.add(*(const _Additioal_info*)statBase);
        else if (level <= 49)
            info.add(*(const _Additioal_info*)statFirst);
        else
            info.add(*(const _Additioal_info*)statSecond);
    }
