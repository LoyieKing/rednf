int Dispatcher_RequestAssaultPrice::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);

    CUser* target = G_GameWorld()->find_from_world((unsigned short)m.m_0d);

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0xbf;
    }
    if (user->IsCurCharacGhost())
    {
        return 0x13;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0x13;
    }
    if (target == 0)
    {
        return 3;
    }
    if (target->IsBusyDisjointStore())
    {
        return 0xc0;
    }

    // ORIG 0x082efe14：GetInstanceAssaultMgr()->check_party_teleport_user(4, user, target, false)
    int tpErr = pvp_assault::GetInstanceAssaultMgr()->check_party_teleport_user(4, user, target, false);
    if (tpErr > 0)
    {
        return tpErr;
    }

    const int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x37);
    if (secuErr != 0)
    {
        return secuErr;
    }

    if (target->get_state() > 4)
    {
        return 0x7fffffff;
    }

    // ORIG 分支语义:请求者非 GM 且目标为 GM → 拒绝（ORIG 无 bool 栈变量,直接判断）。
    if (!GlobalData::s_pGMAccounts_->isGM(user->get_acc_id())
        && GlobalData::s_pGMAccounts_->isGM(target->get_acc_id()))
    {
        return 0x7fffffff;
    }

    // ORIG 0x082edfba：GetInstanceAssaultMgr()->OnRequestAssault(user, target, &tpErr)；
    // tpErr 与 check_party_teleport_user 返回值复用同一栈槽（ORIG -0x18）。
    pvp_assault::GetInstanceAssaultMgr()->OnRequestAssault(user, target, &tpErr);
    if (tpErr != 0)
    {
        return tpErr;
    }
    return 0;
