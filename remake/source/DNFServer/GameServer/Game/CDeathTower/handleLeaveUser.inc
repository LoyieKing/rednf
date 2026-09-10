int CDeathTower::handleLeaveUser(CUser* user)
{
    // ORIG 0x8465aca/0x8465ad1：i 双次置 0 直落 -0xc（范式8）；循环内
    // get_user==user 用 sete %al 直出（范式22，ORIG 0x8465b09）；
    // allDead 物化于 %eax 无槽（0x8465c39/0x8465c40 mov$1/$0 岛）。
    int i = 0;
    i = 0;
    for (; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i))
        {
            // ORIG 0x8465b06：cmp 0xc(%ebp),%eax; sete %al; test al; je skip（范式22）。
            // N96 清绑：具名 bool 槽化反劣（帧 0x20→0x30）⇒ call 结果直比
            // （P197④：结果寄存器直比出 sete 形）。
            if (m_party->get_user(i) == user)
            {
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            m_playData.setMemberAlive(i, false);
            }
        }
    }

    G_GameWorld()->out_from_dungeon(user);

    // [b28d-solo-CDeathTower] p167：ORIG 用嵌套 CPacketHandler（C1 ×1 / D1 ×2）。
    CPacketHandler packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, 0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);

    onLeaveUser(user);

    // N96 清绑：ORIG 0x8465c39/0x8465c40 mov$1/$0 岛物化于 %eax 无槽（P0 无具名
    // 局部）⇒ 短路条件内联直测。
    if (m_party->get_member_count() > 0 && _checkMemberDie())
    {
        _onFinishStage();
        _onPrepareFinishTower();
    }
    return 1;
