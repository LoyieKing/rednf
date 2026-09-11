// ============================================================================
// ORIG T 0x081eed08 _ZN18DisPatcher_ReqPeer12dispatch_sigEP5CUserR9PacketBuf
// （docs/class_func_reports/DisPatcher_ReqPeer/dispatch_sig.md，1379 insn）
// state∈{3,6,8} 门禁 → 三字段读取 → 发起方门禁（私摊/分解店/幽灵/整合 PvP）→
// target 查找与 solo-play/游戏选项拒绝 → check_party_teleport_user(reqType,
// user,target,true) → 按 reqType 分派：0=组队、1=交易、2=PvP 房间、
// 3=战争房间、4=袭击请求、5/7 与 6/8=expert_job。成功尾部统一
// SetReceivedRequestType(target, reqType)。MSG_REQ_TO_PEER 仅作 SoloPlay
// 拒绝包上下文（类型未建模，零缓冲视图【推断】）。
// §6.2 巨件核销：P167 194/194 [0缺0多]；gap 1379→1379/1387；残差归因 N82/P112
// （4.1 不复用槽 vs 4.4 复用 + 块排布，diff 形态计数对称无缺类，详见
// docs/asm_translation_patterns.md 负例表）。无真修条目。
// [b28d-29] P30/N4 + N82：ORIG 结构体基址 lea -0x50 + add $0xd/0xf/0x10 两步，4.4 折独立 lea；语义集合全等  (ORIG 0x081eed08 insn 1379->1387)
// ============================================================================
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 686（ORIG 1377 / ours 1385），残差=setcc-fold bool 物化岛/P168a④ bool 槽化/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
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
    // [K6-DpcSigSmallCheckErr] 改前形 = 三变量带 `= 0` 预清 → 改后形 = 去预清
    // （值在 get_* 成功前不读，失败路径即 return，语义不变）。依据 ORIG
    // 0x081eed65 `lea -0x50(%ebp),%eax; add $0xd,%eax`（msg 视图成员取址，视图
    // base -0x50、peerIdx@+0xd / requestType@+0xf / cookie@+0x10）——ORIG 全函数
    // 无 `movw/movb/movl $0x0,-0x4e/-0x4f/-0x54(%ebp)` 预清（我方 4.4 多发这 3 条）。
    // 残差：成员取址两步形（ORIG lea+add）我方仍为单条 disp = N102/N4 族。
    unsigned short peerIdx;                      // ORIG msg+0xd（local_47）
    char requestType;                            // ORIG msg+0xf（local_45）
    unsigned int cookie;                         // ORIG msg+0x10（local_44）
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

    // [DPC_SigA] 语义修正：ORIG local_54 是**栈上未初始化对象**
    // （Ghidra `MSG_REQ_TO_PEER local_54 [13];`，全函数无 ctor/memset，
    // 直接作实参传给 alonePlay_Mode / get_SoloPlay_msgType）。
    // 原 `static char soloCtx[0x14]`（BSS 零初始化、跨调用共享）既是发明代码，
    // 又使第二次调用读到的"上下文"与 ORIG 不同（ORIG 每次是新的栈对象）。
    // 注：本 TU 只有 MSG_REQ_TO_PEER 前向声明，故以 13B 栈缓冲 + 视图引用复现。
    char soloCtx[0xd];
    const MSG_REQ_TO_PEER& soloMsg =
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
    if (target->alonePlay_Mode(soloMsg))
    {
        user->Send_SoloPlay_Reject_NotiPack(
            target->getCurCharacName(),
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(1),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(
                target->get_SoloPlay_msgType(soloMsg)));
        return 0;
    }
    int optionMap = 0x24;                        // 默认：无对应禁项
    // [P5-DPC 2026-09-11] P251/P261 真 switch 轴：ORIG 0x081ef007..0x081ef04c 为 -O0
    // switch 决策树（`movzbl -0x41; movsbl %al,%eax`；`cmp $0x1,%eax; je <0x1f>`；
    // `cmp $0x1,%eax; jg <hi>`；`test %eax,%eax; je <6>`；`jmp <end>`；
    // hi: `cmp $0x2,%eax; je <7>`；`cmp $0x3,%eax; je <8>`；`jmp <end>`；
    // 各 case 落 `movl $K,-0x34(%ebp); jmp <end>`）⇒ 原码是 case 0/1/2/3 的 switch；
    // 我方旧 if/else-if（含 `< 2` 分段）令 4.4 出 `cmp $0x2,%al` 字节比较链。
    switch (requestType)
    {
    case 0:
        optionMap = 6;
        break;
    case 1:
        optionMap = 0x1f;
        break;
    case 2:
        optionMap = 7;
        break;
    case 3:
        optionMap = 8;
        break;
    default:
        break;
    }
    if (optionMap != 0x24 &&
        target->GetGameOptionRef()->get_etc_option(
            static_cast<ENUM_OPTION_ETC>(optionMap)) != 0)
    {
        user->Send_SoloPlay_Reject_NotiPack(
            target->getCurCharacName(),
            static_cast<ENUM_SOLO_PLAY_REJECT_MODE>(0),
            static_cast<ENUM_SOLO_PLAY_REJECT_MSG>(
                target->get_SoloPlay_msgType(soloMsg)));
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
            // [G5-SigA] P0/P328：ORIG 0x081ef0xx 为 `call getCurCharacVill;
            // cmp $0x7,%al; jne`（直接 8 位比较、**无具名槽**）。原
            // `signed char selfVill` + `bool bothSeven` 两个具名量多出
            // `mov %al,-0x2e(%ebp); cmpb $0x7,-0x2e(%ebp)` 与
            // `mov %al,-0x2d(%ebp); cmpb $0x0,-0x2d(%ebp)` 共 4 条（fn_diff_tu 实测）。
            if (user->getCurCharacVill() == 7 &&
                target->getCurCharacVill() == 7)
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
}
