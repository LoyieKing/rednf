void Dispatcher_SoloTeleport::send(CUser* user, ParamBase& param)
{
    const stSoloTeleportParam& res = static_cast<const stSoloTeleportParam&>(param);
    PacketGuard packet;
    packet.clear();                 // InterfacePacketBuf::clear ORIG 0x080cb8e6
    packet.put_header(1, 0x1d7);
    if (res.m_4 == 0)
    {
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);

        // [G1Frest 2026-09-10] ORIG 0x081e207b `mov -0xc(%ebp),%eax; mov 0xc(%eax),%eax;
        //   mov %eax,(%esp); call Teleport::move_area(CUser*)` —— m_c(+0xc,int) 的值
        //   **直接**作 this（单级读，无二次解引用）；旧码 `*reinterpret_cast<Teleport**>`
        //   多一条 `mov (%eax),%eax` 且引入具名 tp 局部(-0xc)。同理下文 get_end_cool_time。
        // ORIG xor $1;test al；两分支各自 getCurCharacNo → %ebx 跨 trace ctor（register）
        if (!reinterpret_cast<Teleport*>(res.m_c)->move_area(user))
        {
            register const int charNo = user->getCurCharacNo();
            cMyTrace trace(__PRETTY_FUNCTION__, 0x57d9, 5);
            trace("SOLO_TELEPORT : teleport_error:(char_no:%d)", charNo);
        }
        else
        {
            register const int charNo = user->getCurCharacNo();
            cMyTrace trace(__PRETTY_FUNCTION__, 0x57dd, 0);
            trace("SOLO_TELEPORT : teleport_success:(char_no:%d)", charNo);
        }
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((int)(char)res.m_4);
        packet.put_int(reinterpret_cast<Teleport*>(res.m_c)->get_end_cool_time());
        packet.finalize(true);
        user->Send(packet);
    }
