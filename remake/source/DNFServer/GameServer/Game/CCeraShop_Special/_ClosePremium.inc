// ============================================================================
// _ClosePremium —— ORIG 0x08327bb6 (T)
// 关闭 type 指定 premium：取 GetPremiumInfo(type)，非空则逐项投递 LOGDB_Q
// （0x54 头 + uid + accId + type + curSec + curSec+1 + flag + serverCode），
// 并调 CHandlePremium::handleSetUserPremium / handlePremiumEffect /
// handleNotifyPremiumInfo。
// ============================================================================
void CCeraShop::_ClosePremium(CUser* user, int type)
{
    WongWork::CUserPremium* premium = user->GetPremiumInfo();
    std::vector<WongWork::SUserPremiumInfo> list;
    // ORIG 0x08327be4/0x8327bfa：GetPremiumInfo(type) 判空与取值两次独立调用
    // （P48 无 CSE）。
    if (premium->GetPremiumInfo(type)->m_type != 0)
        list.push_back(*premium->GetPremiumInfo(type));

    int curSec = GlobalData::s_systemTime_.getCurSec();
    int nextSec = curSec + 1;
    unsigned int serverCode = 0;
    // ORIG 0x8327c4e..：非 const iterator for（begin 一次、end 逐轮、7×
    // operator-> 逐点重发，P157/P175 —— 禁缓存引用/索引）。
    for (std::vector<WongWork::SUserPremiumInfo>::iterator it = list.begin();
         it != list.end(); ++it)
    {
        serverCode = 0;
        // m_useTargetServer(+0x5c)==1 → serverCode = m_serverEnvir.m_serverGroup
        // （CEnvironment+0x378）
        if (g_SPremiumInfo[it->m_type].m_useTargetServer == 1)
        {
            serverCode = (unsigned int)G_CEnvironment()->m_serverEnvir.m_serverGroup;
        }
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x20e7),
                               true);
        guard.operator*()->operator<<((int)0x54);   // 0x54 头（ORIG lsEi）
        guard.operator*()->operator<<((int)user->GetUID());   // lsEi
        guard.operator*()->operator<<((unsigned int)user->get_acc_id());  // ORIG lsEj
        guard.operator*()->operator<<((int)it->m_type);   // lsEi
        guard.operator*()->operator<<((long)curSec);    // ORIG lsEl
        guard.operator*()->operator<<((long)nextSec);   // ORIG lsEl
        guard.operator*()->operator<<((int)it->m_flag);   // lsEi
        guard.operator*()->operator<<((unsigned int)serverCode);  // ORIG lsEj
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        WongWork::CHandlePremium::handleSetUserPremium(
            user, (ENUM_PREMIUM_TYPE)it->m_type, curSec, nextSec, it->m_flag, false);
        WongWork::CHandlePremium::handlePremiumEffect(user,
            (ENUM_PREMIUM_TYPE)it->m_type);
        WongWork::CHandlePremium::handleNotifyPremiumInfo(user, it->m_type, 0);
    }
}