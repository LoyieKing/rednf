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
