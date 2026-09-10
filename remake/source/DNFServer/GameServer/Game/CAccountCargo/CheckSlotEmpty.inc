bool CAccountCargo::CheckSlotEmpty(int slot)
{
    // P168a ① 裸形（CheckValidSlot 返回 bool）：ORIG `xor $1; test %al,%al;
    // je BODY; mov $0,%eax; jmp end; BODY: …` —— return 0 块在前、命中路径跳走。
    if (!CheckValidSlot(slot))
        return 0;
    // [L8 2026-09-10 P56/P47] ORIG 0x0828a607 段为**分支形态**（`test %eax,%eax;
    // jne L; mov $0x1,%eax; jmp end; L: mov $0x0,%eax`）；三目 `? 1 : 0` 4.4 恒折
    // `sete %al`（ndiff=5 的唯一残差）⇒ 改写 if-return 两条。
    if (m_slots[slot].m_addInfo == 0)
        return 1;
    return 0;
