MSG_UDP_CAPCHA_CERTIFY* WongWork::CCapchaDataManager::peekData()
{
    // ORIG T 0x08299f74：at(get_rand_int(size()))——随机取一条。
    return &m_queue.at(static_cast<unsigned int>(
        get_rand_int(static_cast<int>(m_queue.size()))));
}