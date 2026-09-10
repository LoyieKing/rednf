int Dispatcher_ExitPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    private_store::IBuyer* buyer = 0;
    // [b28c-32] ORIG 0x082026df call GetInstancePrivateStoreMgr 结果 %eax 直传
    // (%esp)（无具名 mgr 缓存槽），且 0x08202702 直取 buyer 无判空（P0/P171）。
    private_store::GetInstancePrivateStoreMgr()->GetStoreBuyer(
        &buyer, user, static_cast<CUser*>(0));
    buyer->ExitPrivateStore();      // ORIG IBuyer vtable 槽 +0x0c
    return 0;
