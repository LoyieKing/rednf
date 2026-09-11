// [b28d-solo-StatisticsPvP] incPvPRoomResult DIFF(ndiff=69) -> AE（96=96，p167 10/10 CLEAR）。
// 修法（三处，均为源码形态差异，无 ABI 变更）：
//  ① else 累加分支：ORIG 单次 operator*（deEv）+ `&second` 入具名槽 -0xc（P0 槽-局部律），
//     原两次 `it->second.m_fieldN += x` 被 4.4 折成两次 deEv 且无槽；改具名
//     `PvP_Room* pRoom = &(*it).second;`（P167：`(*it).m` 才实例化 operator*），
//     累加写 `= pRoom->m_fieldN + n`（复现 ORIG `mov mem,%eax; lea/add %edx; mov`）。
//  ② insert 分支：`std::make_pair(key, room)` 额外生成 make_pair 临时 + 转换构造；
//     ORIG 为 value_type 直构 ⇒ 改 `std::pair<const PvP_Key, PvP_Room>(key, room)`
//     （与 ORIG `_ZNSt4pairIKS0_S1_EC1IRS0_RS2_EEOT_OT0_` 同符号）。
//  ③ PvP_Room 头 8 字节改为具名 PvP_Key 成员整体赋值（`room.m_key = key;`）：
//     ORIG 读-读-写-写 8 字节块拷贝（P246），两条标量赋值 4.4 恒 load-store 交错。
// 复验：check_tu_routed.sh COMPILE OK（identical=27 ae=47 near=0 diff=0）。
void StatisticsPvP::incPvPRoomResult(int keyA, int keyB, int value)
{
    Guard<Mutex> guard(&m_mutex);

    PvP_Key key;
    key.m_a = keyA;
    key.m_b = keyB;

    std::map<PvP_Key, PvP_Room>::iterator it = m_map.find(key);
    if (it == m_map.end())
    {
        // ORIG 0x860c653：PvP_Room 四字段 {keyA, keyB, 1, value}。
        PvP_Room room;
        room.m_key = key;
        room.m_field8 = 1;
        room.m_fieldC = value;
        m_map.insert(std::pair<const PvP_Key, PvP_Room>(key, room));
    }
    else
    {
        // ORIG 0x860c6a6：单次 operator*（deEv）后取 &second 入具名槽 -0xc。
        PvP_Room* pRoom = &(*it).second;
        pRoom->m_field8 = pRoom->m_field8 + 1;
        pRoom->m_fieldC = pRoom->m_fieldC + value;
    }
}