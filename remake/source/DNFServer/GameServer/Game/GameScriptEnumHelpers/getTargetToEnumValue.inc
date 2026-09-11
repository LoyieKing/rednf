// ---------------------------------------------------------------------------
// getTargetToEnumValue — ORIG @ 0x88c2c34，10 个分支 → 0..9，
// 全部未命中时 ORIG 尾部 mov $0xa,%eax：默认返回 10。
// ---------------------------------------------------------------------------
// ORIG @ 0x88c2c34：10 个 if 链（s == 字面量），非查表循环。
int getTargetToEnumValue(std::string& s)
{
    if (s == "myself")
        return 0;
    if (s == "enemy")
        return 1;
    if (s == "party")
        return 2;
    if (s == "random party")
        return 3;
    if (s == "all enemy")
        return 4;
    if (s == "enemy hp max")
        return 5;
    if (s == "enemy hp min")
        return 6;
    if (s == "enemy near")
        return 7;
    if (s == "enemy far")
        return 8;
    if (s == "random enemy")
        return 9;
    return 10; // ORIG 默认分支
}