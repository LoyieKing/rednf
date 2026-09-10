int Dispatcher_New_Gmdebug_Command::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char local_1a = 1;                   // -0x1a（后被 string 复用，语义上为临时）
    char local_19 = 1;                   // -0x19：GM 通过标志
    char* local_1c;                      // -0x18：get_packet(0) 结果
    int local_18;                        // -0x14：*(int*)(pkt+0xd)
    int local_14;                        // -0x10：vector size（填充前）
    game_master::CCommand* local_c;      // -0xc：GetCommand 结果
    char cVar1;

    local_1c = packet.get_packet(0);
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0xa9bc, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned int uVar4 = user->get_acc_id();
    cVar1 = (char)GlobalData::s_pGMAccounts_->isGM(uVar4);
    if (cVar1 != 1)
    {
        local_19 = 0;
    }
    if (local_19 != 1)
    {
        return 0;
    }
    cMyTrace local_34(__PRETTY_FUNCTION__, 0xa9c4, 0);
    local_34("GM Pass Complete!");
    std::vector<std::string> local_40;
    local_18 = *(int*)(local_1c + 0xd);
    *(unsigned char*)(local_1c + local_18 + 0x11) = 0;
    user->getSeparatedTextUnits(local_40, local_1c + 0xd, " ", 0);
    while (local_40.size() < 10)
    {
        local_40.push_back("");
    }
    PacketGuard local_4c;
    DisPatcher_DebugCommand local_50;
    if (user->isGMUser())
    {
        game_master::CParser local_178;
        local_178.m_user = user;
        local_178.m_extra = local_40;
        local_c = GlobalData::s_GameMasterMgr->GetCommand(local_40[0]);
        if (local_c == 0)
        {
            oldGmRoutine(user, packet, local_40);
        }
        else
        {
            local_c->SetParser(&local_178);
            GlobalData::s_GameMasterMgr->HandleRequest(local_c);
        }
    }
    return 0;
