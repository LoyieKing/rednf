// ORIG T 0x081c7602（RequestDBETC.md）：SIG 0x20 → CACHE_ETC；BigStreamPool；
// memset 0x14b60；serverGroup/channelNo ← CEnvironment；tagEtc 清理；
// [b28d-29] P254/N18a：字段->实参两段中转（ORIG mov mem,%eax; mov %eax,%edx）与块序差；语义集合全等  (ORIG 0x081c7602 insn 152->154)
// job 预写 combo 视图 m_job（SIG 内绝对偏移 +0x14ab0）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 41（ORIG 152 / ours 153），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
int DisPatcher_SelectCharac::RequestDBETC(CUser* user, MSG_CHARAC_SELECT& msg)

{
    // [DpcMisc-R2 2026-09-11 残差说明] 除下方 m_job 的 movsbl 修复外，本件另两处
    // 残差为跨版本族，维持原形：① SIG+0x2d30 `mov 0x378(%eax),%eax; mov %eax,%edx`
    // （4.4 多一跳自中转，ORIG 0x081c774e 单跳 `mov 0x378(%eax),%edx`）= 4.4.6 reload
    // 行为（同 RequestDBCharac SIG+0x686 处）；② `mov -0xc(%ebp),%eax; add $0x14aaa,%eax;
    // mov %eax,%ebx`（我方把 `reinterpret_cast<SIG_COMBO_SKILL*>(sig->m_comboSkill)`
    // 的地址跨 GetCharacJob call 缓存到 %ebx，ORIG 0x081c77dd 在 call **之后**重算
    // `mov -0xc(%ebp),%eax; mov %edx,0x14ab0(%eax)`）= N76 族 LHS-address-first。
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
    // [DpcMisc-P6 2026-09-11] 轴 B 符号性（语义轴⑤，ORIG 直证）：ORIG 0x081c77dd
    // `call GetCharacJob; movsbl %al,%edx; mov %edx,0x14ab0(%eax)` —— **符号扩展**
    // （movsbl）后存 int。我方原形直用 `unsigned char GetCharacJob(...)`（CUser.h:3503
    // 声明）⇒ 4.4 产 `movzbl %al,%eax`（零扩展），job ≥ 0x80 时与 ORIG 值分叉
    // （-128 vs 128）。调用点显式收窄为 signed char 即复现 movsbl（CUser.h 的返回
    // 类型由 K4/K7 泳道属主决定，此处不越界）。
    struct LoadEtcComboJobView
    {
        char m_pad[0x14ab0];
        unsigned int m_job;              // ORIG 0x081c77dd `mov %edx,0x14ab0(%eax)`
    };
    // [Q5BigTURest.DisPatcherCore 2026-09-11 P30 disp 折叠轴·落地] ORIG 0x081c77dd 为
    // 单级大 disp：`call GetCharacJob; movsbl %al,%edx; mov -0xc(%ebp),%eax;
    // mov %edx,0x14ab0(%eax)`；原形两级（`reinterpret_cast<SIG_COMBO_SKILL*>
    // (sig->m_comboSkill)->m_job`：先取 +0x14aaa 数组基址再 +6）令 4.4 把基址折进
    // callee-saved（`mov -0xc,%eax; add $0x14aaa,%eax; mov %eax,%ebx` 三条）⇒ 改具名
    // 偏移视图后大 disp 复现，gap −2→**0**（152=152；ours 差异行 8→5）。
    // 残差：`mov -0xc(%ebp),%ebx` 预载（4.4 跨 call callee-saved）与 0x378 自中转（N7/N18a）。
    reinterpret_cast<LoadEtcComboJobView*>(sig)->m_job =
        static_cast<unsigned int>(static_cast<signed char>(
            user->GetCharacJob(static_cast<int>(static_cast<signed char>(msg.m_slot)))));
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
