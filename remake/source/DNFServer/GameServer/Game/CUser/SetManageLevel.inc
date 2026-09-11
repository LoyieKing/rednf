// ============================================================================
// ORIG T 0x0868fe2c _ZN5CUser14SetManageLevelEi：超上限取脚本 m_manageLevelMax。
// ============================================================================
// b28d-solo-CUser AE: ORIG 0x0868fe32-0x0868fe61 `cmp slot,%eax; setl %al; test %al,%al; je`
//   无 bool 槽（sub $0x8）；具名 less 落槽（-0x9）⇒ 删局部内联（probe_bool a6/a7 对照）。
//   条件/体各完整重估 G_CDataManager() 链（P183）。| _ZN5CUser14SetManageLevelEi
void CUser::SetManageLevel(int level)
{
    if (G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax < level)
        m_field8ebd0 = G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax;
    else
        m_field8ebd0 = level;
}