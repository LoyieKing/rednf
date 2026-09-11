bool Secu_AccountHacking::isAvatarPatten(AccountHackingType::T t)
{
    // ORIG W 0x08279282（isAvatarPatten.md）：4/5 判 avatar 伪型。
    if (t == AccountHackingType::AVATAR_COUNT ||
        t == AccountHackingType::AVATAR_TIME)
        return true;
    return false;
}