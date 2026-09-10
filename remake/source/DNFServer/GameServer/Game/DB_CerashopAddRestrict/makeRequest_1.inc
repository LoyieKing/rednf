void DB_SaveCerashopAddRestrictType::makeRequest(
    CUser* user, CerashopAddRestrict::DBInputData& data)
{
    // [b28d-46 判词·N/N18a] 沿用 b28c-33：213 vs 215，p167 CLEAR；两处
    // `mov 0x4(%eax),%ebx` 单段直落 vs 4.4 `mov 0x4(%eax),%eax; mov %eax,%ebx`
    // 两段（N18a；字段/callee 返回类型两旋钮已试）⇒ 封顶。ORIG 0x080e17ba。
    // [b28c-33 判词 NEAR-N18a] 两处 `operator<<(reinterpret_cast<paramDaily*>(
    // data.m_result)->m_count/m_nextDate)`：ORIG 为 `mov 0x4(%eax),%ebx` 单段直落
    // callee-saved；4.4 恒 `mov 0x4(%eax),%eax; mov %eax,%ebx`（+1×2）。其余全等
    // （213 vs 215，实质差异仅此两处，余为重定位/注释）。
    switch (data.m_pInfo->m_type)
    {
    case 0:
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x18c), true);
        guard.operator*()->operator<<(0x340);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<((unsigned int)data.m_pInfo->m_type);
        guard.operator*()->operator<<(data.m_pInfo->m_ipgNo);
        guard.operator*()->operator<<((unsigned int)
            reinterpret_cast<CerashopAddRestrict::paramDaily*>(data.m_result)->m_count);
        guard.operator*()->operator<<((long)
            reinterpret_cast<CerashopAddRestrict::paramDaily*>(data.m_result)->m_nextDate);
        guard.operator*()->operator<<((long)data.m_pInfo->m_end);
        guard.operator*()->operator<<(user->get_acc_id());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        break;
    }
    case 1:
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "CerashopAddRestrictType.cpp", 0x19a), true);
        guard.operator*()->operator<<(0x340);
        guard.operator*()->operator<<(-1);
        guard.operator*()->operator<<((unsigned int)data.m_pInfo->m_type);
        guard.operator*()->operator<<(data.m_pInfo->m_ipgNo);
        guard.operator*()->operator<<((unsigned int)
            reinterpret_cast<CerashopAddRestrict::paramDaily*>(data.m_result)->m_count);
        guard.operator*()->operator<<((long)
            reinterpret_cast<CerashopAddRestrict::paramDaily*>(data.m_result)->m_nextDate);
        guard.operator*()->operator<<((long)data.m_pInfo->m_end);
        guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        break;
    }
    default:
        break;
    }
