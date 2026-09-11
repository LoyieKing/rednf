// ORIG getLevel @0x08157336：shld+shl 组合 (key<<16) 后取高 32 位 sar 16
// —— 即 (int)((key<<16)>>48) = 高 32 位（(itemType<<16)|keyC）的低 16 位（keyC）。
long long CAdvanceAltarLog::getLevel(long long key)
{
    return (key << 0x10) >> 0x30;
}