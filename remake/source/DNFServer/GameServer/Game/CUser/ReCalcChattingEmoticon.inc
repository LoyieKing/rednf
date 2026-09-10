void CUser::ReCalcChattingEmoticon()
{
    CGameOption* opt = GetGameOptionRef();
    CChattingEmoticonConfig cfg = opt->getEmoticonInfo();
    G_CDataManager()->ReCalcEmoticonConf(cfg, *this);
    if (!opt->SetEmoticonInfo(cfg))
        return;

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
