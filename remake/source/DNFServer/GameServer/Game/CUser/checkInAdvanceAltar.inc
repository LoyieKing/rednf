bool CUser::checkInAdvanceAltar() const
{
    return (~(unsigned short)m_advanceAltarIdx & 0x8000) != 0;
