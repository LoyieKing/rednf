// ---------------------------------------------------------------------------
// PI-2nd 加密轮函数域（ORIG 0x080b46xx/0x082744a4）—— ORIG TU = SecurityServer.cpp。
// ===========================================================================
// pi_2nd_print_bin  ORIG T 0x082744a4（本文件首强实体 ⇒ GI 键名）
// ===========================================================================
void pi_2nd_print_bin(char* data, short count)
{
    // b28c-l23 根因翻案（旧"双槽不可翻"判词作废）：ORIG @0x082744aa 只出**一条**
    // 形参 spill（`mov 0xc(%ebp),%eax; mov %ax,-0x1c(%ebp)`）后 `cmpw $0x64,-0x1c`
    // 内存直比 ⇒ 原码是**直接 clamp 形参本身**（P77 形参直用）；我方引入具名
    // `short n` 使 4.4 多出 spill+`movzwl/mov %ax` 双槽物化（帧 0x48 vs 0x38）。
    // buf 侧 ORIG 为两条 dword 清零（`movl $0x0,-0x18/-0x14`）⇒ `char buf[8] = {0}`
    // （P202①/P214③：显式零初始化复现 4.1 逐字清零，第三条 -0x10 是 for 的 i）。
    if (count > 0x64)
        count = 0x64;
    char buf[8] = {0};
    for (int i = 0; i < count; ++i)
    {
        unsigned char v = static_cast<unsigned char>(data[i]);
        sprintf(buf, "%02x", v);
        printf("%s ", buf);
    }
}