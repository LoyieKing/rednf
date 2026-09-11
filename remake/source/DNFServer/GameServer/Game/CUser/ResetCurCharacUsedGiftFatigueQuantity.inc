void CUser::ResetCurCharacUsedGiftFatigueQuantity()
{
    if (m_field8ebac != 0)
    {
        m_field8ebac = 0;
        SetChangedGiftFatigueQuantity(true);
    }
}