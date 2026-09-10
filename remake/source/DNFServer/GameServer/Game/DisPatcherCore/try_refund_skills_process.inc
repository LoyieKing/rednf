void DisPatcher_BuySkill::try_refund_skills_process(CUser* user, char /*treeKind*/,

                                                    buySkillInfo& info)
{
    // [DPC_CheckErr 实测负结论 2026-09-11] 试形「具名形参轴」已回滚：ORIG
    // 0x081bfd45-0x081bfd48 `mov 0x10(%ebp),%eax; mov %al,-0x2c(%ebp)` 为 char
    // 形参 treeKind 建 -0x2c 截断槽（全函数未再读），但形参改回具名 `char treeKind`
    // （body 不引用）后父泳道 fresh 核账 our_insns **不变**（59/58 仍 58）⇒ 本 TU 的
    // 4.4 后端不为未使用具名窄形参发截断 store（单文件 /tmp 实验会发，TU 旗标下不发）
    // ⇒ 该 store 属 4.1 独有物化，N 封顶。形参保持无名；若判定 ndiff 改善可参考
    // /tmp/dpc_after/try_refund_skills_process.inc 重新落地。
    int refundCount = 0;                                                              // -0x1c
    int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));        // -0x38
    int secondGrow = static_cast<int>(user->getCurCharSecondGrowType());        // -0x34
    int firstGrow = static_cast<int>(user->getCurCharFirstGrowType());          // -0x30
    // [DPCB 实验 2026-09-11] P-L11 实参全内联：ORIG 三个活值(job/level/itemIdx)均
    // **无槽**（全在 callee-saved %esi/%ebx/%edi），且求值序 job(0x081bfd4b call,
    // →%esi) < level(0x081bfd58, →%ebx) < itemIdx(0x081bfd65 `movzbl %al,%edi`) <
    // skillW(0x081bfd6e)。把三个具名 register 局部改回实参位内联表达式（从右到左
    // 求值给出同一程序序）。若读数不变/变劣立即回滚。
    info.m_4 =
        ((SkillSlot*)(user->getCurCharacSkillW()))
            ->try_refund_skill(static_cast<unsigned char>(info.m_0),
                               user->get_charac_level(), user->get_charac_job(),
                               firstGrow, secondGrow, addLevel, refundCount);
}
