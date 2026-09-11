bool CodeHackCheckStorage::PopData(char* fileNameOut)     // T @0x08575d06
{
    if (IsEmpty())
        return false;

    CodeHackChecksumEx tmp(m_queue.front());              // copy ctor front() 快照
    m_queue.pop();
    std::strcpy(fileNameOut, tmp.GetFileName());
    return true;
}