// ORIG W 0x08372442：+0x00=0、+0x04=0、std::map::clear(+0x08)。
void AccountCargoScript::clear()
{
    m_reqLevel = 0;
    m_itemIdx = 0;
    m_upgradeMap.clear();
}
