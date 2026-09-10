void CFairMatch::OnConnectP2PTimeout(PvP_Room* room)
{
    if (m_field04 == 4) {
        _SendMatchStatePacket(4, 2);
        // b28d-solo-CFairMatchRooms：局部 unsigned + _ReturnTeam 形参 int 异型
        // （P211/P274 旋钮）令 4.4 先求值第 3 参再写第 4 参常量 —— 复现 ORIG 4.1 的
        // `mov -0x1c(%ebp),%eax; movl $0x0,0xc(%esp); mov %eax,0x8(%esp)` 装载序
        // （探针 /tmp/fm_p3.cpp w31）。
        unsigned int mgrTeam = room->get_manager_team();
        _ReturnTeam(room, mgrTeam, static_cast<CUser*>(0));
        if (mgrTeam != 1) {
            m_teams[0] = m_teams[1];
        }
        m_teams[0].refresh();
        m_teams[1].reset();
    }
