int DisPatcher_MoveMap::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stMoveMapParam& out = static_cast<stMoveMapParam&>(param);
    // [b28d-solo-DisPatcherCore] P211：ORIG `mov %eax,%edx; mov <out>,%eax; mov %edx,0x4(%eax)`。
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo >= 1)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {   // LineFunc(0x1268, pretty, m_errNo, acc_id)；ORIG 直接返回其值
        return LineFunc(0x1268,
                        __PRETTY_FUNCTION__,
                        out.m_errNo, user->get_acc_id());
    }
    MoveMapMsgView& req = *reinterpret_cast<MoveMapMsgView*>(&msg);
    // ORIG 0x081...：`movzbl 0x1a(%eax),%eax; xor $0x1,%eax; test %al,%al; je` =
    // bool 字段 ! 物化（唯 bool lvalue 产 xor$1；uchar 出 cmp$1）。视图字段判 + 恢复传参。
    if (!req.m_1a)
    {   // 客户端未置 0x1a 标志 → 记 0x25a
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25a, 1, 0, 0);
    }
    static_cast<Secu_HackLogCheck*>(user->GetHackCheck())->MoveMap();

    CheckTrapBitsHackCnt(user, *reinterpret_cast<MSG_MOVE_MAP*>(&req));
    CheckOneMapPlayTimeHackCnt(user, *reinterpret_cast<MSG_MOVE_MAP*>(&req));

    if (Global::CBossTowerFunc::MoveMap(user, req.m_1b))
    {   // 死亡塔/Boss 塔自行处理换图
        return 0;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party->IsRoutingState())
    {
        CSpecialItemRoutingManager* mgr =
            G_CGameManager()->GetSpecialItemRoutingManager();
        if (mgr != 0)
        {
            mgr->ProcessFailEtc(party, 0);
        }
    }
    req.m_39 = (unsigned char)party->get_party_seatno(user);
    party->move_map(*reinterpret_cast<MSG_MOVE_MAP*>(&req));
    return 0;
}
