bool Secu_AccountHacking::isGoldPatten(AccountHackingType::T t)
{
    // ORIG W 0x08279258（isGoldPatten.md）：0..3 判金伪型。
    if (t == AccountHackingType::GOLD_0 ||
        t == AccountHackingType::GOLD_1 ||
        t == AccountHackingType::GOLD_2 ||
        t == AccountHackingType::GOLD_3)
        return true;
    return false;
}