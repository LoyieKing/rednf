// ---------------------------------------------------------------------------
// ORIG 全局位工具（source 无既有 producer，objdump 直转）：
//   _Z13isSetBit_Uintji T 0x0889232e：(value & (1<<bit)) != 0
//   _Z11setBit_UintRjbi T 0x088922d7：set ? |= : &=~
bool isSetBit_Uint(unsigned int value, int bit)
{
    // ORIG: 恒寄存器 `1<<bit`（int 移位）并内联，无具名局部槽；`1u` 会差成 `shr`+分开。
    return (value & (1 << bit)) != 0;
}