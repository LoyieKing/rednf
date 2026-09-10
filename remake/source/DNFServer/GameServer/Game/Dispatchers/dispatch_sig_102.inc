int Dispatcher_RemovePrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08202bb0：无 user/manager/seller 判空（GetStoreSeller 假定有效）。
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        PacketGuard response;
        response.put_header(1, 0x59);
        response.put_byte(0);
        response.put_byte(0x15);
        response.finalize(true);
        user->Send(response);
        return 0;
    }
    private_store::CPrivateStoreMgr* manager =
        private_store::GetInstancePrivateStoreMgr();
    private_store::ISeller* seller = 0;
    manager->GetStoreSeller(&seller, user);
    seller->RemovePrivateStore(false);
    return 0;
