// ============================================================================
// df_game_r 全局位工具 + SameBuffer（ORIG 自由函数，objdump 直转）。
// ----------------------------------------------------------------------------
// ORIG 符号（df_game_r，全部 T 强符号，同一工具区域 0x088921c2-0x0889253a）：
//   setBit_Byte(unsigned char&, bool, int)   _Z11setBit_ByteRhbi  0x088921c2
//   isSetBit_Byte(unsigned char, int)        _Z13isSetBit_Bytehi  0x0889221f
//   setBit_Word(unsigned short&, bool, int)  _Z11setBit_WordRtbi  0x0889224a
//   isSetBit_Word(unsigned short, int)       _Z13isSetBit_Wordti  0x088922a9
//   SameBuffer(unsigned char*, unsigned long, unsigned char*)
//                                           _Z10SameBufferPhmS_   0x088924f1
// isSetBit_Uint / setBit_Uint（0x0889232e / 0x088922d7）已由 BingoEventLink4.cpp
// 定义，本 TU 不再重复。
// ----------------------------------------------------------------------------
// 反汇编形态：
//   setBit_Byte：bool 形参 spill（cmpb $0）后两分支各自 movzbl 读 → shl → or/not+and
//                → movb 写；isSetBit_*：mov $1 → shl %cl → and → test → setne；
//   SameBuffer：i 无符号（movl $0 + setb 循环界），不等即返回 0，等完返回 1。
// ============================================================================

void setBit_Byte(unsigned char& value, bool set, int bit)
{
    if (set)
    {
        value = static_cast<unsigned char>(value | (1u << bit));
    }
    else
    {
        value = static_cast<unsigned char>(value & ~(1u << bit));
    }
}

bool isSetBit_Byte(unsigned char value, int bit)
{
    unsigned char mask = static_cast<unsigned char>(1u << bit);
    return (value & mask) != 0;
}

void setBit_Word(unsigned short& value, bool set, int bit)
{
    if (set)
    {
        value = static_cast<unsigned short>(value | (1u << bit));
    }
    else
    {
        value = static_cast<unsigned short>(value & ~(1u << bit));
    }
}

bool isSetBit_Word(unsigned short value, int bit)
{
    unsigned short mask = static_cast<unsigned short>(1u << bit);
    return (value & mask) != 0;
}

bool SameBuffer(unsigned char* a, unsigned long size, unsigned char* b)
{
    for (unsigned long i = 0; i < size; ++i)
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}
