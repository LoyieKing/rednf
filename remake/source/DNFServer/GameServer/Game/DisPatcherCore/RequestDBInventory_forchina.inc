// ORIG T 0x081c7c1c（RequestDBInventory_forchina.md）：SIG 8 → CACHE_INVENTORY；
// BigStreamPool；memset 0x1544c 后 acc/charac/tagInven 清理/+0x8←MSG.m_0e。
// [b28d-29] setcc-fold：ORIG 尾 bool 物化岛（mov $1,%esi/mov $0,%ebx + test），4.4 恒折单条 mov/直分支  (ORIG 0x081c7c1c insn 137->134)
// 尾部两分支均返回 1（ORIG esi 未初始化路径经 test ebx 归一为恒真）。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 8（ORIG 137 / ours 133），残差=N82 槽/块序；沿用上方 b28d-29 归因，封顶。
int DisPatcher_SelectCharac::RequestDBInventory_forchina(CUser* user,

                                                          MSG_CHARAC_SELECT& msg)
{
    // [b28c-L9] fresh 复验 ndiff 14→6；p167 27=27 全清。残差 = EH 寄存器角色：
    // ORIG unwind 路用 %esi 承载返回值、%ebx 承载异常指针（`mov $1,%esi` ×2 +
    // `test %ebx,%ebx; je`），4.4 单寄存器 %ebx 复用 ⇒ N7/N18a 跨版本分配，语义完成。
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x18af), true);
    **guard << 8;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_INVENTORY);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_INVENTORY* sig = guard->GetInBuffer<SIG_LOAD_INVENTORY>();
    memset(sig, 0, sizeof(SIG_LOAD_INVENTORY));
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_tagInven.clear();                      // TAG_CHARAC_INVEN_DATA::clear W 0x822edd0
    sig->m_field8 = msg.m_0e;
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
        return 1;
    }

    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
}

// [DpcMisc-I1 2026-09-11 负结论] 本件 our 133 / ORIG 138（gap −5），唯一残差段 = 函数尾：
// ORIG 0x081c7da8/0x081c7dad 与 0x081c7dd0 为 4.1 的「返回值寄存器 %esi + 已设置标志 %ebx」
// 双寄存器形（分支1 `mov $0x1,%esi; mov $0x0,%ebx`，分支2 `mov $0x1,%ebx`，
// 0x081c7df2 tail：`test %ebx,%ebx; je +1ea; mov $0x1,%esi; mov %esi,%eax`）；
// 我方 .o（197xx）为 4.4.6 单寄存器形 `mov $0x1,%ebx` 两分支 + fall-through dtor +
// `mov %ebx,%eax`（共少 5 条）。
// 4.4.6 探针三形实测（if/else+`return 1;`、两分支各自 `return 1;`、混合）**均只产单寄存器
// 形**，无一复现 ORIG 的 esi+ebx 延迟设置 ⇒ 4.1 时代码生成器特征（N），源码形不可择。
// 头部/中段逐条已对齐（memset 常量、m_slot/m_0e 读取、TAG clear、双 put 分支均同形）。
