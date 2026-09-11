// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081feedc orig/ours=308/314 ndiff-blocks=50
//   帧差 sub $50 vs $60 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x50,%esp | lea    -0x2f(%ebp),%eax | add    $0xd,%eax
int Dispatcher_EPLPCommand::dispatch_sig(CUser* user, PacketBuf& packet)
{
    MSG_EPLP_COMMAND local_33;           // 未初始化（_BroadCastPacket 内部填充）
    CParty* local_24;                    // -0x20
    void* local_20;                      // -0x1c：SECRET_SHOP_DATA*
    int local_1c;                        // -0x18：映射后的 EPLP 命令
    unsigned char local_15;              // -0x11：isOpenAdvanceAltar
    WongWork::CBossTower* local_14;      // -0x10
    WongWork::CDeathTower* local_10;     // -0xc


    char cVar2;


    if (!Global::CGlobalFunc::Get_State_Check((ch_state)user->get_state()))
    {
        return 0;
    }
    // ORIG 081fef08 `lea -0x2f(%ebp),%eax; add $0xd,%eax` + `get_byte(unsigned char&)`
    // ⇒ 目标就是 msg.m_cmd(+0xd)/m_subCmd(+0xe)（-0x22/-0x21 = msg 槽基 -0x2f + 偏移），
    // 原独立局部 local_26/local_25 与 ORIG 的成员直写不符。
    if (!packet.get_byte(local_33.m_cmd))                    // cVar2!=1 → LineFunc 0x9238
    {
        return LineFunc(0x9238, __PRETTY_FUNCTION__, 0, 0);
    }

    if (!packet.get_byte(local_33.m_subCmd))                 // cVar2!=1 → LineFunc 0x9239
    {
        return LineFunc(0x9239, __PRETTY_FUNCTION__, 0, 0);
    }

    if (user->get_state() == 0xd)                    // advancealtar 场景
    {
        // ORIG 081fefaf：isOpenAdvanceAltar() call 与 `mov %al,-0x11` 在 PacketGuard
        // ctor(081fefbd) **之前** ⇒ 赋值先于声明。
        local_15 = (unsigned char)advancealtar::isOpenAdvanceAltar();
        PacketGuard local_40;                // -0x48：advancealtar 应答包
        local_40.put_header(1, 0x4b);
        local_40.put_byte((int)local_15);
        local_40.put_byte((int)local_33.m_cmd);
        local_40.put_byte((int)local_33.m_subCmd);
        local_40.finalize(true);
        user->Send(local_40);
        if (local_33.m_cmd == 1)
        {
            advancealtar::Manager::eplp(user, (advancealtar::EplpType::T)local_33.m_subCmd);
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
        _SendEPLPError(local_24, local_33.m_subCmd, 8);
        return 0;
    }
    cVar2 = local_24->GetEPLPState();
    if (!(cVar2 == 2) && excludeEplpState(user))
    {
        return 0;
    }
    _BroadCastPacket(local_24, local_33);
    if (local_33.m_cmd != 1)
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
        _SendEPLPError(local_24, local_33.m_subCmd, 0x85);
        return 0;
    }
    local_1c = 3;
    if (local_33.m_subCmd == 2)
    {
        local_1c = 1;
    }
    else if (local_33.m_subCmd > 2)
    {
        if (local_33.m_subCmd == 3)
        {
            local_24->SetSelectedEPLPCmd(3);
            return 0;
        }
        _SendEPLPError(local_24, local_33.m_subCmd, 1);
    }
    if (user->get_state() == 0xc)

    {
        local_14 = user->getBossTower();
        if (local_14 == 0)
        {
            return 0;
        }
        local_14->handleEPLPCommand((int)local_33.m_subCmd);
        return 0;
    }
    if (user->get_state() == 10)
    {
        local_10 = user->getDeathTower();
        if (local_10 == 0)
        {
            return LineFunc(0x92b9, __PRETTY_FUNCTION__, 0, 0);

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
}