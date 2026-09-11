WongWork::ENUM_HACKTYPE Secu_AccountHacking::getMatchingHacktype(
    AccountHackingType::T t)
{
    // ORIG W 0x0827912a（getMatchingHacktype.md）：线性 cmpl/jne 映射链。
    if (t == AccountHackingType::GOLD_0) return (WongWork::ENUM_HACKTYPE)0x2c6;
    if (t == AccountHackingType::GOLD_1) return (WongWork::ENUM_HACKTYPE)0x2c7;
    if (t == AccountHackingType::GOLD_2) return (WongWork::ENUM_HACKTYPE)0x2c8;
    if (t == AccountHackingType::GOLD_3) return (WongWork::ENUM_HACKTYPE)0x2c9;
    if (t == AccountHackingType::AVATAR_COUNT) return (WongWork::ENUM_HACKTYPE)0x2ca;
    if (t == AccountHackingType::AVATAR_TIME) return (WongWork::ENUM_HACKTYPE)0x2cb;
    return (WongWork::ENUM_HACKTYPE)0xfa1;
}