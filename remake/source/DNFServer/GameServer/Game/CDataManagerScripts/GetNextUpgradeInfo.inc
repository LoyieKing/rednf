void* AccountCargoScript::GetNextUpgradeInfo(int idx)
{
    std::map<int, UpgradeInfo>::iterator it = m_upgradeMap.find(idx);
    // ORIG 0x88c8149-0x88c814c：find 后立即整字复制 nxt(-0x14→-0x18)，
    // ++nxt 在首个 it != end(0x88c8174 ne) 命中块内（0x88c8183 ppEv(-0x18)）；
    // 两级均 ne+命中 fall-through，je→mov$0 尾岛 0x88c81c6/0x88c81cd。
    std::map<int, UpgradeInfo>::iterator nxt = it;
    if (it != m_upgradeMap.end())
    {
        ++nxt;
        if (nxt != m_upgradeMap.end())
            return &nxt->second;
        return 0;
    }
    return 0;
