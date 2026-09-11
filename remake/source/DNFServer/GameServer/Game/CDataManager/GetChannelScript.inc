void* CDataManager::GetChannelScript() const
{
    // ORIG 0x0814a6f8 W：return &m_channelScript（+0xb3f4）
    return const_cast<channel_script_t*>(&m_channelScript);
}