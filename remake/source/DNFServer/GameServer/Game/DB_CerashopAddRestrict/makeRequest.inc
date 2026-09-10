void DB_LoadCerashopAddRestrictType::makeRequest(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CerashopAddRestrictType.cpp", 500), true);
    guard.operator*()->operator<<(0x341);                      // SIG_LOAD_CERASHOP_ADD_RESTRICT_TYPE
    guard.operator*()->operator<<((int)user->GetUID());                     // operator<<(int)
    guard.operator*()->operator<<(user->get_acc_id());                      // operator<<(uint)
    guard.operator*()->operator<<((unsigned int)user->getCurCharacNo());    // operator<<(uint)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
