int DisPatcher_ReqPeer::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG top gate：三次独立 get_state reload（==3/==6/==8 各自 `call
    // _ZN5CUser9get_stateEv`），不缓存 `st`——调用点计数须为 3。
    if (user->get_state() != 3 &&
        user->get_state() != 6 &&
        user->get_state() != 8)
    {
        return 0;
    }
    PacketGuard pgRes;
    unsigned short peerIdx = 0;                  // ORIG msg+0xd（local_47）
    char requestType = 0;                        // ORIG msg+0xf（local_45）
    unsigned int cookie = 0;                     // ORIG msg+0x10（local_44）
    if (!packet.get_short(peerIdx))
        return LineFunc(0x725a,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(requestType))
        return LineFunc(0x725b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(cookie))
        return LineFunc(0x725c,
                        __PRETTY_FUNCTION__,
                        0, 0);

    static char soloCtx[0x14];                   // ORIG local_54 上下文【推断】
    const MSG_REQ_TO_PEER& soloRef =
        *reinterpret_cast<const MSG_REQ_TO_PEER*>(soloCtx);

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return PEER_ERR3(user,requestType,0x3c), 0;
    if (user->IsBusyDisjointStore())
        return PEER_ERR3(user,requestType,0xbf), 0;
    if (user->IsCurCharacGhost())
        return PEER_ERR3(user,requestType,0x13), 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() && requestType != 2)
        return PEER_ERR3(user,requestType,0x13), 0;
    CUser* target = G_GameWorld()->find_from_world(peerIdx);
    if (target == 0)
        return PEER_ERR3(user,requestType,3), 0;
    if (target->alonePlay_Mode(soloRef))
    {
        user->Send_SoloPlay_Reject_NotiPack(
            target->getCurCharacName(),
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(1),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(
                target->get_SoloPlay_msgType(soloRef)));
        return 0;
    }
    int optionMap = 0x24;                        // 默认：无对应禁项
    if (requestType == 1)
        optionMap = 0x1f;
    else if (requestType < 2)
    {
        if (requestType == 0)
            optionMap = 6;
    }
    else if (requestType == 2)
        optionMap = 7;
    else if (requestType == 3)
        optionMap = 8;
    if (optionMap != 0x24 &&
        target->GetGameOptionRef()->get_etc_option(
            static_cast<ENUM_OPTION_ETC>(optionMap)) != 0)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            target->getCurCharacName(),
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(0),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(
                target->get_SoloPlay_msgType(soloRef)));
        return 0;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        // ORIG 两次独立 reload（==6 / ==7）。
        if (target->get_state() == 6 || target->get_state() == 7)
            return PEER_ERR3(user,requestType,0x13), 0;
    }
    if (target->IsBusyDisjointStore())
        return PEER_ERR3(user,requestType,0xc0), 0;
    int tpErr = pvp_assault::GetInstanceAssaultMgr()
                    ->check_party_teleport_user(requestType, user, target,
                                                true);
    if (tpErr > 0)
        return 0;                                // 错误包由 check 内部发送
    switch (requestType)
    {
    case 0:
    {
        if (user->get_state() == 5 ||
            user->m_network.get_nat_type() == 0)
            return PEER_ERR3(user,requestType,0x13), 0;
        if (target->isBlackUser(
                static_cast<unsigned int>(user->get_charac_no(-1))))
            return PEER_ERR3(user,requestType,0x4d), 0;
        if (user->isCompetitionMercenary())
            return PEER_ERR3(user,requestType,0xea), 0;
        if (target->isCompetitionMercenary())
            return PEER_ERR3(user,requestType,0xbe), 0;
        if (private_store::GetInstancePrivateStoreMgr()
                ->IsBusyPrivateStore(target))
            return PEER_ERR3(user,requestType,0x3d), 0;
        if (target->IsCurCharacGhost())
            return PEER_ERR3(user,requestType,0x13), 0;
        if (target->get_state() == 5 ||
            target->m_network.get_nat_type() == 0)
            return PEER_ERR3(user,requestType,3), 0;
        if (GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            // ORIG 6 次 `_ZNK getCurCharacVill` 内联（无缓存局部）；语义等价于
            // （双 7 且 power 侧异）∨（恰一 7）→ 拒 0x13。逐调用点展开匹配计数。
            signed char selfVill = user->getCurCharacVill();
            bool bothSeven = selfVill == 7 && target->getCurCharacVill() == 7;
            if (bothSeven)
            {
                if (user->getPowerSide() != target->getPowerSide())
                    return PEER_ERR3(user,requestType,0x13), 0;
            }
            else if ((user->getCurCharacVill() == 7 ||
                      target->getCurCharacVill() != 7) &&
                     (user->getCurCharacVill() != 7 ||
                      target->getCurCharacVill() == 7))
            {
                // 同 7 / 同非 7 → 通过
            }
            else
            {
                return PEER_ERR3(user,requestType,0x13), 0;
            }
        }
        CParty* selfParty = static_cast<CParty*>(user->GetParty());
        // ORIG 两个独立 0x13 错误调用点（count>3 与 !CanDo 各一）。
        if (selfParty != 0)
        {
            if (selfParty->get_member_count() > 3)
                return PEER_ERR3(user,requestType,0x13), 0;
            if (!selfParty->CanDoPartyActionAssalutState())
                return PEER_ERR3(user,requestType,0x13), 0;
        }
        if (user->CheckInParty() && target->CheckInParty())
            return PEER_ERR3(user,requestType,0x12), 0;
        if (!GlobalData::s_power_manager->IsPowerWarEventOn())
        {
            int curSec = GlobalData::s_systemTime_.getCurSec();
            // ORIG 三处 `_ZN10expert_job10CAlchemist17GetAssaultPreventEP5CUserPb`
            // 第二参为 (bool*)0x0。
            if (expert_job::CAlchemist::GetAssaultPrevent(
                    user, static_cast<bool*>(0)))
            {
                if (curSec < target->GetCurCharacChaosKillTime() + 600)
                    return PEER_ERR3(user,requestType,0xfa), 0;
                CParty* peerParty =
                    static_cast<CParty*>(target->GetParty());
                if (peerParty != 0 &&
                    peerParty->CheckMemberLastTryAssaultTime())
                    return PEER_ERR3(user,requestType,0xfa), 0;
            }
            if (curSec < user->GetCurCharacChaosKillTime() + 600 &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    target, static_cast<bool*>(0)))
                return PEER_ERR3(user,requestType,0xfa), 0;
            if (selfParty != 0 &&
                selfParty->CheckMemberLastTryAssaultTime() &&
                expert_job::CAlchemist::GetAssaultPrevent(
                    target, static_cast<bool*>(0)))
                return PEER_ERR3(user,requestType,0xfa), 0;
        }
        unsigned short spareShort = 0;           // ORIG local_62 覆盖式读
        packet.get_short(spareShort);            // ORIG 忽略返回值
        pgRes.clear();
        pgRes.put_header(0, 7);
        pgRes.put_short(user->get_unique_id() & 0xffff);
        pgRes.put_byte(0);
        pgRes.put_int(static_cast<int>(cookie));
        pgRes.put_short(
            static_cast<int>(user->getCurCharacTotalMaxFatigue()) -
            static_cast<int>(user->getCurCharacTotalFatigue()));
        pgRes.put_short(
            static_cast<int>(user->getCurCharacStamina()) & 0xff);
        pgRes.put_short((int)(unsigned short)spareShort);
        pgRes.finalize(true);
        target->Send(pgRes);
        break;
    }
    case 1:
    {
        // ORIG local_28 延迟错误码：非交易态先试同 state 直连成功包，失败累计
        // 码值后统一 fall-through 发包。两处 `Get()+isRestricted`、两处
        // `get_state`、两处 `CheckInTrade`。
        int errCode = 1;
        bool restricted =
            ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, static_cast<RestrictCategory::Enum>(1), 6);
        if (!restricted)
            restricted =
                ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                    target, static_cast<RestrictCategory::Enum>(1), 6);
        if (restricted)
            return PEER_ERR3(user,requestType,0xd1), 0;
        int secuErr =
            GlobalData::s_pSecuProtectionField->CheckOpposite(
                user, static_cast<SECURITY_PROTCTION>(0x36), target);
        if (secuErr != 0)
            return PEER_ERR3(user,requestType,secuErr & 0xff), 0;
        if (private_store::GetInstancePrivateStoreMgr()
                ->IsBusyPrivateStore(target))
            return PEER_ERR3(user,requestType,0x3d), 0;
        if (target->IsCurCharacGhost())
            return PEER_ERR3(user,requestType,0x13), 0;
        if (target->isBlackUser(
                static_cast<unsigned int>(user->get_charac_no(-1))))
            return PEER_ERR3(user,requestType,0x4d), 0;
        if (!target->CheckInTrade())
        {
            if (user->get_state() == target->get_state())
            {
                pgRes.put_header(0, 7);
                pgRes.put_short(user->get_unique_id() & 0xffff);
                pgRes.put_byte(1);
                pgRes.put_int(static_cast<int>(cookie));
                pgRes.put_int(
                    static_cast<int>(user->getCurCharacCreateTime()));
                pgRes.finalize(true);
                target->Send(pgRes);
                target->SetReceivedRequestType(requestType);
                return 0;
            }
            errCode = 0x13;
        }
        if (target == 0)
            errCode = 3;
        else if (target->CheckInTrade())
            errCode = 0x12;
        PEER_ERR3(user,requestType,errCode & 0xff);
        break;
    }
    case 2:
    {
        if (user->GetPVPRoom() == 0)
            return LineFunc(0x7494,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (private_store::GetInstancePrivateStoreMgr()
                ->IsBusyPrivateStore(target))
            return PEER_ERR3(user,requestType,0x3d), 0;
        if (target->IsCurCharacGhost())
            return PEER_ERR3(user,requestType,0x13), 0;
        if (target->isCompetitionMercenary())
            return PEER_ERR3(user,requestType,0xbe), 0;
        if (target->get_state() > 4)
            return 0;
        pgRes.put_header(0, 7);
        pgRes.put_short(user->get_unique_id() & 0xffff);
        pgRes.put_byte(2);
        pgRes.put_int((int)(unsigned short)peerIdx);
        pgRes.finalize(true);
        target->Send(pgRes);
        break;
    }
    case 3:
    {
        WarRoom* war = static_cast<WarRoom*>(user->GetWarRoom());
        if (war == 0)
            return LineFunc(0x74bf,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (target->get_state() > 4)
            return 0;
        if (war->GetState() == 2)
            return 0;
        pgRes.put_header(0, 7);
        pgRes.put_short(user->get_unique_id() & 0xffff);
        pgRes.put_byte(3);
        pgRes.put_int((int)(unsigned short)peerIdx);
        pgRes.finalize(true);
        target->Send(pgRes);
        break;
    }
    case 4:
    {
        int secuErr =
            GlobalData::s_pSecuProtectionField->Check(
                user, static_cast<SECURITY_PROTCTION>(0x37));
        if (secuErr != 0)
            return PEER_ERR3(user,requestType,secuErr & 0xff), 0;
        if (target->get_state() > 4)
            return 0;
        if (GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) ||
            GlobalData::s_pGMAccounts_->isGM(target->get_acc_id()))
            return 0;
        if (!pvp_assault::GetInstanceAssaultMgr()->OnRequestAssault(
                user, target, 0))
            return 0x7590;
        break;
    }
    case 5:
    case 7:
    {
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, static_cast<RestrictCategory::Enum>(1), 8))
            return PEER_ERR4(user,requestType,0xd1,pgRes), 0;
        int secuErr =
            GlobalData::s_pSecuProtectionField->Check(
                user, static_cast<SECURITY_PROTCTION>(0x21));
        if (secuErr != 0)
            return PEER_ERR4(user,requestType,secuErr & 0xff,pgRes), 0;
        if (target->GetCurCharacExpertJobType() != 1)
            return PEER_ERR4(user,requestType,0x17,pgRes), 0;
        GlobalData::s_ExpertJobMgr->OnRequestPeer(
            user, target,
            static_cast<ENUM_PEER_REQUEST_TYPE>(
                static_cast<int>(requestType)),
            pgRes);
        break;
    }
    case 6:
    case 8:
    {
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, static_cast<RestrictCategory::Enum>(1), 8))
            return PEER_ERR4(user,requestType,0xd1,pgRes), 0;
        int secuErr =
            GlobalData::s_pSecuProtectionField->CheckOpposite(
                user, static_cast<SECURITY_PROTCTION>(0x20), target);
        if (secuErr != 0)
            return PEER_ERR4(user,requestType,secuErr & 0xff,pgRes), 0;
        if (user->GetCurCharacExpertJobType() != 1)
            return PEER_ERR4(user,requestType,0x17,pgRes), 0;
        GlobalData::s_ExpertJobMgr->OnRequestPeer(
            user, target,
            static_cast<ENUM_PEER_REQUEST_TYPE>(
                static_cast<int>(requestType)),
            pgRes);
        break;
    }
    default:
        break;
    }
    target->SetReceivedRequestType(requestType);
    return 0;
