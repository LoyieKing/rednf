// ==================================================================
// CDataManager::get_hellparty_groupOrder_map @ 0x0836379a
// 报告：docs/class_func_reports/CDataManager/get_hellparty_groupOrder_map.md
// 返回 map<int, HellPartyGroupOrderMap> 中 key 对应内层 map 地址；未命中返回 NULL。
// ==================================================================
void* CDataManager::get_hellparty_groupOrder_map(int idx)
{
    HellPartyGroupOrderMapPool* pool = Get_instance_groupOrder_map_pool();
    // ORIG：it 先默认构造（C1Ev），find 按值返回到临时槽再赋值（4.1 无 copy-elision）；
    // 判定用 operator!=，命中分支 fall-through，NULL 块在尾部。
    HellPartyGroupOrderMapPool::iterator it;
    it = pool->find(idx);
    if (it != pool->end())
        return &it->second;                       // pair<const int, ...>+4
    return NULL;
}