// ORIG reqValue 0x084b368a：repeat 事件(106)有效且连线 <3 时抽新棋盘值，
// 结算连线奖励并以 0x1c3 包回报当前棋盘字。
// [b28d-47] verdict=DIFF(routed) [N18b] ORIG 0x084b368a orig/ours=152/150
//   残差：`if (gained > 3) gained = 3;` 4.4 恒折内存比较+内存写
//   （`cmpl $3,-0x10; movl $3,-0x10`）vs ORIG 寄存器中转 4 条
//   （`mov slot,%eax; cmp; mov $3,%eax; mov %eax,slot`）⇒ N18b 封顶。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=4 [N18b] p167 CLEAR(24/24)；clamp 段
// 4.4 恒折 `cmpl $3,-0x10; movl $3,-0x10`，ORIG 为寄存器中转；if/三目/副本三形实测
// ndiff=4/5/42 ⇒ 取 if 形封顶（4 条内存-寄存器形差）。
// [b28d-solo-2] 复核：orig/ours=153/150，gap 2 全在 clamp 段（ORIG 5 条
// `mov slot,%eax; cmp $3,%eax; jle; mov $3,%eax; mov %eax,slot` vs 4.4 3 条内存形）。
// 追加探针 p5/p6/p8 共 12 形（if/反转/三目/副本/goto/unsigned/长整/枚举/指针写）
// 4.4 恒折内存比较+内存写 ⇒ N18b 封顶；尾部三岛（nop;jmp ×2 + nop）已对齐。
// [b28e-solo-2] 复核（fresh .o 152/150，jcc 目标逐位核对：084b36a4/36c6/36e2 三岛、
// 084b37cc/37a9/378b 循环与块出口与 266a/268c/26a8/278b/2768 逐一同目标，唯 clamp 段
// `jle 084b378c`（回写点）vs 我方 `jle 274e`（跳过内存写）形异）：clamp 原码是三目/
// min 形（无条件回写）。4.4.7 对 `x>3?3:x` 走 MIN_EXPR 发 cmovle（探针 c2/c3/c6/d6/d11），
// 对 if 恒内存比较+内存写（c1/c8/c10），unsigned 比较出 `mov slot,%eax; cmpl; jbe; movl`
// 仍无回写（c4/d2），`+0`/cast/局部副本等 11 形均不产寄存器回写 ⇒ 后端 cmov 选择差异，
// N18b/P321-T5 封顶（禁为装载序改类型）。
// [round5-A20 复核·N18b] fresh .o 147/149（gap -2）唯一残差=clamp 段 5 vs 3 条：
//   ORIG 0x084b37xx `mov -0x10(%ebp),%eax; cmp $0x3,%eax; jle; mov $0x3,%eax;
//   mov %eax,-0x10(%ebp)`（寄存器比较+常量经 eax 统一回写）vs 4.4 `cmpl $0x3,-0x10;
//   jle; movl $0x3,-0x10`（内存形）。判据⑪ PURE SHIFT c=0（frame 64=64、槽集全等）
//   ⇒ 非槽问题；前轮 12 形探针 + 本轮复核结论一致：寄存器回写形仅 `-march=i486`
//   （消 cmov/MIN_EXPR，P230①）可达，属路由域 ⇒ N18b/P321-T5 封顶。
// [round5-A20 配方②复查] **禁按 C05-F2（unsigned→int 内存直比）反向套用本件**：
//   ORIG 条件码为 `jle`（**有符号序**）⇒ gained 必为 int；改 unsigned 会得 `jbe`，
//   触发 P196「符号性/比较方向反演」真 bug 家族（值域含负时行为分叉）。本件缺的是
//   ORIG 的寄存器**回写** `mov $0x3,%eax; mov %eax,-0x10(%ebp)`，F2 只翻比较侧
//   ⇒ 无交集，保留 N18b + 路由域判词。
// [L1FreeTop12.S2 2026-09-11] §6.5 复核：本件 ORIG 无串引用（`$"` 0 条）、无 PRETTY；
//   §6.5.1 enc 全库 0 命中 ⇒ 无语义轴缺口，维持 N18b/路由域封顶。
void CBingoGmTest::reqValue()
{
    CUser* user = GetUser();
    if (user == 0)
    {
        return;
    }

    BingoEvent* event = static_cast<BingoEvent*>(
        static_cast<void*>(GlobalData::s_event_manager->GetRepeatEvent(0x6a)));
    if (event == 0)
    {
        return;
    }

    // [b23S3C] ORIG +0x48 单次 getBingoData→movzbl+cmp$2,setg 寄存器直测（无
    // lines 具名槽）；+0x74 每次 calNewValue 第 1 实参 = 独立 getBingoData() 的
    // bd 指针直传 uint&（ORIG mov %eax,%ebx；无 board 拷贝槽 -0x30）；
    // receiver 链 shared_ptr::operator-> 真调用（+0x8f）。
    if (static_cast<signed char>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4) > 2)
    {
        return;
    }

    int oldBoard = static_cast<int>(
        static_cast<BingoData*>(user->getBingoData())->m_eventId);
    int newValue = (*reinterpret_cast<boost::shared_ptr<CBingo>*>(
                        event->getBingoSystem()))
                       ->calNewValue(
                           static_cast<BingoData*>(user->getBingoData())
                               ->m_eventId,
                           m_subCmd);

    if (*reinterpret_cast<int*>(user->getBingoData()) != oldBoard)
    {
        static_cast<BingoData*>(user->getBingoData())->m_needSave = true;
        int gained = newValue - static_cast<int>(
            static_cast<BingoData*>(user->getBingoData())->m_pad4);
        if (gained > 0)
        {
            // [b23S3C] ORIG +0x0f5 分支形 clamp（读槽→cmp reg→jle→mov $3→写回）。
            // [b29-Lane21] 实测：if 形残差 4 行（4.4 折 `cmpl $3,-0x10; movl $3,-0x10`）；
            //   三目形 `(gained>3)?3:gained` 残差 5 行（4.4 发 `cmovle -0x10,%eax`，本 TU
            //   无 -march=i486 ⇒ cmov 族不可消，P230①/Main 路由域）⇒ 保留 if 形。
            //   残差 = N18b/P321-T5（禁为装载序改类型）：ORIG `mov -0x10,%eax; cmp $3,%eax;
            //   jle; mov $3,%eax; mov %eax,-0x10`（寄存器比较 + 常量经 eax 统一写回）。
            if (gained > 3)
                gained = 3;
            static_cast<BingoData*>(user->getBingoData())->m_pad4 =
                static_cast<char>(newValue);
            for (int i = 0; i < gained; ++i)
            {
                event->sendOneMatchLineReward(*user);
            }
        }
    }

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x1c3);
    packet.put_byte(m_subCmd);
    packet.put_int(*reinterpret_cast<int*>(user->getBingoData()));
    packet.finalize(true);
    user->Send(packet);
}