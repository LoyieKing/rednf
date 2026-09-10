int CPowerManager::ProcessJoinPowerWar(CUser* user, char side)
{
    // ORIG 0x0847fb02：加入/退出 PowerWar 的完整校验与处理。
    // 职业 9/10（未转职）与佣兵不可参加。
    if (user->get_charac_job() == 9)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->get_charac_job() == 10)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->isCompetitionMercenary())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x15);
        return 1;
    }
    if (user->get_charac_guildkey() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 100);
        return 1;
    }
    if (user->getPowerSide() == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x82);
        return 1;
    }
    if (side != 0 && side != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
        return 1;
    }
    int enterLimit = 0;
    if (side == 1)  // 加入
    {
        // ORIG 直调 PowerParameterScript::checkPowerWarServer(a=+0x378 serverGroup,
        // b=+0x1b0 channelNo)，G_CEnvironment() 每次重物化（P183）；禁指针缓存与
        // 发明 helper（§4.1）。m_serverGroup 于 Server_Envir+0x1e0 ⇒ CEnvironment+0x378，
        // m_channelNo 于 Server_Envir+0x18 ⇒ CEnvironment+0x1b0。
        if (G_CDataManager()->m_powerParameter.checkPowerWarServer(
                G_CEnvironment()->m_serverEnvir.m_serverGroup,
                G_CEnvironment()->m_serverEnvir.m_channelNo) != 1)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x80);
            return 1;
        }
        if (!GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x7f);
            return 1;
        }
        enterLimit = CheckPowerWarEnterLimit(user);
        // [b28d-solo] P225：ORIG `cmpl $0,slot; setne %al; test %al,%al; je`（4.1
        // bool 物化后条件分支）唯一复现形 = while (v != 0) { body; break; }。
        while (enterLimit != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, (unsigned char)(enterLimit & 0xff));
            return 1;
        }
        // [b28d-solo] P198①：ORIG 在 G_CDataManager call 前有 `movl $0x0,-0x1c`
        // 预清 ⇒ 声明与赋值必须两句式。
        int cooldown = 0;
        cooldown = G_CDataManager()->m_powerParameter.m_powerWarCooldownMinutes * 0x3c;
        // [b28d-solo] P0/P23：ORIG 无 lastPlay 具名槽（getCurSec→%ebx 跨 call 保活、
        // lastPlay 直落 %eax 后 sub），源码须内联右值（左操作数先求值）。
        if (GlobalData::s_systemTime_.getCurSec() -
                user->getCurCharacLastPlayTickPowerWar() <= cooldown)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 0x84);
            return 1;
        }
        if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
            return 0;
        }
        if (user->IsBusyDisjointStore())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc3, 1);
            return 0;
        }
    }
    user->setCurrCharacJoinPower(side);
    // [b28d-solo] P292 档②：ORIG `movzbl slot,%eax; xor $1,%eax; test %al,%al; je`
    // ⇒ 局部须 bool（char 产 `cmpb $0` 直比）。
    bool isOriginal = false;
    // ORIG 两次 getPowerSide()（cmp $3 / cmp $4 独立调用，P183 反向勿缓存）。
    if (user->getPowerSide() == 3 || user->getPowerSide() == 4)
        isOriginal = true;
    if (side == 1)
    {
        if (isOriginal)
        {
            // [b28d-solo] P0/P52：ORIG 三值全经 callee-saved（%edi/%esi/%ebx）跨
            // GetServerProxy 调用保活，无栈槽 ⇒ 具名局部必须内联为实参表达式。
            GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendApplyOriginalPowerSide((unsigned int)user->getCurCharacNo(),
                                             (unsigned int)user->get_charac_guildkey(),
                                             (unsigned char)user->getPowerSide());
        }
        user->SetPowerWarLastPlayTime(GlobalData::s_systemTime_.getCurSec());
        user->setPowerWarHP((unsigned char)user->getCurCharacStamina());
        user->resetStraightVictories();
        GlobalData::s_power_manager->IncreasePowerUserCount((char)user->getPowerSide());
    }
    else
    {
        GlobalData::s_power_manager->DecreasePowerUserCount((char)user->getPowerSide());
        user->setCurCharacLastPlayTickPowerWar(GlobalData::s_systemTime_.getCurSec());
        user->SetCurCharacChaosDieTime(0);
        user->m_characterView.enableSaveCharacView();
    }
    if (!isOriginal)
    {
        PacketGuard pkt;
        pkt.put_header(1, 0xc3);
        pkt.put_byte(1);
        pkt.put_byte((int)side);
        pkt.finalize(true);
        user->Send(pkt);

        PacketGuard pkt2;
        pkt2.put_header(0, 2);
        pkt2.put_byte(0);
        pkt2.put_short(1);
        user->make_basic_info((char*)&pkt2, 0);
        pkt2.finalize(true);
        G_GameWorld()->send_all(pkt2, user);
    }
    return 1;
