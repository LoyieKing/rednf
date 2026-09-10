void CInventory::SendAvatarEvent(int a, int b, int c, int d, int e) const
{
    // b28d-solo-CInventory：249/262，比较链块序 + 槽残差 ⇒ N82 封顶。
    // b28d-37 [N82 封顶，ORIG 0x850?]：bool 物化/槽残差（249/262）。
    if (m_pParent == 0)
    {
        // [G8InvenPvP 试形 C] P227/P221：ORIG trace 槽 -0x28 浅于 guard -0x30，
        // 我方相反（trace -0x30 / guard -0x20）⇒ 试「构造即调用」全表达式临时链
        // 翻转互斥块对象槽方向。
        cMyTrace(__PRETTY_FUNCTION__, 0x212a, 5)("CInventory::SendAvatarEven ERROR, m_pParent NULL ");
        return;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("inventory.cpp", 0x212f), true);
    int characNo = m_pParent->get_charac_no(-1);
    **guard << a;
    **guard << (int)m_pParent->GetUID();
    // [G8InvenPvP 2026-09-10 · P0] 删 `unsigned char b0` 具名局部：ORIG
    // 0x08508dc3/0x08508dc9 对 b/c 是**形参直读** `mov 0x10(%ebp),%eax; mov %al,
    // -0x3a(%ebp)`（无 b0 槽 store/load）；我们多一条 `mov %al,-0x15(%ebp)` 且
    // 决策树装载序被其推后一位。语义等价（低 8 位），按 P0 内联 cast 还原。
    // P46/P286：ORIG 0x08508d09 为 `mov 0xc(%ebp),%eax` 后 cmp/je/jg 二分决策树
    // （稀疏 switch 固有形，if 链恒生内存直比线性链）⇒ 改 switch；叶块物理序 = case
    // 书写序（P324）：0x31 / 0x32 / 0x33 / 0x34 / 0x36 / 0x2c3 / 0x31a，default 直落 put。
    switch (a)
    {
    case 0x31:
    {
        // [G8InvenPvP 2026-09-10 · P161/P311 组件视图] ORIG 0x08508d5a..0x08508d81
        // 五条写全为**直接 disp store**（`mov %eax,-0x759(%ebp)` … `mov %al,
        // -0x74b(%ebp)`，无 lea 基址），而 `char buf[N]` + `*(int*)(buf+k)=` 形
        // 在 4.4 恒产 `lea -0x75d(%ebp),%eax; mov %edx,(%eax)` 间接写（每字段
        // +1 条 lea + 装载序反）⇒ 按 ORIG 布局改 pack(1) 具名视图逐字段访问。
        // ORIG 布局：+0x00 characNo(int)、+0x04 b(int)、+0x08 (char)c、
        // +0x0a d(int, 非对齐)、+0x0e (char)e；+0x09 空洞不写（ORIG 原样）。
        struct AvatarEventBuf31
        {
            int m_characNo;      // +0x00
            int m_b;             // +0x04
            char m_c;            // +0x08
            char m_pad09;        // +0x09（ORIG 不写）
            int m_d;             // +0x0a（非对齐）
            char m_e;            // +0x0e
            char m_tail[0x53 - 0x0f];
        } __attribute__((packed));
        AvatarEventBuf31 buf31;
        buf31.m_characNo = characNo;
        buf31.m_b = b;
        buf31.m_c = (char)c;
        buf31.m_d = d;
        buf31.m_e = (char)e;
        // [G8InvenPvP 2026-09-10 · 试形 B 失败] ORIG 0x08508d87 先调
        // `CStreamGuard::operator->` 再 lea buf 实参（0x08508d9a）；显式写
        // `guard.operator->()->put_binary(...)`（P103 形）产物与隐式 `guard->`
        // **逐条相同**（仍 `lea -0x759(%ebp),%ebx` 提前跨调用保活）⇒ 求值序
        // 属 4.4 前端固有，记 N（P75/P23 族）。
        guard->put_binary((char*)&buf31, 0x53);
        break;
    }
    case 0x32:
    {
        // [G8InvenPvP 2026-09-10 · P161/P311 组件视图] ORIG 0x08508db1..0x08508dcc
        // 全为直接 disp store（`mov %eax,-0x46(%ebp)` … `mov %al,-0x39(%ebp)`）。
        // ORIG 布局：+0x00 characNo、+0x04 d、+0x08 e、+0x0c (char)b、
        // +0x0d (char)c、+0x0e / +0x12 两个 int（三目结果，非对齐）。
        // 语义（0x08508dfd 段）：+0x0e 槽判定用 (byte)c、写 e；
        // +0x12 槽判定用 (byte)b、写 d。结构未初始化字节按 ORIG 原样发送。
        struct AvatarEventBuf32
        {
            int m_characNo;      // +0x00
            int m_d;             // +0x04
            int m_e;             // +0x08
            char m_b;            // +0x0c
            char m_c;            // +0x0d
            int m_field0e;       // +0x0e（非对齐）
            int m_field12;       // +0x12（非对齐）
        } __attribute__((packed));
        AvatarEventBuf32 buf32;
        buf32.m_characNo = characNo;
        buf32.m_d = d;
        buf32.m_e = e;
        buf32.m_b = (char)b;
        buf32.m_c = (char)c;
        // [G8InvenPvP 2026-09-10 · P92/P0 + P324 书写序] ORIG 0x08508dcf 起先做
        // **b 分支**（读 buf+0x0c `movzbl -0x3a(%ebp)`, 判定源是已写字段而非形参），
        // 结果落 +0x12（-0x34）；0x08508dfd 起再做 **c 分支**（读 buf+0x0d），
        // 结果落 +0x0e（-0x38）⇒ 书写序 b→c，与叶块物理序一致。
        // [G8InvenPvP 试形 A] ORIG 0x08508de5 段为 `call; test %eax,%eax; setne %al;
        // test %al,%al; je <else>`（4.1 对 call 结果做 bool 物化后再分支），
        // 三目形在 4.4 恒折直跳 ⇒ 改 if/else 两臂（判据 P133：独立守卫/两臂形）。
        if (GetClearAvatar((unsigned char)buf32.m_b) != 0)
        {
            // [G8InvenPvP · P92] ORIG 0x08508dee `mov -0x42(%ebp),%eax` = 从**已写
            // buf 字段**（+0x04 = d）读回，非形参 d（形参版少一次槽读、装载源不同）。
            buf32.m_field12 = buf32.m_d;
        }
        else
        {
            buf32.m_field12 = 0;
        }
        if (GetClearAvatar((unsigned char)buf32.m_c) != 0)
        {
            // 同 P92：ORIG 0x08508e1c `mov -0x3e(%ebp),%eax`（+0x08 = e）。
            buf32.m_field0e = buf32.m_e;
        }
        else
        {
            buf32.m_field0e = 0;
        }
        guard->put_binary((char*)&buf32, 0x16);
        break;
    }
    case 0x33:
    {
        // [G8InvenPvP 2026-09-10 · P161/P311 组件视图 + 字段错位修正] ORIG
        // 0x08508e52/0x08508e5c/0x08508e65：+0x00 常量 1、+0x04 characNo、
        // **(char)b 写 +0x5a4**（`mov %al,-0x1b5(%ebp)`，buf 基址 -0x759
        // ⇒ -0x1b5 = buf+0x5a4），buf+0x08..+0x5a3 不写（ORIG 原样发送）。
        struct AvatarEventBuf33
        {
            int m_one;           // +0x00
            int m_characNo;      // +0x04
            char m_unused[0x5a4 - 0x08];   // +0x08..+0x5a3（ORIG 不写）
            char m_b;            // +0x5a4
            char m_tail[0x70c - 0x5a5];
        } __attribute__((packed));
        AvatarEventBuf33 buf33;
        buf33.m_one = 1;
        buf33.m_characNo = characNo;
        buf33.m_b = (char)b;
        guard->put_binary((char*)&buf33, 0x70c);
        break;
    }
    case 0x34:
    {
        // [G8InvenPvP 2026-09-10 · P161/P311 组件视图，同 case 0x31] ORIG
        // 0x08508e??：+0x00 characNo(int)、+0x04 (char)b、+0x05 (short)c（非对齐）。
        struct AvatarEventBuf34
        {
            int m_characNo;      // +0x00
            char m_b;            // +0x04
            short m_c;           // +0x05（非对齐）
        } __attribute__((packed));
        AvatarEventBuf34 buf34;
        buf34.m_characNo = characNo;
        buf34.m_b = (char)b;
        buf34.m_c = (short)c;
        guard->put_binary((char*)&buf34, 7);
        break;
    }
    case 0x36:
    {
        // ORIG GetInBuffer<SIG_AVATAR_CHANGE_STAT>（0x08508ed2 段，sizeof 0x51）：
        // +0x00=b(int ui_id)、+0x04=c(int it_id)、+0x08=(char)d(slot)、
        // +0x09=characNo(int 非对齐 movl)。（参数 e 未用）
        SIG_AVATAR_CHANGE_STAT* s =
            guard->GetInBuffer<SIG_AVATAR_CHANGE_STAT>();
        s->m_uiId = b;
        s->m_itId = c;
        s->m_slot = (unsigned char)d;
        s->m_characNo = characNo;
        break;
    }
    case 0x2c3:
    {
        // ORIG 0x08508f0f：GetInBuffer<SIG_AVATAR_OPTION_CHANGE> 就地写域；
        // **写入序 = characNo(+8) → uiId(+0) → (char)c(+4)**（0x08508f25/0x08508f2e/
        // 0x08508f36，P324 书写序=物理序）。
        SIG_AVATAR_OPTION_CHANGE* p =
            guard->GetInBuffer<SIG_AVATAR_OPTION_CHANGE>();
        p->m_characNo = characNo;
        p->m_uiId = b;
        p->m_field4 = (char)c;
        break;
    }
    case 0x31a:
    {
        // ORIG 0x08508f43：GetInBuffer<SIG_AVATAR_COLOR_CHANGE> 就地写域。
        SIG_AVATAR_COLOR_CHANGE* p =
            guard->GetInBuffer<SIG_AVATAR_COLOR_CHANGE>();
        p->m_characNo = characNo;
        p->m_uiId = b;
        p->m_field8 = (short)c;
        p->m_fieldA = (short)d;
        break;
    }
    default:
        break;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
