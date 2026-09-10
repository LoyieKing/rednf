bool LevelupSupport2ndEventManger::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0xb7, 9, true, false);
    trace("\t- Loading Level Up Support 2nd Event script - ");

    m_script.m_stepItems.clear();
    if (m_script.loadScript())
        return false;
    return true;
