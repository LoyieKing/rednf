// ORIG T 0x081c6ca0（RequestDBCharac.md）：SIG 7 → CACHE_CHARAC；
// [b28d-29] N93附 + N18a：尾块布局差 + `mov 0xe(%eax),%eax; mov %eax,%edx` 单段直落不可造  (ORIG 0x081c6ca0 insn 160->159)
// Reset 后填 acc/charac/school/tag 区；双职业热键装载；队列 5(有效上次登录)/2(DB_Q)。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 23（ORIG 160 / ours 159），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址；沿用上方 b28d-29 归因，封顶。
int DisPatcher_SelectCharac::RequestDBCharac(CUser* user, MSG_CHARAC_SELECT const& msg)

{
    // [b28c-L9] fresh 复验 ndiff 24→19；p167 调用点差集 32=32（0 missing/0 extra）。
    // 全部残差 = CStreamGuard EH 尾块布局跨版本不可翻：ORIG 0x081c6dxx 正常路径
    // `mov $1,%ebx; jmp Ltail` 跳到**函数最末**的共享 D1+epilogue，unwind 块居中；
    // 4.4 把 D1+epilogue 就地内联在正常路径、unwind 块压到最末（少 1 jmp + 8:8 块序
    // rotate，difflib 放大为 18）。语义/gap/槽位集全等 ⇒ 按 §6.2 判语义完成、byte N 封顶。
    // [b23S3C] ORIG 帧 0x20 仅 guard -0x14/sig -0xc 两槽：Acquire 右值直落 ctor
    // 实参（无 stream 槽）；guard 写流 4 处均 `(*guard)->operator<<`（deEv call
    // 080c8c46 + lsEi call 080c8c56，P48 每次重取 operator*）；GetInBuffer 走
    // operator-> 链（ptEv 080c8c4e，P103）；双 job 无具名槽（ebx 跨 call 缓存，
    // +0x192）⇒ 嵌套实参直调。
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG
    cMyTrace(__PRETTY_FUNCTION__, 0x173e, 0)(
        "[CHARLIST_DIAG] select_request acc=%u slot=%d cached_count=%d",
        user->get_acc_id(), static_cast<int>(static_cast<signed char>(msg.m_slot)),
        user->get_charac_count());
#endif
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x173d),
        true);
    (*guard)->operator<<(7);
    (*guard)->operator<<(static_cast<int>(user->GetUID()));
    user->DisableCharacInfo(CACHE_CHARAC);
    if (user->IsValidLastLoginCharac())
    {
        (*guard)->operator<<(user->GetLastLoginChannelNo());
        (*guard)->operator<<(user->GetLastLoginCharacNo());
    }
    SIG_LOAD_CHARAC* sig =
        guard.operator->()->GetInBuffer<SIG_LOAD_CHARAC>();
    sig->m_characData.Reset();                    // CHARAC_DATA::Reset W 0x822ec9e
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_characData.m_schoolNo = user->GetSchoolNo();   // ORIG 写 SIG+0x682
    sig->m_tagCharacData.clear();                 // TAG_CHARAC_DATA::clear W 0x822ed84
    sig->m_tagCharacNo = msg.m_0e;
    sig->m_strikerSkillIndex = msg.m_12;
    user->LoadCharacTypeHotKey(
        user->get_character_job(
            static_cast<unsigned int>(user->GetLastLoginCharacNo())),
        user->get_character_job(sig->m_characNo));
    // [b28e-L5] C05-① 复查实测**不命中**（已回退）：套用 Ticket/Skill 的「两分支各自
    // `return 1;`」形后本件 our_insns 159→161（ORIG 160）且 TU 四数不变（仍 DIFF）
    // ⇒ 本件残差主体是 N18a（`mov 0xe(%eax),%eax; mov %eax,%edx` 单段直落）而非尾块布局，
    // 保持原形（单尾 return 1）不再试形。
    if (user->IsValidLastLoginCharac())
    {
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG
        cMyTrace(__PRETTY_FUNCTION__, 0x1750, 0)(
            "[CHARLIST_DIAG] select_load_queue acc=%u source=last_login charac=%u",
            user->get_acc_id(), sig->m_characNo);
#endif
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
    }
    else
    {
#if !remake_verify && defined(DNF_DIAG) && DNF_DIAG
        cMyTrace(__PRETTY_FUNCTION__, 0x1751, 0)(
            "[CHARLIST_DIAG] select_load_queue acc=%u source=db charac=%u",
            user->get_acc_id(), sig->m_characNo);
#endif
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
    return 1;
}

// [DpcMisc-R1 2026-09-11 负结论] 本件 our 159 / ORIG 160（gap −1），残差两项均已实测为
// 后端 N 类，源码形无自由度，保持原形：
// ① SIG+0x686 赋值：ORIG 0x081c6e01-0x081c6e07 `mov 0x10(%ebp),%eax;
//    mov 0xe(%eax),%edx; mov -0xc(%ebp),%eax; mov %edx,0x686(%eax)`（一跳 mem→edx）；
//    我方 .o 3c9a 段同位置为 `mov 0xe(%eax),%eax; mov %eax,%edx; ...`（多一跳自中转）。
//    4.4.6 探针三形实测（`d->g = a->f;` / `= (unsigned)a->f;` / 具名临时 v）读数**全部相同**
//    ⇒ 恒产两跳，属 4.4.6 reload 行为（ORIG 为 4.1 一跳），非源码可择。
// ② 尾部块布局：ORIG 0x081c6e8c/0x081c6eb4 两分支 `mov $1,%ebx; jmp 0x081c6ed1` 共享
//    函数末 tail（dtor+epilogue），unwind 块居中；我方 4.4.6 把 dtor+epilogue 就地内联在
//    正常路径、unwind 压到最末（少 1 jmp + 8:8 块序 rotate）。b28e-L5 已试「两分支各自
//    return 1」=159→161 更差（+2 条 dtor 调用），本轮未再试。
// p167 实测：ORIG calls=32 / OUR calls=32，差集仅 CStreamGuard C1/C2·D1/D2 同址别名
// （ORIG C2EP6Streamb×1+D2Ev×2 ↔ OUR C1EP6Streamb×1+D1Ev×2）⇒ 判据第 8 条不计。
