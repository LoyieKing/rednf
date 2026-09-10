int Inter_LoadServerMessage::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // ORIG +0x06：包基址先落 -0x10 具名局部（在 G_CGameManager 调用之前）。
    SIG_EVENT_SERVER_MESSAGE* pPkt = reinterpret_cast<SIG_EVENT_SERVER_MESSAGE*>(data);
    CSharedServerMessageManager* mgr = G_CGameManager()->GetSharedServerMessageManager();
    // ORIG +0x15：`cmpl $0x0,-0xc(%ebp)`+`jne body`+早退在 fall-through ⇒ == 0 写法。
    if (mgr == 0)
        return 0;
    mgr->SetServerMessage(pPkt);
    return 0;
