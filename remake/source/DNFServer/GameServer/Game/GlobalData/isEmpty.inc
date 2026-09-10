bool WongWork::CCapchaDataManager::isEmpty()
{
    // ORIG W 0x086977fa：deque::empty。
    return m_queue.empty();
