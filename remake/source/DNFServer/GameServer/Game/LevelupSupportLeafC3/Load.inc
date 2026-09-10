bool LevelupSupportEventManger::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x18, 9, true, false);
    trace("\t- Loading Levelup Support Event script - ");

    m_script.m_stepItems.clear();
    if (!m_script.loadScript())
        return false;

    m_stepIndexMap.clear();
    m_events.clear();

    int stepIndex = 0;   // ORIG -0xc 循环外计数器
    for (std::map<int, std::vector<LevelupSupportSciprt::ItemInfo>*>::iterator it =
             m_script.m_stepItems.begin();
         it != m_script.m_stepItems.end(); it++)
    {
        m_stepIndexMap.insert(std::make_pair(it->first, stepIndex));
        ++stepIndex;
        // ORIG：*pair.first（int 键）截断 ushort 作为达标等级门槛，state=0 默认未领。
        m_events.push_back(EventInfo(static_cast<unsigned short>(it->first), 0));
    }
    return true;
