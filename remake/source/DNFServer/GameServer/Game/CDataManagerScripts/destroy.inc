// ORIG 0x8374752：遍历 delete 各 CWorldMap*（D1 + operator delete），最后清空 map。
// [b28d-solo-CDataManagerScripts] AE：①`iterator it;` 默认构造 + `it = m_map.begin()`（P50 复活）+ `iterator e = m_map.end()` 缓存；②无条件 `delete worldMap`（P141）⇒ 51=51 全等；ORIG 0x08374752。
void CWorldMapList::destroy()
{
    // ORIG 0x8374752：局部变量暂存 second、delete、清零局部（不写回 map 值）。
    std::map<int, CWorldMap*>::iterator it;
    std::map<int, CWorldMap*>::iterator e = m_map.end();
    it = m_map.begin();
    for (; it != e; ++it)
    {
        CWorldMap* worldMap = it->second;
        delete worldMap;
        worldMap = 0;
    }
    m_map.clear();
}