// ORIG 0x0842801a sz 0x139（_ZN19DB_SaveUserHandicap11makeRequestEjiiiii）：
// Acquire("DBThread.cpp",0x698a) → CStreamGuard(stream,true)
//   → <<0xce(int) → <<-1(int) → sig=GetInBuffer<SIG_UPDATE_USER_HANDICAP>()
//   → sig+0x00(int)=param_2、+0x04(uint)=param_1(accId)、+0x08..+0x14(int)=param_3..6
//   → put(DB_Q(2))；随后 curSec=CSystemTime::getCurSec(s_systemTime_)，
//     DB_SaveFeaturedGoodsIdx::makeRequest(accId, 0, param_2, curSec, param_6)。
// [b28d-4] DIFF：N18a/P211：4.4 多 {'mov': 2} 中转 mov（单段直落/同型化待验）；ORIG 0842801a
void DB_SaveUserHandicap::makeRequest(unsigned int accId, int param2, int param3,
                                      int param4, int param5, int param6)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x698a), true);
    guard.operator*()->operator<<((int)0xce);
    guard.operator*()->operator<<((int)-1);
    // 残留（blocked N23 帧槽方向）：ORIG 0x08428xxx guard this=-0x14 比
    // sig(-0x18) 近；4.4 对含 dtor 对象恒置 guard 于更远槽，声明序
    // （sig 前置分离声明）探针无效已回退。
    SIG_UPDATE_USER_HANDICAP* sig = guard->GetInBuffer<SIG_UPDATE_USER_HANDICAP>();
    sig->m_param2 = param2;
    sig->m_accId = accId;
    sig->m_params[0] = param3;
    sig->m_params[1] = param4;
    sig->m_params[2] = param5;
    sig->m_params[3] = param6;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    // [J4S1 P329/P227] 删具名 curSec（ORIG 尾段 `call getCurSec; mov %eax,0xc(%esp)`
    // 无槽）⇒ 内联进实参位。
    DB_SaveFeaturedGoodsIdx::makeRequest(
        accId, 0, param2, GlobalData::s_systemTime_.getCurSec(), param6);
}