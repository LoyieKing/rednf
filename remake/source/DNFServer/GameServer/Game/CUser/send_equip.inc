// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865dd14 | _ZN5CUser10send_equipEi
void CUser::send_equip(int slot)
{
    // ORIG 0x0865dd14：state(+0x8cfc4) ∈ {5,8,10,0xc} 且 slot==0x16 → 发给自己；
    // 否则（slot<0xc 或 0x16）→ 广播 make_basic_info(type 0) 给全房间。
    if (slot >= 0xc && slot != 0x16)
        return;
    // 2026-09-10（G3CUser-B，P0 槽-局部律）：ORIG 0x0865dd2c/0x0865dd3e/0x0865dd50/
    //   0x0865dd62 四处**各自** `mov 0x8(%ebp),%eax; mov 0x8cfc4(%eax),%eax;
    //   cmp $..,%eax; je 0x0865de35` —— m_chState 每次经 this 重读（无 state 槽、无
    //   寄存器缓存），4 个条件各自直跳同一落点（inState **无槽**）⇒ 我方
    //   `int state` / `bool inState` 两个具名量 ORIG 均无，删并内联为 4 项 `||`。
    // b28d-CuE 2026-09-11（P47/P194 then-块物理序直证）：ORIG 0x0865dd2c/3e/50/62 四处
    //   各自 `mov 0x8(%ebp),%eax; mov 0x8cfc4(%eax),%eax; cmp $..,%eax; je 0x0865de35`
    //   —— 四个 **不等链失败** 才跳入后置的 then 块（0x0865de35 起才是 slot==0x16 检查
    //   + type-1 包体），fall-through 落 **0x0865dd74 起的广播体（else）**；我方旧
    //   `== || == || == || ==` 形式被 4.4 折成 3×je + 1×jne 且把 then 块放 fall-through
    //   （0x0865dc.. jne +0x116，then 体在 0x0865dc2a 之前）⇒ 多 2 条且块序相反。
    //   改写为语义等价的 `!= && != && != && !=` 使每个失败条件各自 `je` 跳后置块。
    if (m_chState != 0xc && m_chState != 5 && m_chState != 8 && m_chState != 10)
    {
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        G_GameWorld()->send_all(packet, this);
    }
    else
    {
        // [L9-GrpL 2026-09-11 尾块跳转合并] ORIG 0x0865def2 全函数**只有一条** `jmp epilogue`
        //   （`jmp 0865def5 <+0x1e1>` 后跟 `nop`，0x0865def5 即 `add $0x30,%esp`）；
        //   我方 `if (slot != 0x16) return;` 额外产一条 `jmp +0x1e4`（raw 128 vs ORIG 125，
        //   尾部 `jmp;nop;jmp;nop` 双岛）⇒ 去掉裸 return，改正向 if 包裹：
        //   4.4 对 `if (cond) {…}` 恒 `jne <跳过体>`，跳过目标即 else 体出口（不再生成
        //   return 的跳转指令）。
        if (slot == 0x16)
        {
            PacketGuard packet;
            ((InterfacePacketBuf*)&packet)->put_header(0, 2);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->put_short(1);
            make_basic_info((char*)&packet, (char)0);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
    }
}