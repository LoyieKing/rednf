EquipItemDropRate::EquipItemDropRate(RandomBuffType type, QuickPartyData& data) throw()
    : IRandomwReward(data)
{
    m_type = type;
}