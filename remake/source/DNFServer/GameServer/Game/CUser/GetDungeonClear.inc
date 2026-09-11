// ORIG 0x0864de58 _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0864de58 | _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
bool CUser::GetDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig) const
{
    sig->m_selected = 0;
    // b28d-lane5：ORIG 0x0864de5f-0x0864de6d 无 cur 槽——`call getCurCharacR;
    // test %eax,%eax; sete %al; test %al,%al; je <body>`（P197④ call 结果寄存器
    // 直比产 sete）+ trace 块 fall-through、`return false` 常量岛。具名 cur 局部
    // 反产 `mov %eax,-0xc; cmpl $0,-0xc` 槽比形（P0 槽-局部双向对应律）。
    if (getCurCharacR() == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1400, 5)
            ("CUser::GetDungeonClear User  m_selected is null");
        return false;
    }
    getDungeonClearRefR()->toString(sig->m_clearData, 0x1800);
    return true;
}