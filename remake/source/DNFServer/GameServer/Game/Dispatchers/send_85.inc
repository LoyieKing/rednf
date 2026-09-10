void Dispatcher_PartyTeleport::send(CUser* user, ParamBase& param)
{
    // [b23S3C] ORIG 0x081dc426：param 指针槽物化(-0x10)，m_4/m_state/m_c 各用点
    // 回读（P48 去值槽 hoist，ORIG 无 err/echoState/auraRemain 值槽 P0 律）；
    // GetParty 单取缓存、GetPartyTelePort 每子调用重取（ORIG 3 次 call 实证）；
    // put_byte((uchar)m_4) 复现 ORIG `mov 0x4(%eax),%eax; movzbl %al,%eax` 两段。
    struct StPT { ParamBase hdr; int m_4; unsigned char m_state; char gap[3]; int m_c; };
    StPT* const p = reinterpret_cast<StPT*>(&param);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x1b3);                         // class=1 cmd=0x1b3
    if (p->m_4 == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((unsigned char)p->m_4);
        packet.put_byte(p->m_state);
        packet.put_int(p->m_c);
    }
    packet.finalize(true);
    user->Send(packet);
    if (p->m_4 == 0)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->send_teleport_effect_at_different_place(3);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->send_teleport_status(0);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->process_teleport();
        }
    }
