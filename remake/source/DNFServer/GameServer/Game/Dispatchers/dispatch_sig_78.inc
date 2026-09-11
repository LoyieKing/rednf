// ORIG 0x08224e08（报告 Dispatcher_PvPBuddyConnList/dispatch_sig.md）：
// IsIntegratedPvPBaseChannel==1 → SendPvPBuddyList(accId, characNo)；恒返 0。
int Dispatcher_PvPBuddyConnList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)packet;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() == 0)
        return 0;
    GlobalData::s_community_proxy->SendPvPBuddyList(
        user->get_acc_id(),
        static_cast<unsigned int>(user->getCurCharacNo()));
    return 0;
}