bool AvatarCoin::Use()   // [b28d-Main 代落·S341 P292] unsigned int→bool
{
    // [b28c-lane17] P0：清除历史 register asm("al") 绑定，改自然条件形。
    if (IsUsable())
    {
        m_coin = m_coin + -1;
        return true;
    }
    return false;
}