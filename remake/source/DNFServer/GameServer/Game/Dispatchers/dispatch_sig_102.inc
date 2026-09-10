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
    private_store::ISeller* seller = 0;
    // ORIG 0x08202bb0 段：`movl $0x0,-0xc(%ebp)`（seller 预清）后 `lea -0xc(%ebp),%edx`
    //   —— 无 manager 具名指针槽（我方具名使帧 0x20→0x30）。
    private_store::GetInstancePrivateStoreMgr()->GetStoreSeller(&seller, user);
    seller->RemovePrivateStore(false);
    return 0;
