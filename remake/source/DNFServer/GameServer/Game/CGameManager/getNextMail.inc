WongWork::CMailBox::CMail* WongWork::CMailBox::getNextMail()
{
    // ORIG 0x0855236a：迭代器指向 m_mailMap；未到 end 则返回 second 并前移，
    // 否则返回 0。
    if (m_mailIterator == m_mailMap.end())
        return 0;
    CMail* mail = m_mailIterator->second;
    ++m_mailIterator;
    return mail;
}