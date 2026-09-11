// ORIG clear 0x089623ee：string.clear 后三 int 清零。
void seriaRoomDecoEventSoundInfo::clear()
{
    m_name.clear();
    m_field0 = 0;
    m_field4 = 0;
    m_fieldC = 0;
}