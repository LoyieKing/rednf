char const* SecurityRestrict::enumToString(Enum e)
{
    // ORIG 0x0816d481：无边界检查，直接按 Enum 查表（同 MobileRestrict 形态）。
    return enumStringSecurity[e];
