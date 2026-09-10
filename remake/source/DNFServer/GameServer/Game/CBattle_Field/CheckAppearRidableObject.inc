void CBattle_Field::CheckAppearRidableObject(PacketGuard* packet)
{
    int x = -1;
    int y = -1;
    getCurPosXY(x, y);
    m_ridable.CheckAppearRidableObject(m_party, x, y, packet);
