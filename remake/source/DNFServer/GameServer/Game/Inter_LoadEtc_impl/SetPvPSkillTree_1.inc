// ORIG T 0x084bff8e sz 0x2d5（_Z15SetPvPSkillTreeP5CUserPK15_Mastered_skill20ENUM_SKILL_TREE_KIND，
// 报告 docs/class_func_reports/_lt_global_gt_/SetPvPSkillTree.md 全文已读，
// Ghidra C 与汇编双段对照）。依赖 getGivePvPSkillPoint（定义由 Main 分派）。
void SetPvPSkillTree(CUser* user, const _Mastered_skill* skill,
                     ENUM_SKILL_TREE_KIND kind)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=AE 200/200（fresh，0 差异块）：p167 CLEAR（33/33）。
    // 本轮已修：去 skillR/skillW 缓存（P0/P183）+ giveIdx 预清（P198①）+ **两处
    // givePoint 去具名局部改内联条件表达式**（P0：ORIG `call; test; setne; test; je` 无槽）
    // + 第二处 P272 正写（!= 0）使 then 直落、givePvPSkillTree 落尾。
    int usedSp = 0;     // calcUsedSP 出参1（ORIG -0x2c）
    int usedSfp = 0;    // calcUsedSP 出参2（ORIG -0x30，后续未读）
    // [b28d-solo-Inter_LoadEtc_impl] P0/P183：ORIG 无 skillR 缓存局部
    // （每处 getCurCharacSkillR 返回值寄存器直传实参槽）。
    reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
        ->calcUsedSP(skill, usedSp, usedSfp);

    // ORIG：grade/secondGrow/firstGrow/job 三个计算点各自重取（P48 去缓存）；
    // getCurCharacSkillR 亦两处调用（calcUsedSP 后 get_remain_sp_at_index 前重新 GetInBuffer）。
    int pvpPoint = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
        user->get_charac_job(),
        (int)(char)user->getCurCharFirstGrowType(),
        (int)(char)user->getCurCharSecondGrowType(),
        user->get_pvp_grade(), false);

    int remainSp = reinterpret_cast<SkillSlot*>(
        const_cast<char*>(user->getCurCharacSkillR()))
        ->get_remain_sp_at_index(kind);
    int avail = pvpPoint - (usedSp + remainSp);      // ORIG -0x20
    int giveIdx = 0;                                 // ORIG -0x1c（预清 `movl $0x0`）
    if (kind == -1 || kind == 0)
    {
        giveIdx = 1;
    }
    else
    {
        giveIdx = 2;
    }

    if (avail > 0)
    {
        // ORIG getGivePvPSkillPoint 实参压栈序：job, first, second, grade
        // [b28d-solo-Inter_LoadEtc_impl] P0：ORIG `call; test %eax,%eax;
        // setne %al; test %al,%al; je` 无栈槽往返 ⇒ 原码为内联条件表达式，
        // 非 `int givePoint` 具名局部（旧形 `mov %eax,-0x20; cmpl $0,-0x20`）。
        if (G_CDataManager()->m_pvpSkillTree.getGivePvPSkillPoint(
                user->get_charac_job(),
                (int)(char)user->getCurCharFirstGrowType(),
                (int)(char)user->getCurCharSecondGrowType(),
                user->get_pvp_grade()) != 0)
        {
            reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                ->set_remain_sp_at_index(remainSp + avail, kind);
        }
        else
        {
            user->givePvPSkillTree(0, false, giveIdx);
        }
    }
    else if (avail < 0)
    {
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
            ->clear_all_skills(kind);
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
            ->set_parent(user);
        // 同上（P0）：第二处亦为内联条件表达式。
        // [b28d-solo-Inter_LoadEtc_impl] P272/P222：ORIG `setne %al; test; je <then>`
        // ⇒ then 体（set_remain_sp）直落、givePvPSkillTree 落尾 ⇒ 条件取 != 0 正写。
        if (G_CDataManager()->m_pvpSkillTree.getGivePvPSkillPoint(
                user->get_charac_job(),
                (int)(char)user->getCurCharFirstGrowType(),
                (int)(char)user->getCurCharSecondGrowType(),
                user->get_pvp_grade()) != 0)
        {
            reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
                ->set_remain_sp_at_index(pvpPoint, kind);
        }
        else
        {
            user->givePvPSkillTree(0, false, giveIdx);
        }
    }
    user->givePvPSkillTree(0, true, giveIdx);
}