void CUser::IncCurCharacUsedGiftFatigueQuantity(unsigned int n)
{
    m_field8ebac += n;
    // ORIG 0x08110e57：累加后 SetChangedGiftFatigueQuantity(true)
    SetChangedGiftFatigueQuantity(true);
