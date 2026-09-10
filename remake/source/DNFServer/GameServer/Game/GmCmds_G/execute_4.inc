void CVillageAttackedFightCmd::execute()
{
    CParty* party = static_cast<CParty*>(GetUser()->GetParty());
    if (party == 0)
    {
        party = G_CGameManager()->GetParty();
        if (party != 0)
            party->set_single_play(GetUser());
    }
    if (party != 0)
    {
        int ret = 0;
        ret = party->OnFightVillageMonster(GetUser(), 0);
        // 4.1 bool 物化+je 形：while(cond){body;break} 使 4.4 保留
        // cmpl/setne/test/je 四段（探针 /tmp/bl5/probe7.cpp m1 实证）。
        while (ret != 0)
        {
            GetUser()->SendCmdErrorPacket((ENUM_CMDPACKET)0x10b,
                                          (unsigned char)ret);
            break;
        }
    }
