void CUser::UpdateBloodRecord()
{
    _Charac_info* cur = const_cast<_Charac_info*>(getCurCharacR());
    if (cur == 0)
        return;
    for (std::map<int, stBloodBestRecord>::iterator it = cur->m_bloodBestRecords.begin();
         it != cur->m_bloodBestRecords.end(); ++it)
    {
        // P259：ORIG 0x08651e43 无 stream 具名局部（Acquire 返回值直传 C1 的
        // 0x4(%esp)）⇒ 必须内联实参，否则多一个 -0x10 槽并使全帧槽号平移 -4。
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("user.cpp", 0x1bbb), true);
        // P132/P103：ORIG 0x08651e60/0x08651e9f 显式调 CStreamGuard::operator*()
        // （`de`×2）与 operator->()（`pt`×1）再转发 ⇒ 必须保留代理链
        // （直接 `guard <<`/`guard.GetInBuffer` 少 3 条 call）。
        (*guard)->operator<<(0x140);
        (*guard)->operator<<((int)GetUID());
        BloodBestRecordSigView* sig = reinterpret_cast<BloodBestRecordSigView*>(
            guard.operator->()->GetInBuffer<SIG_UPDATE_BLOOD_DUNGEON_BEST_RECORD>());
        memset(sig, 0, 0x10);
        sig->m_characNo = getCurCharacNo();
        sig->m_time = it->second.m_time;
        sig->m_round = it->second.m_round;
        sig->m_key = it->first;
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    setBloodBestRecordUpdate(false);
}