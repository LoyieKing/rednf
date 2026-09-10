int WongWork::CWorldDrop::loadScript(char const* path, char const* warArea)
{
    if (!ImportWorldDrop(m_worldDrop, const_cast<char*>(path)))
        return 0x7b9;
    if (!ImportWorldDrop(m_warAreaDrop, const_cast<char*>(warArea)))
        return 0x7bb;
    return 0;
