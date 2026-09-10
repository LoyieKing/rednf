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
    // [DpcMisc-E1 · 2026-09-11 ndiff 111→103 落地] P243 书写序：
    // ORIG 0x081c7718..0x081c7736 先算 `get_charac_no((char)param_2[0xd])` 写 SIG+4，
    // 0x081c7739..0x081c7747 才算 `get_acc_id` 写 SIG+0（报告反编译 C 同序）；我方原形
    // acc_id 在前、charac_no 在后（.o 3c8a/3c9a 与 ORIG 物理序相反）。两字段互不依赖
    // （语义无关），纯书写序对齐。父泳道 fresh 核账：指令数 152/154 未动、ndiff
    // **111→103 改善** ⇒ 落地（曾按指令数回滚一次，ndiff 判定后恢复本形）。
    // p167 实测：ORIG calls=31 / OUR calls=31，差集仅 CStreamGuard C1/C2·D1/D2 同址别名
    // ⇒ 判据第 8 条不计。另一残差 `mov 0x378(%eax),%eax; mov %eax,%edx`（我方多 1 条
    // 自中转，ORIG 0x081c774e 为一跳 `mov 0x378(%eax),%edx`）经 4.4.6 探针核实为
    // reload 后端行为（同 RequestDBCharac SIG+0x686 处）⇒ N，未动。
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_accId = user->get_acc_id();
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
}
