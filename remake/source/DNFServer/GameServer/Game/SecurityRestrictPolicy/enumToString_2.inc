char const* CharacterRestrict::enumToString(Enum e)
{
    // ORIG 0x0816d472：无边界检查，直接按 Enum 查表（同 MobileRestrict 形态）。
    return enumStringCharacters[e];
