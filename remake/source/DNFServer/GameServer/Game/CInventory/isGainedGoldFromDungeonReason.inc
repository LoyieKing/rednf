bool isGainedGoldFromDungeonReason(eMoneyAddReason reason)
{
    // ORIG 0x867d9a2：switch 形态（先 mov 参数到寄存器再 cmp/je 链）。
    switch ((int)reason)
    {
    case 4:
    case 0xf:
        return true;
    }
    return false;
