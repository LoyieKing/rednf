// ORIG T 0x080e4892：bit>7 或 byte>0xf → 0；否则返回 !(flag bit set)
// [b28d-solo-CCharacInformNotice] 判词 AE(33=33)：P56 if-return 形修复——ORIG 尾部为
//   `xor$1,%eax; test%al,%al; je; mov$1; jmp; mov$0`（bool 归一 if-return 岛），
//   原先 `return !expr;` 只发 xor 直返（28 条）⇒ 改 `if(!expr) return true;
//   return false;` 后逐条对齐（.o 残余仅重定位分支目标显示，P264）。
bool CCharacInformNotice::checkInformNoticeFlag(unsigned int bit, unsigned int byteIdx)
{
    if (bit > 7)
        return false;
    if (byteIdx >= InformNotice::kInformNoticeIndex)
        return false;
    // [b28c-L15 P0/N96] 存量 register asm("eax") 绑定已清除（全域禁绑，N96）。
    unsigned char v = m_flags[byteIdx];
    if (!(((int)v) >> bit & 1))
        return true;
    return false;
}