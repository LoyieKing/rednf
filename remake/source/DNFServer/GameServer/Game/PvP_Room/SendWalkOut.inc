// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::SendWalkOut（原 TU 第 1607 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

int PvP_Room::SendWalkOut(int seat, ENUM_WALKOUT_TYPE walkoutType)
{
    PacketGuard local_18;
    local_18.put_header(0, 10);
    local_18.put_byte(seat);
    local_18.put_byte((int)walkoutType);
    local_18.finalize(true);
    send_to_pvp(local_18);
    return 1;
}
