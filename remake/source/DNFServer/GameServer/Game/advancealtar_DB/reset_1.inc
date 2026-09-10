void CharacAdvanceAltarManager::reset()
{
    m_currentAdvanceAltarData = 0;
    for (int local_c = 0; local_c <= 0; ++local_c)
    {
        // 单实例布局：m_data@+0x04、stride 0xd0 == ORIG `this + local_c*0xd0 + 4`。
        m_data[local_c].reset();
    }
