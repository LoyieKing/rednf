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
        char* sig = (char*)guard.operator->()->GetInBuffer<SIG_SAVE_CHATTING_EMOTICON>();
        *(unsigned int*)(sig + 0) = get_acc_id();
        // sig 为线格式缓冲（§4.3 例外）：结构整体赋值复现 ORIG rep movsl。
        *(CChattingEmoticonConfig*)(sig + 4) = cfg;
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
