bool CFairPvPScore::_saveData(CUser* pUser)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("FairPvPScore.cpp", 0x10a),
                       true);

    **guard << 0x261;                       // SIG_FAIR_PVP_SCORE 头
    **guard << (int)pUser->GetUID();

    SIG_FAIR_PVP_SCORE* sig =
        guard.operator->()->GetInBuffer<SIG_FAIR_PVP_SCORE>();
    memset(sig, 0, sizeof(SIG_FAIR_PVP_SCORE));          // 0x688
    sig->m_characNo = pUser->get_charac_no(-1);          // ORIG *(int*)local_10 = m_characNo(+0x00)
    this->getData(reinterpret_cast<char*>(sig));         // ORIG vtable+8 (getData)；线格式序列化
    sig->m_lastPlayTime = GlobalData::s_systemTime_.getCurSec();   // +0x680

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return true;                                          // ORIG mov $0x1,%ebx
