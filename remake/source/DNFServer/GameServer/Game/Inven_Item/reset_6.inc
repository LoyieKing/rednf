void Inven_Item::reset()
{
    m_seal = 0;
    m_itemType = 0;
    m_attr = 0;
    m_addInfo = 0;
    m_addInfo2 = 0;
    m_endurance = 0;
    m_extendInfo = 0;
    m_amp.reset();
    m_random.reset();
    m_upgradeSep.reset();
    m_reserved.reset();
}