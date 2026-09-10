void CUser::updateComboSkill()
{
    if (get_charac_job() != 9)
        return;
    // ORIG 0x08692396：Acquire 结果 %eax 直传 ctor（P0 无 stream 槽）；
    // 每 op<< 前 operator* 重取（P48），put 前 operator-> 重取。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xb697),
                       true);
    guard.operator*()->operator<<((int)0x31d);
    guard.operator*()->operator<<((int)-1);
    // ORIG：operator-> 重取 → GetInBuffer call → sig 落槽 -0xc → reset() →
    // getCurCharacSkillR → getComboSkillSaveData(sig)。
    SIG_COMBO_SKILL* sig = guard.operator->()->GetInBuffer<SIG_COMBO_SKILL>();
    sig->reset();
    ((SkillSlot*)getCurCharacSkillR())->getComboSkillSaveData(sig);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
