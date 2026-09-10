void CFairPvPScore::SetGiveItem(int v)                 // 0x8234f74
{
    m_giveItem = (unsigned char)v;
    alter();                                           // ORIG call CData::alter
