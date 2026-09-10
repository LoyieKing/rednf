// [G7-sub 2026-09-11 判词] UdpThread::_processUDPData（ORIG T 0x0857c5a4）：私有 .o 实测
// ORIG 267 / 我方 262（gap 5，我方短）。逐条对照（fdiff --counter --shape）后
// ORIG-only 项**全部**为：`nop`×3（对齐填充，§6.1 豁免）+ 符号/字符串重定位文本
// （我方 .o 侧无解析名）；无缺失语义指令。p167 调用点差集 CLEAR（51/51，且我方
// 3 处 CStreamGuard 序列与 ORIG 逐条同形：Acquire 直作 ctor 实参，无具名 stream 局部）。
// 结论：**封顶，无改动**（残差=对齐 nop + N82/N78 族）。
void UdpThread::_processUDPData(char* data, int len)
{
    // 跨服 UDP 包 = 10 字节 PacketHeader 线格式（shared/packet/include/PacketHeader.h）；
    // ORIG 0857c5ad：每处读都重取形参（mov 0xc(%ebp),%eax; movzwl (%eax)，P86 逐次重读）
    // → +0x00 ushort packetId。
    // [GlobalData-判据①] ORIG 0x0857c5b5 段：packetId 在 CPacketTracerInstance()
    // **之前**求值并升格 %ebx 跨 call 保活（`movzwl (%eax),%eax; movzwl %ax,%ebx;
    // call CPacketTracerInstance; mov %ebx,0x8(%esp)`）⇒ 原码是单语句链式实参
    // （P52/P226），无 `uVar1`/`tracer` 具名局部（旧形多 -0x36/-0x34 两槽 + 3 条）。
    CPacketTracerInstance()->AddUdpLog(
        3, (int)reinterpret_cast<PacketHeader*>(data)->packetId);

    static CPacketCounter<1000, 10240> packet_counter(
        G_CEnvironment()->get_file_name(), "InterDispatcher");
    packet_counter.IncrementPacketCount(
        (int)reinterpret_cast<PacketHeader*>(data)->packetId);

    // [GlobalData-判据①] ORIG 0x0857c68d `call dispatchPacket; test %al,%al;
    // jne +0x42a`（jne 落空 then 岛；无 `xor $1`）⇒ 原码是**正向 if + 空 then +
    // else 体**（探针 /tmp/l7/probe3.cpp t2 逐条同形）；旧写法 `if (!f()){体}`
    // （t1/t3/t4/t5）4.4 恒产 `xor $1,%eax; test %al,%al; je`，多一条 xor 且
    // 分支落点从空岛变成 epilogue 本体。
    if (CDispatchServerPacket::dispatchPacket(data))
    {
    }
    else
    {
        // [GlobalData-判据①] ORIG 0x0857c696 族逐次重读 `mov 0xc(%ebp),%eax;
        // movzbl (%eax),%eax; movsbl %al,%eax; cmp $0x1,%eax`（符号扩展后直接比，
        // 无栈槽往返）⇒ 原码**无具名 char 局部**（P0/P86 表达式直读）；旧写法
        // `char cVar2 = *data;` 产 `mov %al,-0x2d; cmpb $0x1,-0x2d` 槽化两段。
        // [L8 2026-09-10 判据①/P251/P261/判据⑨ 旧判推翻] ORIG 0x0857c696 段是
        // **switch(*data) 决策树**：`movzbl (%eax),%eax; movsbl %al,%eax;
        // cmp $0x1,%eax; je L63; cmp $0x1,%eax; jg L4; test %eax,%eax; je L7;
        // jmp L110; cmp $0x2,%eax; je L185; cmp $0x3,%eax; je L121`（case 1/0/2/3
        // + default 全 4 支）。旧 if/else 链 4.4 产 `cmp $0x1,%al; jne <case1>`
        // 二段形，结构与叶块序均不符 ⇒ 改 switch 复现（4.1 少 case 用线性比较链，P17）。
        switch (*data)
        {
        case 0:
        {
            char* local_24 = data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0xe8), true);
            **guard << 0x3d;
            **guard << -1;
            guard->put_binary(local_24, 0x52);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return;
        }
        // [L8 P324 叶块序] ORIG 决策树 `cmp $1;je +63`（case1 体远在 +63=i128）、
        // 树后紧跟 i76 起的是 **case 0 体** ⇒ 书写序 case0 在前、case1 在后。
        case 1:
        {
            // [L8 2026-09-10 P259] ORIG 0x0857c6ea 段：Acquire 返回值直接经 %eax 传
            // CStreamGuard ctor（`call Acquire; movl $0x1,0x8(%esp); mov %eax,0x4(%esp);
            // lea -0x28(%ebp),%eax; call ctor`），**无 stream 具名局部** ⇒ 我方
            // `Stream* stream = …` 每分支多发一个 4B 槽（×3 分支 = 0xc），把三个
            // CStreamGuard 槽整体顶深 0xc（我方 -0x34/-0x3c/-0x44 vs ORIG -0x28/-0x30/-0x38）
            // 并使帧 0x5c vs ORIG 0x4c。改为内联实参。
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0xf8), true);
            **guard << 0x9f;
            **guard << -1;
            // 线格式负载切块：header(+0x00..0x01) 之后原始字节，ORIG lea 0x2(%eax)（P156 豁免）。
            guard->put_binary(data + 2, len);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 2:
            return;
        case 3:
        {
            GlobalData::s_auction_proxy->SetRunning(false);
            GlobalData::s_cera_auction_proxy->SetRunning(false);
            char* local_20 = data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("NetworkThread.cpp", 0x126), true);
            **guard << 0x3d;
            **guard << -1;
            guard->put_binary(local_20, 0x52);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            return;
        }
        default:
            break;
        }
        if (reinterpret_cast<PacketHeader*>(data)->packetId < 1000)
        {
            LogManager::logFormat(1, "NetworkThread.cpp", __PRETTY_FUNCTION__, 0x112,
                                  "ABNORMAL UDP PACKET [%d]", (int)*data);
        }
    }
