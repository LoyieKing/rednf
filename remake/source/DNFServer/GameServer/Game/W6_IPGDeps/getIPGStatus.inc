void CIPGHelper::getIPGStatus(stIPGStatus_t& status)
{
    // ORIG 早退形 je 直落 + body jmp/nop 中继（P63，探针 f1 同构）。
    if (m_mgr == 0)
        return;
    m_mgr->getIPGStatus(status);
}
