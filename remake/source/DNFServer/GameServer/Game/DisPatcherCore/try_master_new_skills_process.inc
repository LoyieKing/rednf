void DisPatcher_BuySkill::try_master_new_skills_process(CUser* user, char treeKind,

                                                        buySkillInfo& info)
{
    // [DPC_CheckErr 负结论 2026-09-11] 本件 ORIG 84 / 我方 82（我方少 2，负 gap）。
    // 逐段核对：ORIG 独有 = 0x081bfe4b 与 0x081bfedd 两条 `nop`（4.1 分支块对齐填充，
    // 落点分别为 if/else 的显式空岛与尾块）+ 0x081bfe05/0x081bfe0f 的 `cmp $0xc7,%al`
    // / `cmp $0xd0,%al` 8 位比较（4.4 同样产 8 位比较）。nop 填充由分支布局/对齐启发式
    // 决定，无源码形可造 ⇒ N 封顶。判据链（treeKind==1 极性、GetCurCharacSkillTreeIndex
    // 的 -1/0 双判）与 ORIG 0x081bfe17-0x081bfe49 已逐条对齐。
    if ((unsigned int)info.m_0 >= 200 && (unsigned int)info.m_0 <= 208)
    {   // 公会技能 [200,208]：直接返回（ORIG cmp $0xd0 jbe → 尾声）
        return;
    }
        // [DpcMisc-P8 负结论 2026-09-11，已回滚] ORIG 0x081bfe22-0x081bfe32 为
        // 「store + 两次 load→cmp」两段形（`mov %eax,-0x1c(%ebp)` →
        // `mov -0x1c(%ebp),%eax; cmp $0xffffffff,%eax; je` →
        // `mov -0x1c(%ebp),%eax; test %eax,%eax; jne`），4.4 恒折成
        // `cmpl $imm,-0x28(%ebp)` 单段 ⇒ 本件 gap −1/−2 的成因之一。
        // 试形 `volatile int treeIdx`：父泳道 fresh 核账 **83→88（+5 超调，|84−88|=4）**
        // —— volatile 强制每次访问落内存并阻止折叠，额外 load 远多于目标 2 条，
        // 属失真写法 ⇒ 回滚。该两段形归 N（4.4 前端折叠，源码形无自由度）。
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
