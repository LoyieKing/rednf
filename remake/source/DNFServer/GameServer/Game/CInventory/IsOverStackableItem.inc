// ORIG 0x850a088：entry 即 spill count(0xc,%ebp)->%al,-0x4（volatile 形参强制
// 槽落）。纯常量体会被 4.4 折叠掉 caller 的 call，volatile 挡折叠。
bool CInventory::IsOverStackableItem(int itemIdx, volatile unsigned char count)
{
    return true;
}