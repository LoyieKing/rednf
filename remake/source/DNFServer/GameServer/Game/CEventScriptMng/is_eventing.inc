bool CEventEntity::is_eventing()
{
    // ORIG 0x810b016：movzbl 0x4(%eax)（直读 bool）。
    return m_isEventing;
}