char* CLevelRewardTable::GetMailString(int levelIdx)
{
    return &m_messages[levelIdx * 0x100];
