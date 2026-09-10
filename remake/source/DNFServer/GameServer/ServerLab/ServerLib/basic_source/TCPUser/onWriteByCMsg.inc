int TCPUser::onWriteByCMsg(CMsgCell* cell)
{
    if (pSock_ == 0)
    {
        G_TraceLog()->sysLog(7, "onWrite_ : Error(1)");
        return -1;
    }
    if (pSock_->getHandle() < 0)
    {
        G_TraceLog()->sysLog(7, "onWrite_ : Error(2)");
        return -2;
    }
    if (isAboutToDisconnect() || isDisconnected())
    {
        G_TraceLog()->sysLog(7, "onWrite_ : Error(3)");
        return -3;
    }
    if (cell->GetSize() == 0)
    {
        G_TraceLog()->sysLog(7, "onWrite_ : Error(4)");
        return -4;
    }
    int totalSize = cell->GetSize();
    while (m_PosByWouldBlock != cell->GetSize())
    {
        int nSent = pSock_->send(cell->GetBuf() + m_PosByWouldBlock, cell->GetSize() - m_PosByWouldBlock);
        if (nSent > 0)
        {
            m_PosByWouldBlock = m_PosByWouldBlock + nSent;
            totalSize = totalSize - nSent;
            G_TraceLog()->sysLog(3, "SENT  \xBA\xB8\xB3\xBD\xB7\xAE=%d, \xB3\xB2\xC0\xBA\xB7\xAE=%d, \xC3\xD1=%d", m_PosByWouldBlock, totalSize, cell->GetSize());
            G_TraceLog()->sysLog(3, "+++SENT  \xBA\xB8\xB3\xBD\xB7\xAE=%d, \xB3\xB2\xC0\xBA\xB7\xAE=%d, \xC3\xD1=%d", m_PosByWouldBlock, totalSize, cell->GetSize());
        }
        else
        {
            if (nSent == 0)
            {
                G_TraceLog()->sysLog(3, "WOULD \xBA\xB8\xB3\xBD\xB7\xAE=%d, \xB3\xB2\xC0\xBA\xB7\xAE=%d, \xC3\xD1=%d", m_PosByWouldBlock, totalSize, cell->GetSize());
                return 0;
            }
            G_TraceLog()->sysLog(3, "Fail OnWriteByCMsg");
            postDisconnected(0x1e);
            if (nSent == -100)
            {
                getSession()->onDoClose();
            }
            return -5;
        }
    }
    G_TraceLog()->sysLog(3, "SEND  \xBA\xB8\xB3\xBD\xB7\xAE=%d, \xB3\xB2\xC0\xBA\xB7\xAE=%d, \xC3\xD1=%d", m_PosByWouldBlock, totalSize, cell->GetSize());
    m_PosByWouldBlock = 0;
    totalSize = 0;
    return 1;
