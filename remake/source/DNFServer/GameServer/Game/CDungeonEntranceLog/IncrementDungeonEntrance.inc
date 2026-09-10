void CDungeonEntranceLog::IncrementDungeonEntrance(int dungeonIdx, bool isParty)
{
    // [b28d-59] AE（ORIG 0x860e1ee）：指令序列逐条一致，.o 层仅 call 重定位标签
    // 未解析的显示差（AE 口径归零）；95/95。
    // ORIG 0x860e1ee：存在路径 isParty 仅 ++m_partyEnter（不加 total/curInside）；
    // 插入路径 isParty 时 ++m_partyEnter 且 ++m_total/++m_curInside（不对称，忠实复刻）。
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.find(dungeonIdx);
    if (it != m_inoutMap.end())
    {
        if (isParty)
        {
            it->second.m_partyEnter += 1;
        }
        else
        {
            it->second.m_enter += 1;
            it->second.m_total += 1;
            it->second.m_curInside += 1;
        }
    }
    else
    {
        STDungeonInoutCnt cnt;
        if (isParty)
        {
            cnt.m_partyEnter += 1;
        }
        else
        {
            cnt.m_enter += 1;
        }
        cnt.m_total += 1;
        cnt.m_curInside += 1;
        m_inoutMap.insert(std::make_pair(dungeonIdx, cnt));
    }
