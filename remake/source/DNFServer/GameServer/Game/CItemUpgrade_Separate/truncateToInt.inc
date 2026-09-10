int truncateToInt(float value)
{
    // ORIG：fnstcw/fldcw 0xc 置截断后 fistp（含 64 位变体取低 dword）。
    return static_cast<int>(value);
