// ORIG Broadcast @ 0x08340404：
//   +0x1c=0；vill==0 && area==0 → 返回 0；
//   IsGateArea(m_user, area) → m_user->Send(*this)；
//   否则 G_GameWorld()->send_to_area(vill, area, *this)；返回 1。
// [b28d-solo-CPacketRespondent P248档①] 同 Send：gate 路径 `call Send; jmp 尾` 直通，
// 与 CUser::Send(bool) 同型；ORIG 0x08340404 尾 `mov $0x1,%eax` / `mov $0x0,%eax` 两臂。
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
}