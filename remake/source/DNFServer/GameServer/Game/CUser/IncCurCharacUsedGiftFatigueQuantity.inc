// ORIG _ZN5CUser35IncCurCharacUsedGiftFatigueQuantityEj W 0x08110e3a：m_field8ebac += n。
void CUser::IncCurCharacUsedGiftFatigueQuantity(unsigned int n)
{
    m_field8ebac += n;
    // ORIG 0x08110e57：累加后 SetChangedGiftFatigueQuantity(true)
    SetChangedGiftFatigueQuantity(true);
}