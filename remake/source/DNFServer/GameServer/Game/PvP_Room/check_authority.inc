// ============================================================================
// [round5 TU 拆分 · 机制① 部件包含式] 自 PvP_Room.cpp 原样切出。
//   承载：PvP_Room::check_authority（原 TU 第 811 行定义）。
// 本部件不写任何头文件引用：全部头/宏/TU 级声明由聚合器 PvP_Room.cpp 在
// 原位置提供（聚合器按原定义序包含本文件，故宏与声明均可见）。
// 函数体与其前置判词注释逐字搬运：零语义、零 codegen 改动。
// ============================================================================

// [b28d-solo-PvP_Room] P292/P248 档①：ORIG 0x085dd070 调用点
// `call check_authority; xor $0x1,%eax; test %al,%al; je`（无 setcc）⇒ callee bool；
// 旧 int 返回产 `test %eax,%eax; sete %al`（+1 条）。返回类型不入 mangled、
// 函数体两形产物相同（ORIG 本体 0x085d65b4 仍 identical）。
bool PvP_Room::check_authority(CUser* user, int seat)
{
    if (m_seats[7] == user)
    {
        return true;
    }
    if (P_SEATS(seat) == user)
    {
        return true;
    }
    return false;
}
