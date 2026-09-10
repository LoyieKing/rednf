unsigned short CUser::getCurCharacTotalMaxFatigue() const
{
    // ORIG 0x86??：`movw $0,-0xa` dead init + call 结果 `mov %ax,-0xa` 双写（技巧 8）；
    // rate 无槽（register asm("ax")，call 结果直落 test;setne;test;je 物化）。
    unsigned short total = 0;
    total = getCurCharacMaxFatigue();
    // ORIG 0x8??：`test %ax,%ax; setne %al; test %al,%al; je`（技巧 22：register bool
    // 结果 !=0 物化）。b28d-solo-CUser AE：条件内联免 bool 槽（probe_bool a3/a5）。
    if (m_premium.GetAdvantageFatigueRate() != 0)
        total = (unsigned short)(total + getCurCharacMaxPremiumFatigue());
    return total;
