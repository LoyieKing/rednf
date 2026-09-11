// ---- set_party_index @0x082697c8：定位写，idx∈[0,3] 才生效 ----
void CQuickParty::set_party_index(int idx, int partyIdx)
{
    // b28c-lane3：P223 双空块岛复现。ORIG 0x082697cf `jg +0x16`→nop;jmp 岛、
    // 0x082697d5 `js +0x11`→nop 岛、成功路径尾 `jmp +4`；4.4 对
    // `if (idx < 4 && idx >= 0)` 恒 jump-thread 掉两岛（12 vs 14）。
    if (idx > 3)
        goto L1;
    if (idx < 0)
        goto L2;
    m_partyIndex[idx] = partyIdx;
    goto L3;
L1: ;
L2: ;
L3: ;
}