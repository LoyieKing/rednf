// ============================================================================
// cUserHistoryLog::SkillSPInfoSave @ 0x08686188
//   SkillSPInfoSave(CUser*, eSkillSPInfoSaveReason)
//   经 CSkillChanger::GetCurCharacTotalSP 计算总 SP；按当前技能树索引（-1 单树 /
//   其它双树）记录 "SkillSPInfo,%d,%d,%d,%d"（reason, remainSP, totalSP, treeKind）。
// ============================================================================
void cUserHistoryLog::SkillSPInfoSave(CUser* user, eSkillSPInfoSaveReason reason)
{
    // ORIG 语句顺序：changer 构造 → totalSP → buf 清零 → isSingleTree 判定。
    WongWork::CSkillChanger changer;
    int totalSP = changer.GetCurCharacTotalSP(user, false);

    // ORIG 0x08686188：buf1/buf2 各 0x80（rep stos $0x20 dwords，P116 计数口径）；
    // 单/双树判定不物化 bool（ORIG test/je+cmp/je 直落两分支体，P133 独立守卫形）。
    char buf1[0x80] = {0};
    char buf2[0x80] = {0};

    if (user != 0 && user->GetCurCharacSkillTreeIndex() == -1)
    {
        // ORIG 0x08686210：链式 receiver（getR 结果 eax 直转 this 实参，不落
        // slot——P52；命名 slot 局部会 spill+reload 各多 1 条）。
        int remain0 = reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR())
                          ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0);
        if (totalSP >= 0 && remain0 >= 0)
        {
            sprintf(buf1, "SkillSPInfo,%d,%d,%d,%d", (int)reason, remain0, totalSP, 0xffffffff);
            user->LogHistory(buf1);
        }
    }
    else
    {
        int remain0 = reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR())
                          ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0);
        int remain1 = reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR())
                          ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1);
        if (totalSP >= 0 && remain0 >= 0 && remain1 == 0)
        {
            sprintf(buf1, "SkillSPInfo,%d,%d,%d,%d", (int)reason, remain0, totalSP, 0);
            user->LogHistory(buf1);
            sprintf(buf2, "SkillSPInfo,%d,%d,%d,%d", (int)reason, remain1, totalSP, 1);
            user->LogHistory(buf2);
        }
    }
}