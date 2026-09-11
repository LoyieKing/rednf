// ORIG 0x084d7ac8 T dispatch_sig —— 拍卖服务开关：置 proxy 运行态并全员广播。
// 报告 docs/class_func_reports/Inter_AuctionNotifyAuctionService/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=90（ORIG 0x084d7ac8）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-5。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=95（ORIG 0x84d7ac8）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_AuctionNotifyAuctionService::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
        reinterpret_cast<SIG_AUCTION_NOTIFY_AUCTION_SERVICE*>(data);

    PacketGuard packet;
    packet.put_header(0, 0xb7);
    packet.put_byte(sig->m_result);
    if (sig->m_result == 1)
    {   // 点数拍卖服
        // [P5·2026-09-11] ORIG 084d7b1d `movzbl 0x4(%eax),%eax; movzbl %al,%edx;
        //   mov %edx,0x4(%esp)`（**无 test/setne 规范化**）；`(char)m_flag` ⇒
        //   unsigned char→char→bool 双转换令 4.4 多 `test %al,%al; setne %al`（2 条）。
        //   按 bool 直读（值域 0/1，ORIG 与本形都只传低字节 ⇒ 行为同）⇒ 探针
        //   /tmp/P5/bconv.cpp f2 实证逐条 = ORIG 形（无规范化）。
        GlobalData::s_cera_auction_proxy->SetRunning(
            sig->m_flag);
    }
    else
    {   // 金币拍卖服
        // 同上（ORIG 084d7b3a 同形）。
        GlobalData::s_auction_proxy->SetRunning(
            sig->m_flag);
    }

    // ORIG 084d7b59 +0x091 `movzbl 0x4(%eax),%eax; test %al,%al; je +0xaa`（跳到 $0 块）
    //   ⇒ fall-through 分支落 `movl $0x1`，原码为 `!= 0` 在前。
    if (sig->m_flag != 0)
        packet.put_byte(1);
    else
        packet.put_byte(0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);

    // P0 去具名局部：ORIG 084d7bb7..084d7bd8 两个字符串直接落 %esi/%ebx（无栈槽往返，我方原多 8 字节）
    //   ⇒ 原码是调用点内联三目；ORIG 先算 state（最后实参）再算 serverName，正是调用实参自右向左的求值序。
    cMyTrace(__PRETTY_FUNCTION__,
        0x3dea, 0)("%s server %s! SEND SIG_AUCTION_NOTIFY_AUCTION_SERVICE TO ALL USERS",
          sig->m_result == 0 ? "Auction" : "Point",
          sig->m_flag != 0 ? "Opened" : "Closed");
    return 0;
}