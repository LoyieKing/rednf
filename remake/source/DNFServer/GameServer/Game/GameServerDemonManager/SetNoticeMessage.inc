void GameServerDemonManager::SetNoticeMessage(unsigned char len, char* msg)  // T 0x080f6280
{
    m_noticeLen = len;
    memset(m_notice, 0, 0xff);
    memcpy(m_notice, msg, static_cast<unsigned int>(len));
