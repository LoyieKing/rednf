// ORIG T 0x081047ea (0x9a)：Guard(reqMutex)；空队→0；front 为 0 → 返回 0 不弹；
// 否则 pop 并返回原 front。
SIPGData* CIPGMgr::_GetIPGRequest()
{
    Guard<Mutex> guard(&m_reqMutex);
    if (m_reqQueue.empty())
    {
        return 0;
    }
    SIPGData* front = m_reqQueue.front();
    if (front == 0)
    {
        return 0;
    }
    m_reqQueue.pop();
    return front;
}
