bool CHackAnalyzer::CheckHackRateByDifficult(ENUM_HACKTYPE type, int lvDiff,
                                             int difficult)
{
    // ORIG 0x080f842e：std::find 查 m_vec7a30(+0x7a30)；未命中返回 true；
    // type==0x86 时 lvDiff<<1；difficult 越界返回 true；chance=100-lvDiff-表值，
    // <=0x18 且非 0x86 时抬到 0x19；命中当 get_rand_int(100) <= chance。
    std::vector<int>::const_iterator it = std::find(m_vec7a30.begin(),
                                                    m_vec7a30.end(), (int)type);
    if (it == m_vec7a30.end())
        return true;

    if ((int)type == 0x86)
        lvDiff = lvDiff << 1;
    int chance = 100 - lvDiff;   // ORIG 初始化在界检查前，-= 表值在检查后
    if (difficult < 0 || difficult > 4)
        return true;
    chance -= _S_HACK_CHECK_RATE_BY_DIFFICULT[difficult];
    if (chance <= 0x18 && (int)type != 0x86)
        chance = 0x19;
    // ORIG 尾段 if 常量岛（setle 后 je mov$1/jmp/mov$0 块形，非 setcc 直返）。
    if (get_rand_int(100) <= chance)
        return true;
    return false;
