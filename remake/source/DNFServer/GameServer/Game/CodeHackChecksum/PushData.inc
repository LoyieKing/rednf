bool CodeHackCheckStorage::PushData(const CodeHackChecksumEx* item)     // T @0x08575cd0
{
    if (IsFull())
        return false;                                     // mov $0x0,%eax
    m_queue.push(*item);
    return true;                                          // mov $0x1,%eax
}