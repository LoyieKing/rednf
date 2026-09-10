void CAccountCargo::ResetSlot(int slot)
{
    // [b28c-11] AE（i486 旗标下）：P223 尾空标签 goto 形复现 ORIG `jne 尾 nop 岛`
    // + body 尾 jmp；已清历史 `register unsigned char v asm("eax")` 绑定（N96/P0）。
    // ORIG 0x082898c0：call CheckValidSlot; xor $1; test %al; jne <尾 nop 岛>;
    // body(call Inven_Item::reset); jmp end; end-1: nop。P223 尾空标签形复现
    // （禁 register asm 绑定，P0/N96）。
    if (!CheckValidSlot(slot))
        goto L_end;
    m_slots[slot].reset();
L_end:;
