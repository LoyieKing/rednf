void CForcedMoveCharacter::ForcedMove()
{
    if (m_user == 0)
    {
        return;
    }

    // ORIG：%al 标志直传（无内存 bool 中转）。先比村庄（movsbl 后有符号
    // 比较），不等即走 move_area；相等再比 get_area(false)，均等才原地
    // set_position。短路结果无槽物化 ⇒ P226 语句表达式（旧具名 int
    // needMove 落槽致帧/指令膨胀）。ORIG 0x082e567c。
    if (({ static_cast<int>(static_cast<signed char>(m_user->getCurCharacVill())) !=
            m_destVill ||
          m_user->get_area(false) != m_destArea; }))
    {
        G_GameWorld()->move_area(m_user, m_destVill, m_destArea, m_x, m_y,
                                 m_dir, false, 0, 0, 0);
    }
    else
    {
        m_user->set_position(static_cast<unsigned short>(m_x),
                             static_cast<unsigned short>(m_y),
                             static_cast<char>(m_dir));
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(
        static_cast<int>(static_cast<unsigned short>(m_user->get_unique_id())));
    packet.put_short(m_x);
    packet.put_short(m_y);
    packet.put_byte(m_dir);
    packet.finalize(true);

    if (m_destVill != 0 || m_destArea != 0)
    {
        G_GameWorld()->send_to_area(m_destVill, m_destArea, packet);
    }
