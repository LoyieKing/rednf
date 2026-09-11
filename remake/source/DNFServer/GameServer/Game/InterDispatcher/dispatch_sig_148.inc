// ORIG 0x084d75bc T dispatch_sig —— 拍卖物品列表应答：GM 延迟追踪 + 0xbd 包转发余包。
// 报告 docs/class_func_reports/Inter_AuctionResultItemList/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=69（ORIG 0x084d75bc）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-1。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=69（ORIG 0x84d75bc）：N22 cmp 操作数方向+P30/N4 add-vs-lea。
int Inter_AuctionResultItemList::dispatch_sig(CUser* user, char* data, int len)
{
    if (user == 0)                                   // asm cmpl $0x0,0xc(%ebp)
        return 0;
    // 2026-09-11 L9GrpM（P59/P222 守卫极性 → 包裹形）：ORIG 0x084d75bc `cmp $0x3,%eax;
    //   jbe 084d779x`（跳**尾部** return 0、正文 fall-through）；我方早退形令 4.4 产
    //   `ja <正文>` + 失败块 fall-through（块序镜像 + 全函数错位 2 行）。
    //   探针 /tmp/L9/M/pg.cpp：g0（现形 `<=3 return 0`）→ `ja`；g1（本形 `>3 {…}`）→
    //   `jbe <尾>` = ORIG 同向。正文逐字未改，仅整体缩进 +4。
    if (static_cast<unsigned int>(len) > 3)
    {
        // P183：ORIG 084d75e0 +0x024..084d75f5 直接 `movl $0x4,0x8(%esp)` + memcpy，
        //   不见 `movl $0x0,-0x20(%ebp)` 预清零 ⇒ 原码无 `= 0` 初值（我方多 1 条存储）。
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
                    0x3cab, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultItemList",
                      user->getCurCharacNo());
                break;
            }
        }

        // [H2·2026-09-11 laneC P0] ORIG 084d774b `mov $0x0,%eax` 在 ~PacketGuard **之后**
        //   （域外直接 eax）；旧写把 return 0 放在 packet 作用域内 ⇒ 4.4 用 %ebx 跨清理暂存
        //   （mov $0x0,%ebx + mov %ebx,%eax，多 4 条，n 116 vs ORIG 112）⇒ 包独立作用域。
        {
            PacketGuard packet;
            packet.put_header(1, 0xbd);
            packet.put_byte(1);
            // [P5·2026-09-11] 类型轴：ORIG 084d76e5 `sub $0x4,%eax`（就地），我方 4.4 对
            //   `len - 4`(int) 产 `lea -0x4(%eax),%edx`+寄存器交换（唯一残差）。改
            //   `(int)(len - 4u)` 走 unsigned 减法路径 ⇒ 探针 /tmp/P5/lea2.cpp v8 得
            //   `mov 0x14(%ebp),%eax; sub $0x4,%eax; mov 0x10(%ebp),%edx; add $0x4,%edx`
            //   与 ORIG 逐字节同形。守卫 `(unsigned)len > 3` 已保证 len≥4 ⇒ 等义。
            packet.put_binary(data + 4, (int)(len - 4u)); // 余包原样转发
            packet.finalize(true);
            user->Send(packet);
        }
    }
    return 0;
}