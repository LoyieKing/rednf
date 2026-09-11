// ---------------------------------------------------------------------------
// CUser::SendItemMakingSkillInfo() T 0x0868b00a sz0x39
// 报告：docs/class_func_reports/CUser/SendItemMakingSkillInfo.md
// GetCharacExpandDataR(this,10)（CUser.h:2715 声明，返回 CExpandEquipslot*，
// ORIG Ghidra 视图即 CItemMakingSkill* 槽）→ 非空则 CItemMakingSkill::
// Send_ItemMakingSkillList(this)。
// ---------------------------------------------------------------------------
void CUser::SendItemMakingSkillInfo()
{
    CItemMakingSkill* skill =
        reinterpret_cast<CItemMakingSkill*>(GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)10));
    if (skill != 0) {
        skill->Send_ItemMakingSkillList(this);
    }
    return;
}