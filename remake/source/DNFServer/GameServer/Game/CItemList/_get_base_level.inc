int CCompound_ExtreamDun_Item::_get_base_level(                     // ORIG W 0x832e40c
    const CEquipItem* const* refs) const
{
    return (refs[0]->getUsableLevel() + refs[1]->getUsableLevel()) >> 1;
}