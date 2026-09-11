// ORIG W 0x85beda4 _ZN9CWorldMap14GetDungeonListEv（0xb 字节）：
//   push ebp; mov esp,ebp; mov 0x8(%ebp),%eax; add $0x10,%eax; pop ebp; ret
// 非 const 成员，直返 &m_dungeonList（this+0x10）。
// 消费方：CParty::make_blood_dungeon_info（0x085b3e14 区）对返回值 begin()/end()
// 遍历（Ghidra：GetDungeonList(...); vector<int>::begin();）。
std::vector<int>& CWorldMap::GetDungeonList()
{
    return m_dungeonList;
}