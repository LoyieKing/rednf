// ============================================================================
// ORIG T 0x08664ae8 sz 0x167 _ZN5CUser13calc_lev_statER15_Additioal_info：
//   基础块 add 后按当前等级循环叠加成长属性块（≤14 首转/≤49 二转/之后三转）。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08664ae8 | _ZN5CUser13calc_lev_statER15_Additioal_info
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
    // b28d-GrpD 按 ORIG 0x08664ae8 直证：ORIG 第三个 get_growtype_stat 的第二实参
    //   直接来自 `call getCurCharSecondGrowType; movsbl %al,%esi`（+0x0b7），全程
    //   **无 secondGrow 具名槽**（我方旧 `int secondGrow` 多占 -0x14 ⇒ 帧 0x30 vs
    //   ORIG 0x2c、且三个指针槽整体下移 4：我方 -0x1c/-0x18/-0x10 vs ORIG
    //   -0x28/-0x24/-0x20）。删除具名量、实参内联（P329/P183）。
    const void* statSecond = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()].get_growtype_stat(
            (signed char)getCurCharFirstGrowType(),
            (signed char)getCurCharSecondGrowType());                   // 二转

    for (int level = 1; get_charac_level() > level; ++level)
    {
        if (level <= 14)
            info.add(*(const _Additioal_info*)statBase);
        else if (level <= 49)
            info.add(*(const _Additioal_info*)statFirst);
        else
            info.add(*(const _Additioal_info*)statSecond);
    }
}