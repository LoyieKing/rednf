void CDungeonEntranceLog::DecrementDungeonEntrance(int dungeonIdx, bool /*isParty*/)
{
    std::map<int, STDungeonInoutCnt>::iterator it = m_inoutMap.find(dungeonIdx);
    if (it != m_inoutMap.end())
        it->second.m_curInside -= 1;   // ORIG 节点 +0x10 -1（未命中不动）
}