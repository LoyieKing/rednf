pvp_channel_info_t::pvp_channel_info_t()
{
    // ORIG 0x8370912：+0x00..+0x20 共 9 个 dword 清零；+0x24 不初始化。
    m_field0 = 0;
    m_minGrade = 0;
    m_maxGrade = 0;
    m_upgradeRevision = 0;    // +0x0c（ORIG 先清 +0x0c 再清 +0x10）
    m_channelType = 0;        // +0x10
    m_minLevel = 0;
    m_maxLevel = 0;
    m_field1c = 0;
    m_field20 = 0;
