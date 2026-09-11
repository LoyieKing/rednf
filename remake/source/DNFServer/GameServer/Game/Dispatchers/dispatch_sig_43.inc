// dispatch_sig T 0x0821e4ac：恒返 0；read/process W 0x08233e52/0x08233e5c：恒返 0。
int Dispatcher_GiveGiftToNPC::dispatch_sig(CUser* user, PacketBuf& packet)
{
    (void)user;
    (void)packet;
    return 0;
}