std::map<unsigned char, _Slot>* AdvanceAltarData::getSlotMap(
    AdvanceAltarSlotType::T type)
{
    switch (static_cast<int>(type))
    {
    case 1:
        return &m_slots1;
    case 2:
        return &m_slots2;
    default:
        return 0;
    }
