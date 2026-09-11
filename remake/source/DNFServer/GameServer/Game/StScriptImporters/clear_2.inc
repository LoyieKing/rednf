// ORIG clear 0x891ac9a（0x33）：+0x00/+0x04/+0x08 int=0 + vector<pair> clear。
void stNeoplePCBANG::clear()
{
    m_field0 = 0;
    m_level = 0;
    m_peopleCount = 0;
    m_status.clear();
}