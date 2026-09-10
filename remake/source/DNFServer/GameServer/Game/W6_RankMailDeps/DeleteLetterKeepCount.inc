bool WongWork::CMailBox::DeleteLetterKeepCount(unsigned int key)
{
    // ORIG +0x006..+0x07a: find(key); it != end() → erase(it) 返回 1，否则 0。
    std::map<int, bool>::iterator it =
        m_letterKeepCount.find(static_cast<int>(key));
    if (it != m_letterKeepCount.end())
    {
        m_letterKeepCount.erase(it);
        return 1;
    }
    return 0;
