Inven_Item* CAccountCargo::GetSlotRef(int slot)
{
    // 技巧 31：ORIG 帧 sub $0x18，4.4 裸形只给 0x8 ⇒ 4 个 dead int 撑满 16B
    //（无指令，仅占槽）。
    unsigned int g0, g1, g2, g3;
    (void)g0; (void)g1; (void)g2; (void)g3;
    // P168a ① 裸形（CheckValidSlot 返回 bool）：`if(!call()) return 0;`
    // 4.4 直产 `call; xor$1; test %al,%al; je BODY; mov $0x0,%eax; jmp end; BODY: …`
    // 与 ORIG 逐条同形（探针 /tmp/probeB.cpp ka 实证，无 nop 残差）。
    if (!CheckValidSlot(slot))
        return 0;
    m_alter = 1;
    return &m_slots[slot];
