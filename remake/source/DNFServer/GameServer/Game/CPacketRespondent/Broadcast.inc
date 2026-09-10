bool CPacketRespondent::Broadcast(int vill, int area)
{
    m_autoSend = 0;
    if (vill == 0 && area == 0)
        return false;  // ORIG 0x834041e mov $0x0,%eax
    // ORIG 0x834045a：gate 臂 call Send 后直接 jmp 统一尾（+0x7f=add esp），
    // 跳过 mov $0x1,%eax ⇒ Send 返回值透传（正向 return 形）。
    if (IsGateArea(m_user, area))
        return m_user->Send(*this);
    G_GameWorld()->send_to_area(vill, area, *this);
    return true;   // ORIG 0x834047e mov $0x1,%eax
