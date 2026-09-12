// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08216b9a orig/ours=93/92
// [R12a01 收敛确认] 帧 sub 已对齐（我方 0x44 = ORIG 0x44，0x54→0x44 由：
//   ① 删 uid/characNo 具名槽（P0：ORIG 值直落 packet 成员，无独立槽）；
//   ② PCK_AUCTION_MY_BIDDING_INFO_GA 加 pack(1)（该头原先脱离 PacketDesign.h 的
//      pack 区 ⇒ 派生类自然对齐使载荷落 +0x14/+0x18、sizeof 0x1c；pack 后 +0x12/+0x16、
//      sizeof 0x1a = ORIG setSize 值）；
//   ③ 字段写从裸偏移两步（lea+lea+mov）改为具名成员 EBP 直存（P367）。
//   新 .o 同形：0x27e9c `sub $0x44` ↔ ORIG 0x08216b9e；0x27f30 `lea -0x26(%ebp)` ↔
//   ORIG 0x08216c32 `lea -0x36(%ebp)`；0x27f46 `mov %eax,-0x14`（= packet+0x12）↔
//   ORIG 0x08216c48 `mov %eax,-0x24`；0x27f54 `mov %eax,-0x10`（= packet+0x16）↔
//   ORIG 0x08216c56 `mov %eax,-0x20`；0x27f5c `lea -0x26(%ebp),%edx`（SendPacket 实参）
//   ↔ ORIG 0x08216c5e。残余 = 帧外形态（trace 求值序 N80/P227 族、cMyTrace 槽）⇒ 维持 DIFF。
//   前注（保留）：帧差 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
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
    // [R12a01 P367] 具名成员直存：ORIG 0x08216c48/0x08216c56 为 EBP 直存（packet+0x12/+0x16），
    // 无 uid/characNo 具名槽（旧形 `unsigned int uid = …` 多 2 槽 + lea 两步写）。
    PCK_AUCTION_MY_BIDDING_INFO_GA req;
    req.m_uid = user->GetUID();                                          // +0x12
    req.m_characNo = (unsigned int)user->getCurCharacNo();               // +0x16
    GlobalData::s_auction_proxy->SendPacket((nsl::PACKET_HEADER*)&req);

    // ORIG cmpl $0x4,-0xc; setle → i <= 4（含 4，共 5 轮）
    for (int i = 0; i <= 4; ++i)
    {
        if (gmList[i] == (int)user->getCurCharacNo())   // [R4 2026-09-11 · 符号名轴] 原 s_gmList（定义已删）
        {
            // [R15a03 2026-09-12 P227 试验] ORIG 0x08216c88 `call getCurCharacNo;
            //   mov %eax,%ebx` **在** ctor 0x08216cc1 之前求值并以 %ebx 跨 ctor 保活，
            //   其后 0x08216cd2 `mov %ebx,0xc(%esp)`；两语句形（具名 trance + 单独调用）
            //   使 4.4 恒 ctor 后求值 ⇒ 改「构造即调用」全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0xc84e, 0)(
                "Trace Auction Delay, %s(%d)", "REQ AuctionMyBiddingInfo",
                (int)user->getCurCharacNo());
            break;
        }
    }
    return 0;
}