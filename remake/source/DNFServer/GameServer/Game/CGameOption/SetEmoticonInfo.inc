bool CGameOption::SetEmoticonInfo(const CChattingEmoticonConfig& ChattingEmoticonConfig)
{
    if (memcmp(&m_ChattingEmoticonConfig, &ChattingEmoticonConfig, 0x210) != 0)
    {
        m_ChattingEmoticonConfig = ChattingEmoticonConfig;
        return true;
    }
    return false;
