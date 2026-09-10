int Dispatcher_EPLPCommand::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_EPLP_COMMAND local_33;           // 未初始化（_BroadCastPacket 内部填充）
    unsigned char local_26;              // -0x22：命令字节
    unsigned char local_25;              // -0x21：子命令字节
    CParty* local_24;                    // -0x20
    void* local_20;                      // -0x1c：SECRET_SHOP_DATA*
    int local_1c;                        // -0x18：映射后的 EPLP 命令
    unsigned char local_15;              // -0x11：isOpenAdvanceAltar
    WongWork::CBossTower* local_14;      // -0x10
    WongWork::CDeathTower* local_10;     // -0xc
    PacketGuard local_40;                // -0x48：advancealtar 应答包（ORIG PacketGuard ctor 0x0858dd4c）
    char cVar2;
    int iVar4;
    int ret;

    if (!Global::CGlobalFunc::Get_State_Check((ch_state)user->get_state()))
    {
        return 0;
    }
    cVar2 = packet.get_byte(local_26);
    if ((char)(cVar2 ^ 1) != 0)          // cVar2!=1 → LineFunc 0x9238
    {
        ret = LineFunc(0x9238, __PRETTY_FUNCTION__, 0, 0);
        return ret;
    }
    cVar2 = packet.get_byte(local_25);
    if ((char)(cVar2 ^ 1) != 0)          // cVar2!=1 → LineFunc 0x9239
    {
        ret = LineFunc(0x9239, __PRETTY_FUNCTION__, 0, 0);
        return ret;
    }
    iVar4 = user->get_state();
    if (iVar4 == 0xd)                    // advancealtar 场景
    {
        local_15 = (unsigned char)advancealtar::isOpenAdvanceAltar();
        local_40.put_header(1, 0x4b);
        local_40.put_byte((int)local_15);
        local_40.put_byte((int)local_26);
        local_40.put_byte((int)local_25);
        local_40.finalize(true);
        user->Send(local_40);
        if (local_26 == 1)
        {
            advancealtar::Manager::eplp(user, (advancealtar::EplpType::T)local_25);
        }
        return 0;
    }
    local_24 = (CParty*)user->GetParty();
    if (local_24 == 0)
    {
        return 0;
    }
    if (local_24->getManager() != user)
    {
        _SendEPLPError(local_24, local_25, 8);
        return 0;
    }
    cVar2 = local_24->GetEPLPState();
    if (!(cVar2 == 2) && excludeEplpState(user))
    {
        return 0;
    }
    _BroadCastPacket(local_24, local_33);
    if (local_26 != 1)
    {
        return 0;
    }
    local_20 = user->GetSecretShopData();
    bool bVar1 = false;
    if (local_20 != 0 && ((secretshop::SECRET_SHOP_DATA*)local_20)->IsOpen() != 0)
    {
        bVar1 = true;
    }
    if (bVar1 && ((secretshop::SECRET_SHOP_DATA*)local_20)->IsCompleteBuy() == 0)
    {
        _SendEPLPError(local_24, local_25, 0x85);
        return 0;
    }
    local_1c = 3;
    if (local_25 == 2)
    {
        local_1c = 1;
    }
    else if (local_25 > 2)
    {
        if (local_25 == 3)
        {
            local_24->SetSelectedEPLPCmd(3);
            return 0;
        }
        _SendEPLPError(local_24, local_25, 1);
    }
    iVar4 = user->get_state();
    if (iVar4 == 0xc)
    {
        local_14 = user->getBossTower();
        if (local_14 == 0)
        {
            return 0;
        }
        local_14->handleEPLPCommand((int)local_25);
        return 0;
    }
    iVar4 = user->get_state();
    if (iVar4 == 10)
    {
        local_10 = user->getDeathTower();
        if (local_10 == 0)
        {
            ret = LineFunc(0x92b9, __PRETTY_FUNCTION__, 0, 0);
            return ret;
        }
        local_10->handleSelectEPLPCommand(user, local_1c);
        return 0;
    }
    if (!isReTurnToVillage(user))
    {
        local_24->SelectEPLPCommand(local_1c);
    }
    else
    {
        local_24->ReturnToVillage();
    }
    return 0;
