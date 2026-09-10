short CBattle_Field::getNextSamllRoundOrder(short round, int diff)
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08306212：
    // ①P140 无 GetCurrentMapInfo 判空（ORIG 直 lea 0xc(%eax)）；
    // ②P0 无 mapInfo 具名局部——循环增量处再次 call GetCurrentMapInfo，
    //   end() 每轮重求（P13 反向）；
    // ③P157/P183 it->second 逐字段重读（两次 operator->()），禁引用缓存；
    // ④short result 预清 `movw $0x0,-0xa(%ebp)`，尾 movzwl 直返。
    // ⑤增量段 ORIG 调后置 operator++（ppEi，sret 临时 -0x10）⇒ 源写 `it++`
    //   （P28 增量侧；本件 4.4 未折叠，逐条复现 66=66 gap=0）。
    short result = 0;
    std::map<int, map_monster>::iterator it = GetCurrentMapInfo()->m_monsterMap.begin();
    for (; it != GetCurrentMapInfo()->m_monsterMap.end(); it++)
    {
        if (it->second.m_bloodRoundOrder2 == round &&
            (int)(signed char)it->second.m_bloodSpawned == diff)
        {
            result = (short)it->second.m_bloodSmallRoundOrder;
            break;
        }
    }
    return result;
