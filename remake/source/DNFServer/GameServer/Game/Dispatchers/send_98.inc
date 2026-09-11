// ==== Dispatcher_SoloTeleport::send (ORIG 0x081e1ff6) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e1ff6 orig/ours=123/126 ndiff-blocks=26
//   帧差 sub $40 vs $50 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x40,%esp | mov    %eax,-0xc(%ebp) | lea    -0x38(%ebp),%eax
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
            // [AttackA b28d-r7 P221 试形] 全表达式临时链复现 ORIG sibling/互斥块槽序
            cMyTrace(__PRETTY_FUNCTION__, 0x57d9, 5)("SOLO_TELEPORT : teleport_error:(char_no:%d)", charNo);
        }
        else
        {
            register const int charNo = user->getCurCharacNo();
            cMyTrace(__PRETTY_FUNCTION__, 0x57dd, 0)("SOLO_TELEPORT : teleport_success:(char_no:%d)", charNo);
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
}