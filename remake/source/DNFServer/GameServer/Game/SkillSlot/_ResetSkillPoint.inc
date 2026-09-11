void CSkillChanger::_ResetSkillPoint(CUser* user, ENUM_SKILL_TREE_KIND kind)
{
    // b28d-solo-SkillSlot 判词（AE 40=40）：修法 = 加法右操作数 `(int)` 收窄。
    // GetSpAtLevelUp 返回 unsigned int，与左操作数 int 异型时 4.4 发
    // `lea (%ebx,%eax,1),%eax; mov %eax,%ebx`（+1 条）；ORIG 4.1 为 `add %eax,%ebx`。
    // 调用点 `(int)G_CDataManager()->GetSpAtLevelUp(level)` 使两端同型 ⇒ 逐条 AE
    // （40=40，助记符/寄存器/槽全等；.o 层 call/jmp 重定位标签差为 P264 假报）。
    // 原 b28d-36（修法 P0+P52+P134）：ORIG 0x0860a558 无 charLevel/sp/spUp/slotR/slotW
    // 具名槽（-0x18/-0x14/-0x10/-0xc 全无）——remain 值全程 %ebx 直传、跨
    // getCurCharacSkillW 调用保活 ⇒ 原码是单条嵌套实参表达式 + for 尾条件 setge
    // 物化。槽往返版多 5 个槽（our 45→40 insn）。
    for (int level = 1; level <= user->get_charac_level(); ++level)
    {
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())
            ->set_remain_sp_at_index(
                reinterpret_cast<const SkillSlot*>(user->getCurCharacSkillR())
                        ->get_remain_sp_at_index(kind)
                    + (int)G_CDataManager()->GetSpAtLevelUp(level),
                kind);
    }
}