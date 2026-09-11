// ORIG W 0x83746ac _ZN9CWorldMap9get_indexEv（0xa 字节）：
//   push ebp; mov esp,ebp; mov 0x8(%ebp),%eax; mov (%eax),%eax; pop ebp; ret
// 非 const 成员（mangled 无 K），直返 m_index（this+0x0）。
// 消费方：CWorldMapList::insert_world_map（0x08364694 call 0x83746ac）。
int CWorldMap::get_index()
{
    return m_index;
}