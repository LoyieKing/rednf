// ORIG C1 0x089623ba：+0/+4/+0xc 清零，string 默认构造。
seriaRoomDecoEventSoundInfo::seriaRoomDecoEventSoundInfo()
    : m_field0(0), m_field4(0)   // m_name(+8) 自动构造先于 m_fieldC，故 m0/m4 入 init-list、m_fieldC 留 body(ORIG 序)
{
    m_fieldC = 0;
}