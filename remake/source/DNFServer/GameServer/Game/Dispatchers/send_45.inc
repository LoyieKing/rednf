void Dispatcher_PartyTeleportConfirm::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081dc830：p4==0 → Ok(0x1b4)+party teleport 状态/处理/复位；否则 Error(0x1b4,p4&0xff)
    stPartyTeleportConfirm& p =
        static_cast<stPartyTeleportConfirm&>(param);
    if (p.m_result == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1b4);
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->send_teleport_status(0);
            static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                ->process_teleport();
            if (p.m_bConfirm != 1)
            {
                static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                    ->reset_teleport_data();
            }
        }
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b4,
                                 (unsigned char)(p.m_result & 0xff));
    }
