bool CBattle_Field::SetDamageValue(int uid, int damage)
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
    // ORIG +0x88/+0x96：operator-> 后 add $0x4（second）存 mob，再 0x24(%eax)
    // 整型累加（m_field28 为 float 视图，ORIG 按 int 加）。
    mob = &it->second;
    // ORIG +0x96/+0xa1：0x24(%eax) 整型累加 damage = m_field24（int；
    // 非 m_field28，报告注释偏移需以汇编为准）。
    mob->m_field24 += damage;
    return true;
