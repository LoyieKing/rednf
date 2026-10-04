// ORIG T 0x08651740：角色视图变化落库包。频道 !=7 且 isSaveCharacView() 时组
// SIG_UPDATE_CHARAC_VIEW（header 6 + UID），memset 全块后逐 _Charac_info 填
// 记录（stride 0x94）并调 updateCharacViewVisibleData，投递 DB_Q 后清保存标志。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08651740 | _ZN5CUser16UpdateCharacViewEv
// [b28d-GrpD 判词·结构化 N] ORIG 0x08651740：残差为槽化引起的地址计算式差异 ——
//   ORIG 每次 `imul $0x94; lea (%R,%R,1); add $0x10/$0x4` 逐步折基址，我方用
//   `imul $0x94; add -0xS` + 折叠副偏移（0x1c=0x10+0xc、0x22=0x10+0x12、0x98=0x94+4）；
//   两式同址、位掩码（$BIG 的 and/or 位域改写）与字段偏移一致 ⇒ 无语义缺口。
void CUser::UpdateCharacView()
{
    if (G_GameWorld()->GetChannelType() == 7)
        return;
    if (!m_characterView.isSaveCharacView())
        return;

    // b28d-lane5：ORIG 0x08651740 无 Stream* 槽——Acquire 结果 %eax 直传 ctor（P0）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1b1f), true);

    // b28d-solo-CUser p167：ORIG 0x08651740 两次 `CStreamGuard::operator*()` 后
    //   `operator<<`（MISSING ×2）⇒ 显式 operator* 调用链（P132）。
    guard.operator*()->operator<<((int)6);
    guard.operator*()->operator<<((int)GetUID());

    // b28d-CUser p167：ORIG 0x086517f7 显式调 CStreamGuard::operator->()（ptEv）
    //   再 GetInBuffer（P103）；直接成员调用少一次 call。
    SIG_UPDATE_CHARAC_VIEW* sig =
        guard.operator->()->GetInBuffer<SIG_UPDATE_CHARAC_VIEW>();
    memset(sig, 0, sizeof(SIG_UPDATE_CHARAC_VIEW));

    sig->m_serverGroup = (unsigned char)GetServerGroup();

    sig->m_mId = get_acc_id();
    SigUpdateCharacViewRecord* recs = sig->m_recs;

    // ORIG 无任何缓存：目标记录每次都按 sig + recIdx*0x94 + 偏移重算，源字段每次
    // 都重新 it.operator->()（Ghidra C 每个字段前都有一次 __normal_iterator::
    // operator->）；begin()/end() 各调一次转成 const_iterator 后再进循环。
    int recIdx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator endIt = m_characList.end();
    for (; it != endIt; ++it)
    {
        recs[recIdx].m_data.m_characNo = it->m_characNo;
        strncpy(recs[recIdx].m_data.m_name, it->m_name, 0x1e);
        recs[recIdx].m_data.m_job = (unsigned char)it->m_job;
        recs[recIdx].m_data.m_field19 = (unsigned char)it->m_growthType.m_growthType;
        recs[recIdx].m_data.m_level = it->m_level;

        // b28d-solo-CUser p167：ORIG 0x08651740 全字段走 `it.operator->()`
        //   （MISSING ×5；我方 `*it` 走 operator*，EXTRA ×5）⇒ 改显式 operator-> 形。
        updateCharacViewVisibleData(*(it.operator->()), recs[recIdx].m_data);

        recs[recIdx].m_data.m_lastPlayTime =
            (long)(*reinterpret_cast<const CommonTime*>(
                it.operator->()->m_delReqTime));
        recs[recIdx].m_data.m_field8c = it.operator->()->m_chaosDieTime;

        // bit0 ← 禁玩标记（m_pad1240 非零）。
        recs[recIdx].m_data.m_flags = (unsigned char)(
            ((unsigned int)recs[recIdx].m_data.m_flags & 0xfeu) |
            ((it.operator->()->m_pad1240 != 0) ? 1u : 0u));

        recs[recIdx].m_data.m_field1d = it->m_strikerSkillIndex;
        recs[recIdx].m_data.m_field1e = it->m_tagCharacNo;

        // bit4 ← m_tagVisibleFlags bit1（ORIG shl $6 / sar $7 取位）。
        recs[recIdx].m_data.m_flags = (unsigned char)(
            ((unsigned int)recs[recIdx].m_data.m_flags & 0xefu) |
            (((((unsigned int)(unsigned char)
                    it.operator->()->m_tagVisibleFlags.m_flag
                << 6) >> 7) & 1u) << 4));

        recs[recIdx + 1].m_headField = it.operator->()->m_chaosResponTime;

        ++recIdx;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_characterView.disableSaveCharacView();
}