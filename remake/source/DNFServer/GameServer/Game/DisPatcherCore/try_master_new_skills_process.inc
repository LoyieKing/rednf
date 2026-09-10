void DisPatcher_BuySkill::try_master_new_skills_process(CUser* user, char treeKind,

                                                        buySkillInfo& info)
{
    if ((unsigned int)info.m_0 >= 200 && (unsigned int)info.m_0 <= 208)
    {   // 公会技能 [200,208]：直接返回（ORIG cmp $0xd0 jbe → 尾声）
        return;
    }
        int treeIdx = user->GetCurCharacSkillTreeIndex();
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (treeKind == 1)
            {
                return;
            }
        }
        else if (treeKind != 1)
        {
            return;
        }
        int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));    // -0x38
        int secondGrow =
            static_cast<int>(user->getCurCharSecondGrowType());                       // -0x34
        int firstGrow =
            static_cast<int>(user->getCurCharFirstGrowType());                        // -0x30
        // [b28e-L5] P52/P290/P200：ORIG 0x081bfe86..0x081bfed2 把 (job→%esi,
        // level→%ebx, (byte)info.m_0→%edi) 物化入 callee-saved 后跨
        // getCurCharacSkillW call 保活（`push %edi/%esi/%ebx; sub $0x4c`）；拆具名
        // job/level/skillW 局部会使 4.4 三值各占栈槽（帧 0x4c→0x58、丢三 push）。
        // 链式单表达式的 receiver 置于末尾求值，实参位相应对齐 ORIG 装载序。
        info.m_4 = ((SkillSlot*)(user->getCurCharacSkillW()))->try_master_new_skills(
            static_cast<unsigned char>(info.m_0), user->get_charac_level(),
            user->get_charac_job(), firstGrow, secondGrow, addLevel);
}
