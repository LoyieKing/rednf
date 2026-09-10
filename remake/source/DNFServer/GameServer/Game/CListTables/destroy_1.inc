void CMapList::destroy()
{
    // [b28d-solo-CListTables] ORIG 0x0834f08c：同 QuestList::destroy 形——for-init
    // 逗号式直构 it/end、`(*it)`（deEv）、前置 `++it`（ppEv）、map 无 0 初始化。
    CMap* map;
    for (__gnu_cxx::hash_map<int, CMap*>::iterator it = m_mainMap.begin(),
             end = m_mainMap.end();
         it != end;
         ++it)
    {
        map = (*it).second;
        delete map;
    }
    m_mainMap.clear();
    // [b28d-solo-CListTables 真语义 bug 修复] ORIG 0x0834f11c 三组 clear 目标为
    // this+i*0x14+0x14/+0x154/+0x294（Ghidra C 同步）⇒ 第一组是 m_mapList[i]；
    // 旧实现写 m_mapList[i+1] 既漏清 m_mapList[0] 又多清 m_mapList[15]（越界 1 元素）。
    for (int i = 0; i <= 14; ++i)
    {
        m_mapList[i].clear();
        m_typeMap1[i].clear();
        m_typeMap2[i].clear();
    }
