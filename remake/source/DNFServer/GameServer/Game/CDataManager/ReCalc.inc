int CChattingEmoticonList::ReCalc(CChattingEmoticonConfig& config,
                                  const CUser& user)
{
    std::vector<CChattingEmoticonConfigRow> rows;
    std::set<int> used;

    // ORIG 0x080e58?? `mov -0x20; cmp $0x17; setbe` = 无符号比较（范式库
    // P2 反例：signed setle 异形）。P310：局部 int + 显式 (unsigned) 比较
    // 复现 ORIG 的「装载 + 寄存器比较」两段尾测（unsigned 局部恒折内存直比）。
    for (int i = 0; (unsigned int)i <= 0x17; ++i)
    {
        ChattingEmoticonBase* p = find_emoticon(config.m_Row[i].m_nCount);
        if (p == 0)
            continue;
        if (!p->isCommonEmoticon(user))
            continue;
        rows.push_back(config.m_Row[i]);
        used.insert(config.m_Row[i].m_nCount);
    }
    config.clear();

    for (unsigned int r = 0; r < rows.size(); ++r)
    {
        ChattingEmoticonBase* p = find_emoticon(rows[r].m_nCount);
        if (p == 0)
            continue;
        for (int i = p->GetStartArrayIndex(); i <= p->GetEndArrayIndex(); ++i)
        {
            if (config.m_Row[i].m_nCount == 0)
            {
                config.m_Row[i] = rows[r];
                break;
            }
        }
    }

    for (std::map<int, ChattingEmoticonBase*>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        if (used.find(it->second->GetIndex()) != used.end())
            continue;
        if (!it->second->isCommonEmoticon(user))
            continue;
        for (int i = it->second->GetStartArrayIndex();
             i <= it->second->GetEndArrayIndex(); ++i)
        {
            if (config.m_Row[i].m_nCount == 0)
            {
                config.m_Row[i].m_nCount = (short)it->second->GetIndex();
                break;
            }
        }
    }
    return 1;
