char const* MobileRestrict::enumToString(Enum e)
{
    // ORIG 0x0816d463：无边界检查，直接按 Enum 查表（表 35 项）。
    return enumStringMobiles[e];
}