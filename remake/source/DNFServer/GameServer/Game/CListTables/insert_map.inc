int CMapList::insert_map(int channel, int key, CMap* map,
                         std::vector<PVPMAP_DATA>& pvpData)
{
    // [b28d-solo-CListTables] 范围检查为内联 get_index()（无具名槽，P0 右值直传）。
    if (find_map(map->get_index()) != 0)
    {
        return 0;
    }
    // [b28d-solo-CListTables] ORIG 0x0834f18e：范围检查为内联表达式（每次重调
    // get_index()，无具名槽往返；P0 右值直传）。
    if (map->get_index() > 0xc350 && map->get_index() <= 0xc738)
    {
        m_pvpMapCount++;
        PvPMapInfo info;
        info.m_mapIdx = map->get_index();
        int count = 0;
        // [b28d-solo-CListTables P96/P270] ORIG 0x0834f25a 迭代器为 const_iterator
        // （begin() 结果走 normal_iterator<const PVPMAP_DATA*> 转换构造 call）。
        for (std::vector<PVPMAP_DATA>::const_iterator it = pvpData.begin();
             it != pvpData.end(); ++it)
        {
            // [b28d-solo-CListTables P167] ORIG 用 operator*（deEv）非 operator->（ptEv）。
            if ((*it).m_mapIdx == info.m_mapIdx)
            {
                break;
            }
            count++;
        }
        // [b28d-solo-CListTables] ORIG 0x0834f2d0：单次 operator[] + 自比较守卫 +
        // memcpy 5 —— 4.4 对 5 字节结构赋值即产同形（同 GetPvpMapInfo）。
        mapPvpMapInfo_[count] = info;
    }
    // ORIG 0x0834f2ed..0x0834f42e：4 处插入各只有两次 call —— 先就地构造
    // hash_map 的 value_type（pair<const int,CMap*>），再 insert(value_type const&)：
    //   +0x17b _ZNSt4pairIKiP4CMapEC1IiRS2_EEOT_OT0_   （第一实参为 get_index() 右值）
    //   +0x1c4/+0x226/+0x272 …C1IRiRS2_…            （第一实参为左值 key）
    // 用 make_pair 会多出 pair<int,CMap*> → value_type 的转换临时（第三次 call）。
    m_mainMap.insert(
        __gnu_cxx::hash_map<int, CMap*>::value_type(map->get_index(), map));
    // [b28d-solo-CListTables P167/P183] ORIG 调 get_map_type() 两次（无具名 mapType 局部）。
    if (map->get_map_type() == 0)
    {
        // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x0834f357 目标地址
        // this+0x14*channel+0x14 = m_mapList[channel]（旧实现 channel+1 偏移错一格）。
        m_mapList[channel].insert(
            __gnu_cxx::hash_map<int, CMap*>::value_type(key, map));
    }
    else if (map->get_map_type() == 2)
    {
        m_typeMap2[channel].insert(
            __gnu_cxx::hash_map<int, CMap*>::value_type(key, map));
    }
    else
    {
        m_typeMap1[channel].insert(
            __gnu_cxx::hash_map<int, CMap*>::value_type(key, map));
    }
    return 1;
