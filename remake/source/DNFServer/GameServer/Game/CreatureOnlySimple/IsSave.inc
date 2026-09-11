// 【objdump】ORIG _ZN13user_creature4CEgg6IsSaveEv W 0x0833f102（size 0x0a）：
// push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— ORIG 符号字面
// 返回 0（蛋条目默认不可保存态；全二进制无此 W 符号的 call 引用，仅 linkonce
// 导出）。按指令忠实转写，非编造固定桩。
bool CEgg::IsSave()
{
    return false;
}