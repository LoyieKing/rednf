int Dispatcher_EnterPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820a...：`state==3 && CurCharacR==0` 单表达式直判（jne 短路物化），
    //   无 gate 具名 bool / 无 ok / 无 mgr 具名指针（三者均使 4.4 落槽、帧 0x24→0x28）。
    if (user->get_state() == 3 && user->getCurCharacR() == 0)
    {
        // [G1-Dispatchers] 两处 LineFunc 早退均走 `jmp <epilogue>` 复用 %eax（ORIG 无独立
        // `mov $0x0,%eax`；我方旧形多 2 条）⇒ `return LineFunc(...);` 共享出口。
        return LineFunc(0x98d4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    short sellerIndex = 0;
    private_store::IBuyer* buyer = 0;
    if (!packet.get_short(sellerIndex))
    {
        return LineFunc(0x98d8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    private_store::GetInstancePrivateStoreMgr()->GetStoreBuyer(
        &buyer, user, (unsigned short)sellerIndex);
    buyer->EnterPrivateStore();
    return 0;
