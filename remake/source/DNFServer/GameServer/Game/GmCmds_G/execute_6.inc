void CUserPremiumDelete::execute()
{
    CUser* user = GetUser();
    WongWork::CUserPremium* premium = user->GetPremiumInfo();

    std::vector<WongWork::SUserPremiumInfo> list;
    for (int type = 1; type <= 0x69; ++type)
    {
        // P48：ORIG 0x084abf93 与 0x084abfb0 是两次独立 GetPremiumInfo 调用
        // （O0 无 CSE；缓存具名局部会少 1 次 call，p167 MISSING 实测）。
        if (premium->GetPremiumInfo(type)->m_type != 0)
            list.push_back(*premium->GetPremiumInfo(type));
    }

    long curSec = (long)GlobalData::s_systemTime_.getCurSec();
    long nextSec = curSec + 1;

    for (std::vector<WongWork::SUserPremiumInfo>::iterator it = list.begin();
         it != list.end(); ++it)
    {
        int serverCode = 0;
        if (g_SPremiumInfo[it->m_type].m_useTargetServer == 1)
            serverCode = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x378

        Stream* stream =
            GlobalData::s_stream_pool->Acquire("GameMaster.cpp", 0x989);
        CStreamGuard guard(stream, true);
        // ORIG operator<< 重载序：int / uint / uint / int / long / long /
        // int / uint（0x54 头 + uid + accId + type + curSec + nextSec +
        // flag + serverCode）。
        // P103/P132：ORIG 每处都先 call CStreamGuard::operator*（0x080c8c46）
        // 再 call CStreamGuard::operator<<（0x080c8c56/0x080e2276/0x080e2292）
        // ⇒ 必须保留 operator* 链（8 处；直接 `guard << x` 会少 8 次 call）。
        guard.operator*()->operator<<(0x54);
        guard.operator*()->operator<<(user->GetUID());
        guard.operator*()->operator<<(user->get_acc_id());
        guard.operator*()->operator<<(it->m_type);
        guard.operator*()->operator<<(curSec);
        guard.operator*()->operator<<(nextSec);
        guard.operator*()->operator<<(it->m_flag);
        guard.operator*()->operator<<(serverCode);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);

        WongWork::CHandlePremium::handleSetUserPremium(
            user, (ENUM_PREMIUM_TYPE)it->m_type, curSec, nextSec,
            it->m_flag, false);
        WongWork::CHandlePremium::handlePremiumEffect(
            user, (ENUM_PREMIUM_TYPE)it->m_type);
        WongWork::CHandlePremium::handleNotifyPremiumInfo(
            user, it->m_type, (int)(nextSec - curSec));
    }
