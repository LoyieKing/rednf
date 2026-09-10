int DisPatcher_SetTradeState::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() != 3)
    {
        return 0;
    }
    // ORIG 0x81f6258：`lea -0x1a(%ebp),%eax; add $0xd,%eax` = 0xe 字节栈缓冲
    // （-0x1a..-0xd），目标字节 = buf[0xd]（-0xd，恰在 trade 槽 -0xc 之下）。
    char buf[0xe];
    if (!packet.get_byte(buf[0xd]))
    {
        return LineFunc(0x82e6, __PRETTY_FUNCTION__, 0, 0);
    }
    CTradeSpace* trade = G_CGameManager()->GetTradeSpace(user->GetTradeSpace());
    if (trade == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a, 1);
        return 0;
    }
    // ORIG `test %al,%al; je`：is_empty bool 返回直判（技巧19，勿加 !=0）
    if (trade->is_empty())
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp",
                              __PRETTY_FUNCTION__, 0x82f1,
                              "Trade Space[%d] is Empty User[%d]",
                              trade->get_index(),
                              user->get_unique_id() & 0xffff);
        G_CGameManager()->PutTradeSpace(trade);
        return 0;
    }
    // ORIG `xor $0x1,%eax; test %al,%al; je`：!boolcall 形（技巧1，ORIG 0x852994a
    // check_trade_possibility 体 xor$1 = bool 返回）
    if (!trade->check_trade_possibility())
    {
        trade->cancel_trade();
        G_CGameManager()->PutTradeSpace(trade);
        return 0;
    }
    trade->change_trade_state(user, (int)(char)buf[0xd]);
    return 0;
