bool CGameOption::isEqualEmoticon(const CChattingEmoticonConfig& ChattingEmoticonConfig)
{
    return memcmp(&m_ChattingEmoticonConfig, &ChattingEmoticonConfig, 0x210) == 0;
}