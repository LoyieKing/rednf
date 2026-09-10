void CUser::OnPunishUser()
{
    // ORIG 0x0868137a `test %eax,%eax; setne %al; test %al,%al; je +0x50(epilogue)`：
    // !=0 物化形 + body fall-through，尾无 jmp 岛（==0 return 形反产 sete/jne/jmp+nop）。
    if (getCurCharacR() != 0)
    {
        // ORIG 0x08681396 `mov %eax,%ebx`：get_money 结果跨
        // GetInstanceCirculationStatistic 调用存活于 callee-saved ebx（技巧2/P52）。
        // [b28d-solo-CUserOnlyOrig P0/P24] ORIG 无 money 槽 ⇒ 原码是实参内联表达式；
        // 具名局部恒 spill -0xc 槽并收紧帧 sub $0x28（ORIG push ebx+sub $0x14）。
        GetInstanceCirculationStatistic()->AddCirculationStatistic(
            (CIRCULATION_STATISTIC_FIELD)0x1c, this,
            getCurCharacInvenR()->get_money());
    }
