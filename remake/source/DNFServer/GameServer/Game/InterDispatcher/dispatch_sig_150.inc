// ORIG 0x084d78f4 T dispatch_sig —— 我的竞拍列表应答：GM 追踪 + 0xc0 包转发余包。
// 报告 docs/class_func_reports/Inter_AuctionResultMyBidding/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=69（ORIG 0x084d78f4）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=69（ORIG 0x84d78f4）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_AuctionResultMyBidding::dispatch_sig(CUser* user, char* data, int len)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;
    // 2026-09-11 L9GrpM（P59/P222 守卫极性 → 包裹形）：ORIG 0x084d78f4 同型
    //   `cmp $0x3; jbe <尾>`（正文 fall-through）；现形 `ja` 镜像。探针 pg.cpp g1 实证。
    if (static_cast<unsigned int>(len) > 3)
    {
        // P183：ORIG 084d7918 +0x024 起直接 memcpy，无 `movl $0x0,-0x20(%ebp)` 预清零 ⇒ 原码无 `= 0` 初值。
        int characNoMsg;
        memcpy(&characNoMsg, data, 4);               // msg+0x00（本地 4 字节拷贝）
        if (characNoMsg != user->getCurCharacNo())
            return 0;

        // GM 延迟追踪（asm 循环界 cmpl $0x4, setle）
        for (int i = 0; i <= 4; ++i)
        {
            if (gmList[i] == user->getCurCharacNo())
            {
                cMyTrace(__PRETTY_FUNCTION__,
                    0x3d31, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultMyBidding",
                      user->getCurCharacNo());
                break;
            }
        }

        // [H2·2026-09-11 laneC P0] 同 AuctionResultItemList（ORIG 084d78f4 形）：return 0 置域外。
        {
            PacketGuard packet;
            packet.put_header(1, 0xc0);
            packet.put_byte(1);
            // [P5·2026-09-11] 同 #M2（ORIG 084d7918 同形 `sub $0x4,%eax`）：
            //   `(int)(len - 4u)` ⇒ unsigned 减法路径（探针 lea2.cpp v8）。
            packet.put_binary(data + 4, (int)(len - 4u)); // 余包原样转发
            packet.finalize(true);
            user->Send(packet);
        }
    }
    return 0;
}