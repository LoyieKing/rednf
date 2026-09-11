Inven_Item CAccountCargo::GetSlot(int slot)
{
    // [b28d-solo-CAccountCargo] ORIG 0x082898f8：`call CheckValidSlot; xor $1;
    // test %al; je <valid 块>` ⇒ 否定 guard 早返，invalid 块 fall-through 在前、
    // valid 块在后（P59/P168a①）。原 if(valid) 正极性令 4.4 产 `test; je <invalid>`
    // （invalid 后置）块序相反。
    if (!CheckValidSlot(slot)) {
        Inven_Item out;
        return out;
    }
    return m_slots[slot];
}