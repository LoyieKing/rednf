// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvpResultRecvFlag::IsRecv（原 TU 第 128 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================
// [b28d-solo-PvP_Room] P210② 跨 TU 迁定义（Main 指派）：ORIG IsRecv 0x085d5ff0 与
// PvP_Room::C1 0x085d6012 地址相邻 ⇒ 原码同 TU；定义迁入本 TU 且置于 4 个 caller
// （recv_pvp_rank_all / recv_pvp_end_result_all / get_recv_pvp_rank_count /
// get_recv_pvp_end_result_count）之前 ⇒ 4.4 精确继承小帧（ORIG 帧 $0x18 / 槽 -0x4）。
// 形按 ORIG：js/jle 双守卫 + movzbl 直返（bool 视图免 setne 归一）。
// [b28d-solo-PvP_Room] AE 归零（13=13，本轮）：P235/P311 —— PvPTypes.h 权威头把
// m_pad 包进匿名 union 增 bool m_bool[0x8] 视角，改具名成员直访问后 4.4 复现
// ORIG 的 `mov 0xc(%ebp),%eax; mov 0x8(%ebp),%edx; movzbl (%edx,%eax,1),%eax`
// （index 先、base 后）；旧 reinterpret_cast 视图形恒 base 先载（NEAR 2 的真因）。
bool PvpResultRecvFlag::IsRecv(int flag) const
{
    if (flag < 0 || flag > 7)
        return false;
    // [b28d-solo-PvP_Room] P235/P311：改用 PvPTypes.h 权威头新增的具名数组视角
    // m_bool[flag]（union 与 m_pad 同宽 8B，sizeof/偏移零变化）⇒ 完整类型成员
    // 直访问复现 ORIG `mov 0xc(%ebp),%eax; mov 0x8(%ebp),%edx; movzbl (%edx,%eax,1),%eax`
    // （index 先、base 后）；旧 reinterpret_cast 视图形恒 base 先载。
    return m_bool[flag];
}