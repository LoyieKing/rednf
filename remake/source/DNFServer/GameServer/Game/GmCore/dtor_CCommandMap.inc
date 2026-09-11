// [b28d-solo-GmCore] AE | ORIG 0x082a7d88 | 双容器（map+unordered_map）迭代器析构
// 循环逐条全等（含两段 EH landing）；show_func_diff ndiff=0，仅 .o 未解析重定位
// 显示差（call 目标）属 AE 归一范围。
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
}