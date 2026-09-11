// ----------------------------------------------------------------------------
// ORIG T 0x0814512a _ZN8WongWork10CBossTower17onReadyTimerCheckEv：
// 虚调 checkAllMemberReady(get_member_count())（vptr+0x0c）为真 → 0；
// party 空或无成员 → _destroy 返回 0；逐槽位有效且未就绪
// （m_memberReady[i]!=1，基类 +0x11 数组）的成员：onChargeRequireItem +
// 虚调 handleLeaveUser(member)（vptr+0x14）；仍有成员 → 广播包(0,0x1e)/int 0，
// 否则 _destroy；返回 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x0814512a：被调 int 返回 setne（P197④）+ 槽平移（N82）。
// [b28d-solo-CBossTower] AE（145:145，Main 代落 bool 化后翻正）：CBossPlay.h:60
//   checkAllMemberReady int→bool + invalid 裸条件化 + m_memberReady 检查保留。
int CBossTower::onReadyTimerCheck()
{
    if (checkAllMemberReady(m_party->get_member_count()))
        return 0;

    if (m_party == 0 || m_party->get_member_count() < 1)
    {
        _destroy();
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081451eb..0x081451f7：短路物化岛无 bool 槽 ⇒ 裸条件（P0/P226）。
        if (member == 0 || m_party->checkValidUser(i) != 1)
            continue;
        if (m_memberReady[i] != 1)
        {
            onChargeRequireItem(member);
            handleLeaveUser(member);
        }
    }

    if (m_party->get_member_count() > 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x1e);
        packet.put_int(0);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
    else
    {
        _destroy();
    }
    return 1;
}