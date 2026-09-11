// ORIG clear 0x891ba80（0x3f）：+0x00/+0x04 int=-1、+0x08 vector clear + reserve(4)。
void stQPGuideScript::stRecommendQPData::clear()
{
    m_job = -1;
    m_grow = -1;
    m_items.clear();
    m_items.reserve(4);
}