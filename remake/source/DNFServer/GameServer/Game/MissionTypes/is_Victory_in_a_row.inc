// ORIG 0x8a5aa4e（is_Victory_in_a_row.md）：连胜判定——mask=(1<<victory)-1，
// (bits & mask) == mask（低 victory 位全置位）。返回 bool：末尾直透
// operator== 的 al 结果，无 setne/movzbl 规范化（P9）。
// P112 封顶（b26 Lane27）：44 条指令/助记符全等；仅 3 个 bitset 局部槽位
// 非单调分配差（4.1 maskA@-0x14/maskB@-0xc/res@-0x10 vs 4.4 声明序
// -0xc/-0x10/-0x14），声明序换位会使 ctor 调用序反向，无合法源形，封顶。
// [b28c-L23 N93附 复测维持] 三枚 bitset 局部分配方向跨版本相反：4.1 先声明占**更深**
// 槽（maskA -0x14 / maskB -0xc / 结果 -0x10），4.4 先声明占**更浅**
// （-0xc/-0x10/-0x14）。本件已是「全头部声明」形（P212 前置条件满足仍不翻正），且
// 声明序绑死构造序（ORIG ctor 序 = maskA,maskB,operator&），翻转声明即翻转指令序
// ⇒ 无合法源形。44=44、助记符序列全等，仅 3 处 lea 槽号差。
// [b28d-23 N·N93附] 三枚 bitset 局部分配方向跨版本相反（4.1 先声明占深槽 -0x14/-0xc/-0x10 vs 4.4 -0xc/-0x10/-0x14），声明序绑死构造序 ⇒ N。ORIG 0x08a5aa4e
bool is_Victory_in_a_row(std::bitset<32> bits, short victory)
{
    std::bitset<32> maskA((1 << victory) - 1);
    std::bitset<32> maskB((1 << victory) - 1);
    std::bitset<32> res = bits & maskB;
    return res == maskA;
}