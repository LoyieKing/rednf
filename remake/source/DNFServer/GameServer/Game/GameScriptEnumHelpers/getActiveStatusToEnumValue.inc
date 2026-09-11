// ---------------------------------------------------------------------------
// getActiveStatusToEnumValue — ORIG @ 0x88c29a2，20 个 strcmp 分支 → 0..19，默认 -1。
// ---------------------------------------------------------------------------
int getActiveStatusToEnumValue(std::string& s)
{
    // ORIG 0x88c29a2：20 个 if 链 strcmp 分支 → 0..19，默认 -1（非循环查表）。
    if (s == "slow")        return 0;
    if (s == "freeze")      return 1;
    if (s == "poison")      return 2;
    if (s == "stun")        return 3;
    if (s == "curse")       return 4;
    if (s == "blind")       return 5;
    if (s == "lightning")   return 6;
    if (s == "stone")       return 7;
    if (s == "sleep")       return 8;
    if (s == "burn")        return 9;
    if (s == "weapon break") return 10;
    if (s == "bleeding")    return 11;
    if (s == "haste")       return 12;
    if (s == "bless")       return 13;
    if (s == "element")     return 14;
    if (s == "confuse")     return 15;
    if (s == "hold")        return 16;
    if (s == "armor break") return 17;
    if (s == "any")         return 18;
    if (s == "all")         return 19;
    return -1;
}