// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08216b9a orig/ours=94/98 ndiff-blocks=14
//   帧差 sub $44 vs $54 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x44,%esp | mov    &_ZN10GlobalData15s_auction_proxyE,%eax | movl   $&_ZZN31Dispatcher_AuctionMyBiddingInfo12dispatch_sigEP5CUserR9PacketBufE1
int Dispatcher_AuctionMyBiddingInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08216b9a
    // ORIG _ZL6gmList：TU 内 static int[5]（0x093701d8，0x14 字节 .data，初值忠实转写）
    // [R4BigTUDispatchers 2026-09-11 · 符号名轴] 删具名 `s_gmList` ⇒ 用 TU 文件级
    //   `_ZL6gmList`（消 `_ZZ…E7s_gmList` 发明符号；初值逐项相同）。

    // call CAuctionServerProxy::IsRunning(s_auction_proxy); xor $1 → !IsRunning
    if (!GlobalData::s_auction_proxy->IsRunning())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xc0, 0x8c);
        return 0;
    }

    // cmp $0x2,get_state; jle → state<=2 即 invalid；否则 getCurCharacR==0 亦 invalid
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xc836,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    // PCK_AUCTION_MY_BIDDING_INFO_GA 栈对象（packet 槽基 -0x36，sizeof 0x1a：
    //   C1Ev `memset(this,0,0x1a)` = setSize(0x1a)，两侧 C1Ev 逐条全等）。
    // ORIG 0x08216b9a `08216c48 mov %eax,-0x24(%ebp)` / `08216c56 mov %eax,-0x20(%ebp)`
    //   —— 按槽基换算 = packet+0x12（uid，GetUID）/ packet+0x16（characNo，
    //   getCurCharacNo），即 body 两个 int；与服务器权威布局一致
    //   （AuctionPacket.h: int char_idx @18 / int buyer_id @22）。
    // [V331b02 2026-09-12 修复] 原写 +0x0e/+0x12 少 4 字节 ⇒ +0x0e 落进 PACKET_HEADER
    //   内部、尾部 +0x16 从未写（协议字段错位，服务器买入竞价信息查询键为 0）。
    unsigned int uid = user->GetUID();                                   // local_28
    unsigned int characNo = (unsigned int)user->getCurCharacNo();         // local_24
    PCK_AUCTION_MY_BIDDING_INFO_GA req;
    *(unsigned int*)((char*)&req + 0x12) = uid;
    *(unsigned int*)((char*)&req + 0x16) = characNo;
    GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&req);

    // ORIG cmpl $0x4,-0xc; setle → i <= 4（含 4，共 5 轮）
    for (int i = 0; i <= 4; ++i)
    {
        if (gmList[i] == (int)user->getCurCharacNo())   // [R4 2026-09-11 · 符号名轴] 原 s_gmList（定义已删）
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xc84e, 0);
            // ORIG [69]：characNo 经 %ebx 直传（无栈槽）。
            trace("Trace Auction Delay, %s(%d)", "REQ AuctionMyBiddingInfo",
                  (int)user->getCurCharacNo());
            break;
        }
    }
    return 0;
}