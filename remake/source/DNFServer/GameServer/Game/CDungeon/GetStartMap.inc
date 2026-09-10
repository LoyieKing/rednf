int CDungeon::GetStartMap(int direction) const
{
    // ORIG 0x0834c75a：方向守卫共享一个 -1 返回块；空表 log 后 -1；
    // 随机前进 it++（后置）逐槽检查；返回 it->first。
    if (direction > 0xf || direction < 0)
        return -1;

    if (m_maplist[direction].size() == 0)
    {
        LogManager::logFormat(
            1, "data_manager.cpp", __PRETTY_FUNCTION__,
            0x79a,
            "CDungeon::GetStartMap() : direction(%d), dungeon index(%d), "
            "start_maplist_[direction].size() == 0, May be Script Error!",
            direction, m_index);
        return -1;
    }

    int r = get_rand_int(m_maplist[direction].size());
    __gnu_cxx::hash_map<int, CMap*>::const_iterator it =
        m_maplist[direction].begin();
    for (int i = 0; i < r; i++)
    {
        it++;
        if (it == m_maplist[direction].end())
            return -1;
    }
    return it->first;
