bool CodeHackCheckStorage::IsRightChecksum(const CodeHackChecksum* p)   // T @0x08575dc4
{
    if (IsEmpty())
        return false;
    return m_queue.front().IsRightChecksum(p);
