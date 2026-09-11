GameServerDemonManager::GameServerDemonManager()      // W 0x080f6330
{
    m_prohibitFlag = 0;
    m_noticeLen = 0;
    memset(m_notice, 0, 0xff);
}