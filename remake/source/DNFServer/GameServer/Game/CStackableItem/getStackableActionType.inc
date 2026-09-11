// ===================== StackableAction 字符串→枚举映射（ORIG T） =====================
// getStackableActionType @0x089aeb9b：14 项字符串→Type 映射，默认 0（if-return 链，
// 每项 call eq/test/je/mov+jmp 统一出口，与 ORIG 同形）。
int getStackableActionType(std::string& type)
{
    if (type == "[megaphone]")
    {
        return 0x16;
    }
    if (type == "[upgrade cargo]")
    {
        return 0x2b;
    }
    if (type == "[chatemoticon]")
    {
        return 0x2c;
    }
    if (type == "[seria room deco]")
    {
        return 0x2a;
    }
    if (type == "[open guide book]")
    {
        return 0x2d;
    }
    if (type == "[class change]")
    {
        return 0x2e;
    }
    if (type == "[character name change]")
    {
        return 0x33;
    }
    if (type == "[bingo]")
    {
        return 0x34;
    }
    if (type == "[portable upgrade]")
    {
        return 0x35;
    }
    if (type == "[add fatigue]")
    {
        return 0x37;
    }
    if (type == "[candle golden wax]")
    {
        return 0x07;
    }
    if (type == "[add avatar socket]")
    {
        return 0x20;
    }
    if (type == "[LotteryCount]")
    {
        return 0x36;
    }
    if (type == "[equipment purify]")
    {
        return 0x21;
    }
    return 0;
}