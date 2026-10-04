// ORIG _ZN5CUser35IncCurCharacUsedGiftFatigueQuantityEj W 0x08110e3a：m_usedGiftFatigueQuantity += n。
void CUser::IncCurCharacUsedGiftFatigueQuantity(unsigned int n)
{
    m_usedGiftFatigueQuantity += n;
    // ORIG 0x08110e57：累加后 SetChangedGiftFatigueQuantity(true)
    SetChangedGiftFatigueQuantity(true);
}