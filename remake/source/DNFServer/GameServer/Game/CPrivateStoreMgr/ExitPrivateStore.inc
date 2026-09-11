// ---- ExitPrivateStore（ORIG T 0x85c8fd4，报告 ExitPrivateStore.md）----
// ISeller 槽 +0x24 / IBuyer 槽 +0x0c。买家离店：发退出命令包、释放买家表项、
// 从围观列表摘除并清空买家指针。列表非空且找不到该买家时报 0x15。
bool CPrivateStore::ExitPrivateStore()
{
    m_errorHandler.Set(m_pBuyer, 1, 0x57);

    if (m_pBuyer == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }

    std::list<CUser*>::iterator found =
        std::find(m_users.begin(), m_users.end(), m_pBuyer);
    if (!m_users.empty())
    {
        if (found == m_users.end())
        {
            m_errorHandler.Do(0x15);
            return false;
        }
    }

    CPacketBufPrivate packet;
    packet.MakeCmdpacketExitPrivateStore();
    m_pBuyer->Send(packet);

    m_pParent->FreeStoreBuyer(m_pBuyer);

    if (!m_users.empty())
        m_users.erase(found);

    m_pBuyer = 0;
    return true;
}