void CGuildAgitArea::SetAreaMapping(unsigned int guildKey, int areaIdx)
{
    m_areaMap.insert(std::make_pair(guildKey, areaIdx));
}