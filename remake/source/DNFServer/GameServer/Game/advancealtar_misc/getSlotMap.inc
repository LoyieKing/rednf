// ORIG T 0x08899a0c（非 const，nm 实证 _ZN 无 K）：1/2 → +0x84/+0x9c，
// 其它（含 0）→ 0。
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
}