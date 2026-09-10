bool isSetBit_Uint(unsigned int value, int bit)
{
    // ORIG: 恒寄存器 `1<<bit`（int 移位）并内联，无具名局部槽；`1u` 会差成 `shr`+分开。
    return (value & (1 << bit)) != 0;
