bool CodeHackCheckStorage::IsFull()                       // T @0x08575cb6
{
    return m_queue.size() == MAX_STORED_CHECKS;           // cmp $0x30,%eax / sete
}