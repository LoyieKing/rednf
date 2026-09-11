// ===========================================================================
// CParty::do_after_dungeon_start_checkndelete_item —— T 0x085a0756
// 报告：do_after_dungeon_start_checkndelete_item.md。
// 入口 jmp 直达包构造块（中间 check/delete 块为死代码，见文件头注记）：
// put_header(0,0xe)/put_byte(0)/put_short(1)/MakeItemPacket(INVEN 1, slot=0)/
// finalize(true)/Send，恒返回 0。
// ===========================================================================
int CParty::do_after_dungeon_start_checkndelete_item(const RequiredItem& item,
                                                     CUser* user)
{
    // [b28d-solo-PartyClosureC2 判词·P337] ORIG 0x085a0756。
    // 旧判词 N111「死块不可造」被 P337 推翻：恒 0 **volatile** 守卫的
    // 读取不可省略，块体与 ORIG 7 个调用点强制发射（`if(0)`/goto 才会 DCE）。
    // 死块运行期不可达（与 ORIG 入口 jmp 等价），活动路径语义零变化。
    //   现读数：133 vs 134（gap −1）、p167 19 vs 19（余差仅 PacketGuard/cMyTrace
    //   C1↔C2/D1↔D2 别名对）、帧 0x90 vs ORIG 0x80（N82 槽阶梯残余）。
    // [M2S4 2026-09-11] 我方有效 132 条（+1 条 ret 后填充 nop，§6.1 豁免不计）vs
    //   ORIG 133 ⇒ 净 −1；p167 19/19（余 C1↔C2/D1↔D2 同址别名）。本波两处：
    //   ① `const bool deadMissing = (slot == -1); if (deadMissing)` → `if (slot == -1)`
    //      （S1 去具名）。注：ORIG 0x085a0784 `cmpl $-1; sete %al; test %al,%al; je`
    //      是 4.1 独有形（bool 值留 %al 不落槽）；4.4 实测只有两极：`cmpl;jne`（2 条）
    //      与具名 `sete;mov %al,-N;cmpb;je`（5 条），无 4 条形（探针 /tmp/m2s4_sete.cpp
    //      A–G 七形全测）⇒ 该 2 条形差封 N。
    //   ② trace 改单语句 `cMyTrace(...)(...)`（P329：ORIG ctor 在 getCurCharacNo 之后）。
    //   残余（封 N，4.1 独有的不可达码预取，买不回那 −1）：ORIG 0x085a0765/85a0768
    //   `mov 0xc(%ebp),%eax; mov (%eax),%ebx` + 0x085a0775/779 死实参
    //   `mov %ebx,0x4(%esp); mov %eax,(%esp)`（入口 jmp 前，%eax/%ebx 延续给死块首句
    //   0x085a0781 `mov %eax,-0xc(%ebp)`）；4.4 对具名 `inv` 必落槽，无法表达跨守卫的
    //   寄存器延续 ⇒ 该 4 条为我方不可造段（去掉具名会多一次 getCurCharacInvenR 调用，
    //   p167 会 EXTRA，禁止）。帧 0x90 vs 0x80 由 inv/守卫槽引起。
    // [M2S5E 2026-09-11 负结论·实测回滚] ① 删 inv 具名（改入口
    //   `int slot = (int)reinterpret_cast<size_t>(user->getCurCharacInvenR());`）：
    //   编译通过，读数 129 vs ORIG 133（gap 0→+4，劣化），且槽仍不对齐
    //   （deadItem -0x69 vs ORIG -0x65、packet -0x30 vs -0x28、帧 0x90 vs 0x80）
    //   ⇒ 已回滚。根源：ORIG 入口 4 条（0x085a0765 `mov 0xc(%ebp),%eax; mov (%eax),%ebx`
    //   + 0x085a0775 `mov %ebx,0x4(%esp); mov %eax,(%esp)`）是 4.1 不可达块的寄存器
    //   延续残留；删 inv 后我方无指令可补这 4 条。
    //   ② 探针实证（/tmp/probeRDAR/Game/RDARScriptStringManager.cpp，4.4.6 后端 + 带 EH
    //   作用域，FULLFLAGS 复刻）：`if(0)` / `if(false)` / `goto` / `const int z=0` 四种形
    //   一律 DCE 块体（块内 0 条发射），只有 volatile 守卫保留块体（4 条
    //   `movb/movzbl/test/je`）vs ORIG 1 条 `jmp`（0x085a077c）⇒ 「保留不可达块」是
    //   4.1 -O0 不做 CFG DCE 的行为，4.4 无源形等价物；P337 折衷即上限，维持。
    int slot = 0;                                         // ORIG -0xc（+0x008 movl）
    const CInventory* inv = user->getCurCharacInvenR();   // ORIG +0x1a（返回值仅死块用）

    volatile bool neverEnter = false;                     // P337 恒 0 守卫
    if (neverEnter)
    {
        // ORIG +0x2b：+0x1a 返回值直存 slot 槽再与 -1 比较（按指令转写）
        slot = (int)reinterpret_cast<size_t>(inv);
        // [M2S4] ORIG +0x2e..+0x35 是 `cmpl $-1; sete %al; test %al,%al; je`：
        // 具名 bool 会让 4.4 多 emit `mov %al,-0x9; cmpb $0x0,-0x9`（+2 条）⇒
        // 直接写相等判断。
        if (slot == -1)                                           // ORIG +0x2e
            return 0x11;
        const Inven_Item deadItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, slot);    // ORIG +0x43..+0x6c
        if (deadItem.m_addInfo2 < item.m_itemCount)               // ORIG +0x6f..+0x7a
            return 0x11;
        if (!user->getCurCharacInvenW()->delete_item(             // ORIG +0x86..+0xc2
                (INVEN_TYPE)1, slot, item.m_itemCount, (eItemDelReason)0xe, true))
        {
            // [P329] ORIG +0xdc 序列为「实参求值(characNo→%ebx) → cMyTrace ctor →
            //   operator()」单语句形（ctor 在 getCurCharacNo 之后）；两语句写法让
            //   4.4 把 ctor 提前到实参求值之前（整段物理序反）。
            cMyTrace(__PRETTY_FUNCTION__, 0xdfa, 5)(
                "CParty::isEventDungeon, delete_item failed , User ch=%d , %d %d",
                user->getCurCharacNo(), slot, item.m_itemCount); // ORIG +0xff..+0x11c
            return 0x11;
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}