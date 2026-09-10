void CUser::set_ingame_event_history(int eventType, int v1, int v2)
{
    // set_ingame_event_history(iii).md @0x868d580：map<int,pair<int,int>> m_map8eb68
    // find/insert-or-update；随后 update_ingame_event_history(eventType) 置脏标记。
    // ORIG 0x0868d5xx：`call operator!=; je INSERT岛`——命中直写体为
    // fall-through（技巧17），insert 岛在后
    std::map<int, std::pair<int, int> >::iterator it = m_map8eb68.find(eventType);
    if (it != m_map8eb68.end())
    {
        // ORIG：两段各重调 iterator::operator->，`mov %edx,0x4(%eax)`/
        // `mov %edx,0x8(%eax)` 免槽直写 pair 字段
        it->second.first = v1;
        it->second.second = v2;
    }
    else
    {
        // [b28d-solo-CUserOnlyOrig P167/P81 真形] ORIG 0x0868d5fd 岛：先
        // `call pair<int,int>::pair()` 默认构造（-0x2c）+ 逐字段 store
        // （`mov 0x10(%ebp),%eax; mov %eax,-0x2c(%ebp)`），再
        // `call pair<const int,pair<int,int>>::pair<const int&,pair<int,int>&>(&eventType,&value)`
        // （-0x14）后 map::insert。旧写法 make_pair 链产 EXTRA
        // make_pair<int&,int&>/<int&,pair<int,int>> + move ctor、缺 pair() 与左值对构造。
        std::pair<int, int> value;
        value.first = v1;
        value.second = v2;
        // 第一参须为 const int 左值才能选中 ORIG 的
        // `pair<const int&, pair<int,int>&>(int const&, pair<int,int>&)` 构造
        // （裸 eventType 会选 `pair<int&, pair<int,int>&>` 变体）。
        m_map8eb68.insert(
            std::pair<const int, std::pair<int, int> >(
                static_cast<const int&>(eventType), value));
    }
    update_ingame_event_history(eventType);
