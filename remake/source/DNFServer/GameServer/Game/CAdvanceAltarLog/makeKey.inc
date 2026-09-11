// ORIG makeKey @0x08157298：低 32 = itemIndex，高 32 = (itemType<<16)|keyC
// （ORIG 汇编实证：组合 (itemType<<16)|keyC 后 sar $0x1f 符号扩展为 64 位；
// 高 32 位即该组合的有符号扩展——与低 32 位 itemIndex 拼合）。
long long CAdvanceAltarLog::makeKey(unsigned short itemType,
                                    unsigned int itemIndex,
                                    unsigned short keyC)
{
    return (static_cast<long long>(
                static_cast<int>((itemType << 16) | keyC))
            << 32) |
           static_cast<long long>(itemIndex);
}