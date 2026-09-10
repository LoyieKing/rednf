bool CBattle_Field::SetDamagedPacketCnt(int uid)
{
    // P28：先声明迭代器再赋值（find 经临时中转拷贝）；P48：GetCurrentMapInfo
    // 在 find/end 两处各自调用（无 CSE）；-0xc 初 0 为 operator-> 结果槽。
    map_monster* mob = 0;
    std::map<int, map_monster>::iterator it;
    it = GetCurrentMapInfo()->m_monsterMap.find(uid);
    if (it == GetCurrentMapInfo()->m_monsterMap.end())
    {
        return false;
    }
    // ORIG +0x88/+0x96：operator-> 后 add $0x4（second）存 mob，再 0x20(%eax)
    // 自增（m_field20；Ghidra C 的 +0x24 为 P8 偏移错位，汇编 0x20 为准）。
    mob = &it->second;
    mob->m_field20 += 1;
    return true;
