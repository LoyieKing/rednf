// ORIG W 0x08695f68。
bool LimitItemUsage::isUsable()
{
    // P0：ORIG 三处 movzbl 直读字段，无 state 缓存槽。
    if (m_field10 == 2)
    {
        if (m_field19 >= m_field11)
            return false;
        return true;
    }
    if (m_field10 == 0 || m_field10 == 1)
    {
        if (isAnotherDay())
            m_field19 = 0;
        if (m_field19 >= m_field11)
            return false;
    }
    return true;
}