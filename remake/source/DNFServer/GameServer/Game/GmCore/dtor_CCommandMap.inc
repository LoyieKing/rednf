CCommandMap::~CCommandMap()                    // 0x082a7d88
{
    // ORIG 双遍历：id 侧与名字侧各按迭代器取 second，判空后 delete。
    for (std::map<int, CCommand*>::iterator it = m_byId.begin(); it != m_byId.end(); ++it)
    {
        CCommand* cmd = it->second;
        if (cmd != 0)
        {
            delete cmd;
        }
    }
    for (std::unordered_map<std::string, CCommand*>::iterator it = m_byName.begin();
         it != m_byName.end(); ++it)
    {
        CCommand* cmd = it->second;
        if (cmd != 0)
        {
            delete cmd;
            cmd = 0;
        }
    }
