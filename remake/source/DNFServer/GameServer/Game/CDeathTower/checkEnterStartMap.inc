bool CDeathTower::CPlayData::checkEnterStartMap(int idx)
{
    // ORIG 0x08461e4a：!m_enterStartMap[idx] 时置 true 并返回 true。
    if (!m_enterStartMap[idx])
    {
        m_enterStartMap[idx] = true;
        return true;
    }
    return false;
