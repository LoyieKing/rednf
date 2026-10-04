void CUser::SetCurCharacUsedGiftFatigueQuantity(unsigned int quantity)
{
    if (m_usedGiftFatigueQuantity != static_cast<int>(quantity))
    {
        m_usedGiftFatigueQuantity = static_cast<int>(quantity);
        SetChangedGiftFatigueQuantity(true);
    }
    // ORIG 0x084ed081 call 后 jmp merge + 0x084ed083 nop 尾岛（技巧7：return 置 if 块后、函数尾）
    return;
}