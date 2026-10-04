// 0x08689a22：表情配置重算（ReCalcChattingEmoticon.md）。读 GameOption 表情配置 →
// CDataManager 重算 → 写回；变化则组 SIG_SAVE_CHATTING_EMOTICON（0x214：+0 accId、
// +4.. 配置 0x210）投递队列 2。SIG 当前 pad 布局按偏移写入（推断）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08689a22 | _ZN5CUser22ReCalcChattingEmoticonEv
void CUser::ReCalcChattingEmoticon()
{
    CGameOption* opt = GetGameOptionRef();
    CChattingEmoticonConfig cfg = opt->getEmoticonInfo();
    G_CDataManager()->ReCalcEmoticonConf(cfg, *this);
    // 2026-09-10（G3CUser-B，P59 分支布局反写）：ORIG 0x08689a87 `test %al,%al;
    //   je 0x08689b85`（==0 直接跳到尾声 -0x163，落点体即整段发报体）⇒ 源形是
    //   **正向** `if (SetEmoticonInfo(cfg)) { ... }`；我方 `if (!x) return;` 多出
    //   `xor $0x1,%eax` 一条且跳转目标反置。
    if (opt->SetEmoticonInfo(cfg))
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xa076), true);
        // b28d-solo-CUser p167：ORIG 0x08689a22 用 `guard.operator*()->operator<<` ×2 +
        //   `guard.operator->()->GetInBuffer`（MISSING operator* ×2/operator-> ×1），
        //   且 cfg 用结构整体赋值（我方 memcpy EXTRA ×1，P83/P132）。
        guard.operator*()->operator<<((int)0x2d7);
        guard.operator*()->operator<<((int)GetUID());
        SIG_SAVE_CHATTING_EMOTICON* sig =
            guard.operator->()->GetInBuffer<SIG_SAVE_CHATTING_EMOTICON>();
        sig->m_mId = get_acc_id();
        // 2026-09-11（Lane2，等宽 align-4 视图 = N78 慢路解药）：ORIG 0x08689b2b 对
        //   +4 的 0x210 字节块走**纯** `lea 0x4(%eax),%ebx; mov $0x84,%eax; mov %ebx,%edi;
        //   mov %edx,%esi; mov %eax,%ecx; rep movsl`（8 条，无对齐剥离前导/尾部）。
        //   我方 `*(CChattingEmoticonConfig*)(sig+4)` 因 CGameOption.h 的
        //   `#pragma pack(push,1)`（CGameOption.h:129/144/166）使该类型 align=1
        //   ⇒ 4.4 发「align 探测 + 剥离 1/2 字节 + rep movsl + 尾 2/1 字节」39 条慢路
        //   （历史 H3「aligned typedef 别名」失败正因别名仍以 align-1 类型为基）。
        //   等宽 align-4 局部视图后 4.4 逐条复现 ORIG 8 条（探针
        //   /tmp/lane2/probe_align3.cpp g2）。视图为纯局部类型：零符号发射、零语义变更。
        sig->m_cfg = cfg;
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
}