int DisPatcher_SelectCharac::RequestDBETC(CUser* user, MSG_CHARAC_SELECT& msg)
{
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x1814), true);
    **guard << 0x20;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_ETC);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_ETC* sig = guard->GetInBuffer<SIG_LOAD_ETC>();
    memset(sig, 0, sizeof(SIG_LOAD_ETC));
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_serverGroup =
        G_CEnvironment()->m_serverEnvir.m_serverGroup;      // ORIG 读 ENV+0x378
    sig->m_channelNo = G_CEnvironment()->get_channel_no();
    sig->m_fieldC = msg.m_0e;
    sig->m_tagEtc.clear();
    reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill)->m_job =
        user->GetCharacJob(static_cast<int>(static_cast<signed char>(msg.m_slot)));
    // [b28e-L5] C05-① 复查实测**不命中**（已回退）：本件 ORIG 尾部同样是两分支
    // `mov $0x1,%ebx; jmp 0x081c7822` 共享尾块，但套形后 our_insns 153→155（ORIG 152）
    // 且 TU 四数不变 ⇒ 残差主体为 P254/N18a 中转与帧差，保持原形。
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
