ChattingEmoticonBase* CChattingEmoticonList::find_emoticon(int idx)
{
    std::map<int, ChattingEmoticonBase*>::iterator it = m_map.find(idx);
    if (m_map.end() == it)   // ORIG @0x80e5850：eq 调用对象=end 槽(arg0=-0xc)
        return 0;
    return it->second;
