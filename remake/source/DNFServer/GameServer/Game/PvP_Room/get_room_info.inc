// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::get_room_info（原 TU 第 2169 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

void PvP_Room::get_room_info(PVP_ROOM_INFO* out)
{
    out->title = m_roomName;
    out->byte4 = m_mode8;
    out->short6 = m_requestedMapNo;
    out->byte8 = m_isExistPassword;
    out->passwd = m_password;
}
