// ---- requestLoadFromDBAndES（advancealtar__DBAndExchangeServerProc 报告全文）----
// ORIG T 0x08133784：reset 管理器 → Acquire stream(opcode 0x19) → uid →
// DisableCharacInfo(10) →（ValidLastLogin 时补 channelNo/characNo）→
// GetInBuffer<_CharacAdvanceAltarDbData> 清零 0x51c 后写 m_characNo →
// ValidLastLogin ? put 队列5 : put DB_Q(2) → 返回 1。
// [S6 本波] 123/124（我方 122→124）：原 N93附 尾块差异已消 —— ORIG 两臂各一份
//   `mov $0x1,%ebx` ⇒ 源形为两臂各自 `return true;`；改写后该差异归零。
// [b28d-solo-advancealtar_DB] 余 N76+N104 封顶（orig 123 / ours 124，clean resid=25）：
//   (1) `data->m_characNo = get_charac_no(...)`：ORIG 0x081338d6 LHS-first 直存，
//       4.4 对**局部指针**字段 store 恒 RHS-first+edx 中转（探针 f2 形参指针=LHS-first，
//       局部指针形恒值先）⇒ N76 族；
//   (2) 尾块 `mov $0x1,%ebx`：ORIG 两臂各一份（4.1 把 return true 内联到两臂），
//       4.4 只在共享尾块一份 ⇒ N93附/跨版本 CFG；
//   (3) EH landing 块相对序（ORIG 居中、4.4 压尾）⇒ N104。
//   P167 CLEAR（26/26）、语义等价。
bool DBAndExchangeServerProc::requestLoadFromDBAndES(CUser& user, MSG_CHARAC_SELECT& msg)
{
    user.m_advanceAltar.reset();  // ORIG CharacAdvanceAltarManager::reset @0x08131660

    // [b28d-solo-advancealtar_DB] P89/P259：ORIG 无 stream 具名槽（0x0813379e
    //   `mov &s_stream_pool,%eax` → Acquire 结果经 eax 直压 ctor 的 0x4(%esp)，
    //   guard 槽 -0x14）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localglobal/global_AdvanceAltar.cpp", 0xc3a), true);
    guard.operator*()->operator<<(0x19);
    guard.operator*()->operator<<(static_cast<int>(user.GetUID()));
    user.DisableCharacInfo(CACHE_ADVANCE_ALTAR);  // ORIG push 0xa（枚举名 Prod_DispatcherHelpers 落地）

    if (user.IsValidLastLoginCharac())
    {
        guard.operator*()->operator<<(user.GetLastLoginChannelNo());
        guard.operator*()->operator<<(user.GetLastLoginCharacNo());
    }

    _CharacAdvanceAltarDbData* data =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memset(data, 0, 0x51c);
    // ORIG 0x081338d6 `call get_charac_no; mov -0xc(%ebp),%edx; mov %eax,(%edx)`
    //   = LHS-first 直存（P211 同型）⇒ 去掉旧 static_cast<unsigned int>（异型化反产
    //   eax→edx 中转）。
    data->m_characNo = user.get_charac_no(static_cast<int>(msg.m_slot));

    // ORIG 0x081338ee `test %al,%al; je <put(2)>` ⇒ 正极性：IsValid 真走 fall-through
    //   put(队列5)，假走 DB_Q(2)。
    // [L2Free4to15.S6 本波] ORIG 两臂各一份 `mov $0x1,%ebx`（0x0813392x/0x0813394x）
    //   ⇒ 源形为两臂各自 `return true;`（非共享尾块单 return）。
    if (user.IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(5), guard);  // 第 6 槽（ORIG 无枚举名）
        return true;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return true;
    }
}