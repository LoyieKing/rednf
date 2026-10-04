void CUser::ResetCurCharacUsedGiftFatigueQuantity()
{
    if (m_usedGiftFatigueQuantity != 0)
    {
        m_usedGiftFatigueQuantity = 0;
        SetChangedGiftFatigueQuantity(true);
    }
}