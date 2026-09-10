void ChattingEmoticonBase::AddPremiumIndex(int index)
{
    // ORIG 0x080e5dde：特定 index 映射到 premium 槽位集合。
    switch (index)
    {
    case 0x2898bf:
    case 0x2898c0:
        m_set.insert(0x53);
        break;
    case 0x2898c1:
    case 0x2898c2:
        m_set.insert(0x57);
        break;
    }
