int DisPatcher_PvpHeartBeat::dispatch_sig(CUser* user, PacketBuf& /*packet*/)
{
    // ORIG 0x08204458（报告全文转写）：短路 || 真值经分支直落 %eax（cmp $1;
    // jle→true 块；call R; test; jne→false 块），随后 test %al + fall 进
    // `return 0`（mov $0; jmp end）、je 达 refresh 体（体尾独立 mov $0）。
    if (user->get_state() <= 1 || user->getCurCharacR() == 0)
    {
        return 0;
    }
    disconnect_detecter::GetInstanceDisconnectDetecter()->RefreshRecvTime(user);
    return 0;
