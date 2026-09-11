// ORIG T 0x85cac16（报告 LoadPrivateStoreResponse.md 逐指令）。
//   sig->m_dollCreated==1（DB 恢复失败回执）：GetStoreSeller → ResetItems →
//     CopyItems(sig 条目,0xe) → RecallPrivateStoreItemToMail → SavePrivateStore →
//     Clear → FreeStoreSeller；
//   否则若 IsPermissionPrivateStore：MakeNotipacketPrivateStoreInfo(sig) → Send；
//     包内条数非 0 时再经 ISeller 虚槽 +0x14（RecreatePrivateStore）通知卖家。
void CPrivateStoreMgr::LoadPrivateStoreResponse(CUser* user, SIG_PRIVATE_STORE* sig)
{
    int made = 0;
    if (sig->m_dollCreated == 1)
    {
        ISeller* seller;
        GetStoreSeller(&seller, user);
        CPrivateStore* store = static_cast<CPrivateStore*>(seller);
        store->ResetItems();
        store->CopyItems(reinterpret_cast<PrivateStoreItem*>(sig->m_items), 0xe);
        store->RecallPrivateStoreItemToMail();
        store->SavePrivateStore();
        store->Clear();
        FreeStoreSeller(user);
    }
    else if (user->IsPermissionPrivateStore())
    {
        CPacketBufPrivate packet;
        made = packet.MakeNotipacketPrivateStoreInfo(sig);
        user->Send(packet);
        if (made != 0)
        {
            ISeller* seller;
            GetStoreSeller(&seller, user);
            seller->RecreatePrivateStore(sig);
        }
    }
}