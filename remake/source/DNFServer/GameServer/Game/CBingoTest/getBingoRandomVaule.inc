int CBingo::getBingoRandomVaule(int param_1)
{
    unsigned long range = 9;
    int iVar1 = m_rand->randInt(range) + 1;
    iVar1 = iVar1 - 10 + param_1;
    return (iVar1 <= 0 || iVar1 > 0x32) ? 0 : iVar1;
