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