// ---- ORIG 0x84a57fe _saveData(CUser*) --------------------------------------
// [b28 BM2 判词·N7] AE-ndiff=6（fresh .o 归一，全二进制口径 12）：唯一差 =
// 虚槽调用（CUser 写流路径）ORIG 4.1 `mov (%eax),%edx; …; call *%edx` vs 4.4
// 恒 `call *%eax` + 实参装载寄存器对调（N7 间接 call 目标族，b26 NL28 实证）。
// 助记符/条数 87=87 全等；禁绑定形（P0/N96）。
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
}