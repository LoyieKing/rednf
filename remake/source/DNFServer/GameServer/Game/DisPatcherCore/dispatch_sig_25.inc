int DisPatcher_ResPvpRank::dispatch_sig(CUser* user, PacketBuf& packet)

{
    // ORIG 0x081f0282 T（报告 DisPatcher_ResPvpRank/dispatch_sig.md 全文转写）。
    if (user->get_state() != 7)
    {
        return 0;
    }
    MSG_RES_PVP_RANK msg;
    // ORIG 0x081f0282：bool 返回值具名槽 `mov %al,-0x9(%ebp); movzbl -0x9(%ebp),%eax`
    // 后 xor$1 取反（技巧1+8：槽复用于两处调用）。声明序 msg→ok ⇒ ok 更近 ebp。
    bool ok;
    ok = ExtractPacket(msg, packet);
    if (!ok)
    {
        return LineFunc(0x7736, __PRETTY_FUNCTION__, 0, 0);
    }
    ok = SavePvpRank(user, msg);
    if (!ok)
    {
        return LineFunc(0x7739, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
